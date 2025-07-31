#include "CCarEntryTask.h"

CCarEntryTask::CCarEntryTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessType = 8;
	headBack.bussinessLength = sizeof(CommonBack);
	headBack.crc = this->clientFd;
}

void CCarEntryTask::work()
{
	cout << "CCarEntryTask正在执行" << endl;


	//数据解析
	HEAD head;
	CarEntryRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	Parking parkingInfo(Parking::PARKINGTYPE::ENTRY, request.account, request.carNumber, request.entryTime, request.entryPosition);
	cout << "+++++++CarEntryRequest详细信息+++++++" << endl;
	cout << "account=" << request.account << " carNumber=" << request.carNumber << endl;
	cout << "entryTime=" << request.entryTime << " entryPosition=" << request.entryPosition << endl;
	string entryTime2 = request.entryTime;
	cout << "entryTime2=" << entryTime2 << " parkingInfo.getEntryTime()=" << parkingInfo.getEntryTime() << endl;
	//获取入场图片id
	string sql = "select id from picture_info where type = 1 and account='"+ parkingInfo.getAccount() + "' and createtime='" 
		+ parkingInfo.getEntryTime() + "';";
	unique_ptr<vector<unique_ptr<Picture>>> resVec = OperationFactory::getInstance()->createRepository(
		OperationFactory::RepositoryType::PICTURE)->query<Picture>(sql);
	CBaseOperation* parkOp = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING);

	if (resVec->size() == 1) {
		//查询成功
		unique_ptr<Picture>& pic = resVec->front();
		cout << "入场图片查询成功id=" << pic->getId() << endl;
		parkingInfo.setEntryPicId(pic->getId());
		int rows = parkOp->doInsert(&parkingInfo);
		if (rows > 0) {
			cout << "入场成功" << endl;
		}
		else {
			cout << "入场失败" << endl;
		}
		
	}
	else {
		//查询失败
		cout << "入场图片查询成功失败sql=" << sql << endl;
		cout << "无法入场" << endl;
	}

	//查询该用户的当天的最新的四条入场记录，只有入场图片查询成功且入场记录插入成功的情况下才会产生最新的入场记录，否则是老记录
	string beginTime = string(request.entryTime, 0, 11) + "00:00:00";
	string endTime = string(request.entryTime, 0, 11) + "23:59:59";
	string sql3 = "select id,entryTime,entryPosition,carNumber from " + parkOp->getTablename() +
		" where `account`= '" + parkingInfo.getAccount() + "' and entryTime>='" + beginTime + "' and entryTime<='" + endTime +
		"' ORDER BY entryTime desc LIMIT 4;";
	unique_ptr<vector<unique_ptr<Parking>>> resVec3 = parkOp->query<Parking>(sql3);
	bodyBack.validNum = resVec3->size();
	for (int i = 0; i < resVec3->size(); i++) {
		auto& ele = (*resVec3)[i];
		bodyBack.carEntryArr[i].id = ele->getId();
		strcpy(bodyBack.carEntryArr[i].carNumber, ele->getCarNumber().c_str());
		strcpy(bodyBack.carEntryArr[i].entryTime, ele->getEntryTime().c_str());
		strcpy(bodyBack.carEntryArr[i].entryPosition, ele->getEntryPosition().c_str());
	}
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
