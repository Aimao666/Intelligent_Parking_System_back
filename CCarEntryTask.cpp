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

	//获取入场图片id
	string sql = "select id from picture_info where type = 1 and account='"+ parkingInfo.getAccount() + "'and createtime='" 
		+ parkingInfo.getEntryTime() + "';";
	unique_ptr<vector<unique_ptr<Picture>>> resVec = OperationFactory::getInstance()->createRepository(
		OperationFactory::RepositoryType::PICTURE)->query<Picture>(sql);

	if (resVec->size() == 1) {
		//查询成功
		unique_ptr<Picture>& pic = resVec->front();
		cout << "入场图片查询成功id=" << pic->getId() << endl;
		parkingInfo.setEntryPicId(pic->getId());
		int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING)->doInsert(&parkingInfo);
		if (rows > 0) {
			cout << "入场成功" << endl;
			bodyBack.flag = 1;
			strcpy(bodyBack.message, "入场信息保存成功");
		}
		else {
			cout << "入场失败" << endl;
			bodyBack.flag = 0;
			strcpy(bodyBack.message, "入场信息保存失败");
		}
	}
	else {
		//查询失败
		cout << "入场图片查询成功失败sql=" << sql << endl;
		cout << "无法入场" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "入场图片查询成功失败,无法入场");
	}

	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
