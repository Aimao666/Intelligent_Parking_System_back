#include "CParkingInfoTask.h"

CParkingInfoTask::CParkingInfoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
	headBack.bussinessType = 22;
	headBack.crc = this->clientFd;
}

void CParkingInfoTask::work()
{
	cout << "CParkingInfoTask车辆信息查询" << endl;
	//数据解析
	memcpy(&head, taskData, sizeof(HEAD));
	ParkingInfoRequest request;
	memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
	CBaseOperation* parkop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING);
	CBaseOperation* picop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PICTURE);
	string sql = "select * from " + parkop->getTablename() + " where account='" + string(request.account) + "'";
	if (strcmp(request.carNumber, "")) {
		sql = sql + " and carNumber like '%" + request.carNumber + "%'";
	}
	if (strcmp(request.entryTime, "")) {
		sql = sql + " and entryTime >= '" + request.entryTime + "'";
	}
	if (strcmp(request.leaveTime, "")) {
		sql = sql + " and leaveTime <= '" + request.leaveTime + "'";
	}
	bodyBack.currentPage = request.currentPage;
	int pageSize = sizeof(bodyBack.parkingInfoDataArr) / sizeof(ParkingInfoData);//每一页的数据量
	int offset = (request.currentPage - 1) * pageSize;//偏移量
	sql = sql + " limit " + CTools::itos(pageSize) + " offset " + CTools::itos(offset);
	auto resVec = parkop->query<Parking>(sql);
	bodyBack.num = 0;
	if (resVec) {
		bodyBack.num = resVec->size();
		for (int i = 0; i < resVec->size(); ++i) {
			auto& ele = (*resVec)[i];
			bodyBack.parkingInfoDataArr[i].id = ele->getId();
			bodyBack.parkingInfoDataArr[i].dueCost = ele->getDueCost();
			bodyBack.parkingInfoDataArr[i].reallyCost = ele->getReallyCost();
			strcpy(bodyBack.parkingInfoDataArr[i].carNumber, ele->getCarNumber().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].entryTime, ele->getEntryTime().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].leaveTime, ele->getLeaveTime().c_str());
			auto picVec = picop->query<Picture>("select * from " + picop->getTablename() + " where id='" + 
				CTools::itos(ele->getEntryPicId()) + "' or id='" + CTools::itos(ele->getLeavePicId()) + "';");
			if (picVec) {
				for (int j = 0; j < picVec->size(); ++j) {
					auto& pic = (*picVec)[j];
					if (pic->getId() == ele->getEntryPicId()) {
						strcpy(bodyBack.parkingInfoDataArr[i].entryPicName, pic->getName().c_str());
						strcpy(bodyBack.parkingInfoDataArr[i].entryPicPath, pic->getKhdPath().c_str());
					}
					else if (pic->getId() == ele->getLeavePicId()) {
						strcpy(bodyBack.parkingInfoDataArr[i].leavePicName, pic->getName().c_str());
						strcpy(bodyBack.parkingInfoDataArr[i].leavePicPath, pic->getKhdPath().c_str());
					}
				}
			}
		}
	}

	cout << "+++++++ParkingInfoBack详细信息+++++++" << endl;
	cout << "请求页=" << bodyBack.currentPage << " 有效数据个数=" << bodyBack.num << endl;
	cout << "sql=" << sql << endl;
	cout << "++++++++++++++++++++" << endl;

	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
