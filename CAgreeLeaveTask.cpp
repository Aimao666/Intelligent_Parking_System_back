#include "CAgreeLeaveTask.h"

CAgreeLeaveTask::CAgreeLeaveTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
}

void CAgreeLeaveTask::work()
{
	cout << "CAgreeLeaveTask正在执行" << endl;
	//数据解析
	AgreeLeaveRequest request;
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
		if (tmpParking->getReallyCost() != 0) {
			cout << "缴费信息保存失败，已缴费" << endl;
			bodyBack.flag = 0;
			strcpy(bodyBack.message, "缴费信息保存失败，已缴费");
		}
		else {
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
				cout << "缴费信息保存失败sql=" << sql2 << endl;
				bodyBack.flag = 0;
				strcpy(bodyBack.message, "缴费信息保存失败");
			}
		}
	}
	else {
		cout << "未找到出入场记录" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "未找到出入场记录");
	}
	cout << "+++++++CAgreeLeaveBack详细信息+++++++" << endl;
	cout << "flag=" << bodyBack.flag << " message=" << bodyBack.message << endl;
	cout << "++++++++++++++++++++" << endl;
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);

	// 请求日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：车辆放行\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n"
			<< "实付金额：" << request.reallyCost << "\n"
			<< "车牌号：" << request.carNumber << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}

	// 响应日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：车辆放行\n"
			<< "类型：响应\n"
			<< "用户账号：" << request.account << "\n"
			<< "结果标志：" << bodyBack.flag << "\n"
			<< "说明：" << bodyBack.message << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
}
