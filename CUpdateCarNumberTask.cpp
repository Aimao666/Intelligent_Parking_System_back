#include "CUpdateCarNumberTask.h"

CUpdateCarNumberTask::CUpdateCarNumberTask(int fd, char* data, size_t len) 
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
}

void CUpdateCarNumberTask::work()
{
    cout << "CUpdateCarNumberTask修改车牌请求" << endl;
	//数据解析
	UpdateCarNumberRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	CBaseOperation* op = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING);
	string sql = "update " + op->getTablename() + " set carNumber='" + string(request.newCarNumber) +
		"' where account='" + string(request.account) + "' and entryTime='" + string(request.entryTime) +
		"' and carNumber='" + string(request.oldCarNumber) + "';";
	int rows = op->executeUpdate(sql);
	//数据库修改车牌信息
	if (rows > 0) {
		cout << "修改车牌信息保存成功" << endl;
		bodyBack.flag = 1;
		strcpy(bodyBack.message, "修改车牌信息保存成功");
	}
	else {
		cout << "修改车牌信息保存失败" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "修改车牌信息保存失败");
	}
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
			<< "功能：修改车牌\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n"
			<< "入场时间：" << request.entryTime << "\n"
			<< "错误的车牌号：" << request.oldCarNumber << "\n"
			<< "正确的车牌号：" << request.newCarNumber << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}

	// 响应日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：上传视频录制信息\n"
			<< "类型：响应\n"
			<< "用户账号：" << request.account << "\n"
			<< "结果标志：" << bodyBack.flag << "\n"
			<< "说明：" << bodyBack.message << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
}
