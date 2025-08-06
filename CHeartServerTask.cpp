#include "CHeartServerTask.h"

CHeartServerTask::CHeartServerTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CHeartServerTask::work()
{
    cout << "CHeartServerTask心跳服务" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    HeartServerRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
	// 请求日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：心跳服务\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n";
		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
}
