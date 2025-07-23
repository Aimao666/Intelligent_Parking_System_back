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
	LoginRequest loginRequest;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&loginRequest, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "登录请求-账号:" << loginRequest.account << "	密码:" << loginRequest.password << endl;
	
	//数据库查询
	CBaseOperation* userop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::USER);
	unique_ptr<vector<unique_ptr<User>>>vec = userop->query<User>("select * from " + userop->getTablename() + " where account = `" +
		loginRequest.account + "` and password = `" + loginRequest.password + "`;");
	if (vec != nullptr && vec->size() > 0) {
		cout << "登陆成功" << endl;
	}
	else {
		cout << "登陆失败" << endl;
	}
	
}
