#include "RegisterTask.h"

RegisterTask::RegisterTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

RegisterTask::~RegisterTask()
{
}

void RegisterTask::work()
{
	cout << this->taskData << "正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	RegisterRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "注册请求-账号:" << request.account << endl;

	//数据库查询
	CBaseOperation* userop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::USER);
	User* user = new User(request.account,"e10adc3949ba59abbe56e057f20f883e");
	
	int res = userop->doInsert(user);
	if (res > 0) {
		cout << "注册成功" << endl;
	}
	else {
		//账号已存在无法注册
		cout << "注册失败" << endl;
	}
}
