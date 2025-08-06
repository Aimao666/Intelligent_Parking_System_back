#include "CUploadPlayinfoTask.h"

CUploadPlayinfoTask::CUploadPlayinfoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
}

void CUploadPlayinfoTask::work()
{
	cout << "CUploadPlayinfoTask上传视频播放信息" << endl;
	//数据解析
	memcpy(&head, taskData, sizeof(HEAD));
	UploadPlayinfoRequest request;
	memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
	
	Video videoInfo;
	videoInfo.setId(request.videoId);
	videoInfo.setAccount(request.account);
	videoInfo.setCurrentPlaytime(request.currenttime);
	int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::VIDEO)->doUpdate(&videoInfo);
	if (rows > 0) {
		cout << "视频播放信息更新成功" << endl;
		bodyBack.flag = 1;
		strcpy(bodyBack.message, "视频播放信息更新成功");
	}
	else {
		cout << "视频播放信息更新失败" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "视频播放信息更新失败");
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
			<< "功能：上传视频播放信息\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n"
			<< "视频id：" << request.videoId << "\n"
			<< "当前播放时长：" << request.currenttime << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}

	// 响应日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：上传视频播放信息\n"
			<< "类型：响应\n"
			<< "用户账号：" << request.account << "\n"
			<< "结果标志：" << bodyBack.flag << "\n"
			<< "说明：" << bodyBack.message << "\n";

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
}
