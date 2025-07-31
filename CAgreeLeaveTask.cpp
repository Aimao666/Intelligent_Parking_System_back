#include "CAgreeLeaveTask.h"

CAgreeLeaveTask::CAgreeLeaveTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CAgreeLeaveTask::work()
{
	cout << "CCarEntryTask正在执行" << endl;
	//数据解析
	HEAD head;
	AgreeLeaveRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);


	Parking parkingInfo;
	parkingInfo.setAccount(request.account);
	parkingInfo.setCarNumber(request.carNumber);
	//寻找入场记录
	string sql1 = "select * from parking_info where account='" + parkingInfo.getAccount() + "' and carNumber='" +
		parkingInfo.getCarNumber() + "' order by id desc;";
	auto resVec1 = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING)->query<Parking>(sql1);
	if (resVec1->size() > 0) {
		cout << "找到入场记录" << endl;
		auto& tmpParking = resVec1->front();
		parkingInfo.setId(tmpParking->getId());
		parkingInfo.setReallyCost(request.reallyCost);
		auto parkOp = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING);
		string tablename = parkOp->getTablename();
		string sql2 = "update " + tablename + " set reallyCost = " + CTools::itos(parkingInfo.getReallyCost()) +
			" where id = " + CTools::itos(parkingInfo.getId());
		int rows = parkOp->executeUpdate(sql2);
		if (rows > 0) {
			cout << "缴费信息保存成功" << endl;
			bodyBack.flag = 1;
			strcpy(bodyBack.message, "缴费信息保存成功");
		}
		else {
			cout << "缴费信息保存失败" << endl;
			bodyBack.flag = 0;
			strcpy(bodyBack.message, "缴费信息保存失败");
		}
	}
	else {
		cout << "未找到出入场记录" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "未找到出入场记录");
	}
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
