#include "CLoginTask.h"

CLoginTask::CLoginTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

CLoginTask::~CLoginTask()
{
}

void CLoginTask::work()
{
	cout << this->taskData << "正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	LoginRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "登录请求-账号:" << request.account << "	密码:" << request.password << endl;
	
	//数据库查询
	CBaseOperation* userop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::USER);
	unique_ptr<vector<unique_ptr<User>>>vec = userop->query<User>("select * from " + userop->getTablename() + " where account = '" +
		request.account + "' and `password` = '" + request.password + "';");
	//准备好报文发给前置服务器，只有前置服务器才有网，才能发给客户端
	HEAD headBack;
	CommonBack bodyBack;
	headBack.bussinessType = 2;
	headBack.bussinessLength = sizeof(CommonBack);
	headBack.crc = this->clientFd;
	if (vec != nullptr && vec->size() > 0) {
		bodyBack.flag = 1;
		sprintf(bodyBack.message, "用户%s登录成功", request.account);
	}
	else {
		bodyBack.flag = 0;
		sprintf(bodyBack.message, "用户%s登录失败", request.account);
	}
	cout << bodyBack.message << endl;
	//数据存放共享内存
	IPCManager::getInstance()->saveData(this->taskData, this->dataLen, 2);
}
