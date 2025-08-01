#include "CUpdateCarNumberTask.h"

CUpdateCarNumberTask::CUpdateCarNumberTask(int fd, char* data, size_t len) 
	:CBaseTask(fd, data, len)
{
}

void CUpdateCarNumberTask::work()
{
    cout << "CUpdateCarNumberTask修改车牌请求" << endl;
	//数据解析
	HEAD head;
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
}
