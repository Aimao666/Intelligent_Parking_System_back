#include "RegisterTask.h"

RegisterTask::RegisterTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

void RegisterTask::work()
{
	cout << "RegisterTask正在执行" << endl;
	//数据解析
	HEAD head;
	RegisterRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "注册请求-账号:" << request.account << endl;

	//数据库查询
	CBaseOperation* userop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::USER);
	User user(request.account,"e10adc3949ba59abbe56e057f20f883e");

	//准备好报文发给前置服务器，只有前置服务器才有网，才能发给客户端
	HEAD headBack;
	CommonBack bodyBack;
	headBack.bussinessType = 6;
	headBack.bussinessLength = sizeof(CommonBack);
	headBack.crc = this->clientFd;
	int res = userop->doInsert(&user);
	if (res > 0) {
		bodyBack.flag = 1;
		sprintf(bodyBack.message, "用户%s注册成功", request.account);
		//注册成功则创建一个该用户文件夹
		string dirPath = DataManager::basePath + string(request.account)+"/pictures/";
		//!!!mkdir只能够创建单级目录，不能创建多级目录，因此最好的选择时递归创建
		if (CTools::createDirectoryRecursive(dirPath)) {
			cout << "创建目录成功:" << dirPath << endl;
		}
		else if (res == -1) {
			perror("mkdir err");
		}
	}
	else {
		//账号已存在无法注册
		bodyBack.flag = 0;
		sprintf(bodyBack.message, "用户%s注册失败", request.account);
	}
	cout << bodyBack.message << endl;
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
