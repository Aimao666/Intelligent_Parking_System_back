#include "CCarLeaveTask.h"

CCarLeaveTask::CCarLeaveTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessType = 10;
	headBack.bussinessLength = sizeof(bodyBack);
	headBack.crc = this->clientFd;
}

void CCarLeaveTask::work()
{
	cout << "CFileCheckTask正在执行" << endl;
	//数据解析
	HEAD head;
	CarLeaveRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	Parking parkingInfo(Parking::PARKINGTYPE::LEAVE, request.account, request.carNumber, request.leaveTime, request.leavePosition);
	//寻找入场记录
	string sql1 = "select * from parking_info where account='" + parkingInfo.getAccount() + "' and carNumber='" + 
		parkingInfo.getCarNumber() + "' order by id desc;";
	auto resVec1 = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING)->query<Parking>(sql1);
	if (resVec1->size() > 0) {
		cout << "找到入场记录" << endl;
		auto& tmpParking = resVec1->front();
		parkingInfo.setId(tmpParking->getId());
		parkingInfo.setEntryPicId(tmpParking->getEntryPicId());
		parkingInfo.setEntryTime(tmpParking->getEntryTime());
		parkingInfo.setEntryPosition(tmpParking->getEntryPosition());

		//获取出场图片id
		string sql2 = "select id from picture_info where type = 2 and account='" + parkingInfo.getAccount() + "'and createtime='"
			+ parkingInfo.getLeaveTime() + "';";
		unique_ptr<vector<unique_ptr<Picture>>> resVec2 = OperationFactory::getInstance()->createRepository(
			OperationFactory::RepositoryType::PICTURE)->query<Picture>(sql2);

		if (resVec2->size() == 1) {
			//查询成功
			unique_ptr<Picture>& pic = resVec2->front();
			cout << "出场图片查询成功id=" << pic->getId() << endl;
			parkingInfo.setLeavePicId(pic->getId());
			time_t leaveTimeStamp = CTools::convertTimeStr2TimeStamp(parkingInfo.getLeaveTime());//时间字符串转时间戳
			time_t entryTimeStamp = CTools::convertTimeStr2TimeStamp(parkingInfo.getEntryTime());
			time_t diffTime = leaveTimeStamp - entryTimeStamp;//时间差
			int costHour = (diffTime + 3599) / 3600;//消费时长，小时计
			//计算应付金额
			parkingInfo.setDueCost(costHour * 5);//每小时5元
			int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING)->doUpdate(&parkingInfo);
			bodyBack.dueCost = costHour * 5;
			strcpy(bodyBack.carNumber, request.carNumber);
			strcpy(bodyBack.entryTime, parkingInfo.getEntryTime().c_str());
			bodyBack.mesc = diffTime;
			if (rows > 0) {
				cout << "出场信息保存成功" << endl;
			}
			else {
				cout << "出场信息保存失败" << endl;
			}
		}
		else {
			//查询失败
			cout << "出场图片查询成功失败sql=" << sql2 << endl;
			cout << "无法出场" << endl;
		}
	}
	else {
		cout << "未找到入场记录" << endl;
		bodyBack.dueCost = 0;
		strcpy(bodyBack.carNumber, request.carNumber);
		strcpy(bodyBack.entryTime, "");
		bodyBack.mesc = 0;
	}


	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
