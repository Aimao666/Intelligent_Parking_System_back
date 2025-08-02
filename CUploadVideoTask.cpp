#include "CUploadVideoTask.h"

CUploadVideoTask::CUploadVideoTask(int fd, char* data, size_t len)
    :CBaseTask(fd, data, len)
{
    headBack.bussinessType = 14;
    headBack.bussinessLength = sizeof(bodyBack);
    headBack.crc = this->clientFd;
}

void CUploadVideoTask::work()
{
    cout << "CUploadVideoTask正在执行" << endl;
	//数据解析
	UploadVideoRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);


	Video videoInfo(request.account, request.vname, request.vpath, request.totaltime, request.createtime, request.pname, request.ppath);

	//将视频信息插入数据库
	int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::VIDEO)->doInsert(&videoInfo);
	if (rows > 0) {
		cout << "视频信息保存成功" << endl;
		bodyBack.flag = 1;
		strcpy(bodyBack.message, "视频信息保存成功");
	}
	else {
		cout << "视频信息保存失败" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "视频信息保存失败");
	}
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
