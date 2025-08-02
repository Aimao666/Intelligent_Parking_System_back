#include "CTaskFactory.h"
CTaskFactory* CTaskFactory::instance = nullptr;
pthread_mutex_t CTaskFactory::mutex;

CTaskFactory* CTaskFactory::getInstance()
{
	pthread_mutex_lock(&mutex);
	if (instance == nullptr) {
		instance = new CTaskFactory();
	}
	pthread_mutex_unlock(&mutex);
	return instance;
}

unique_ptr<CBaseTask> CTaskFactory::createTask(int clientFd, int bussinessType, char* data, size_t length)
{
	switch (bussinessType)
	{
	case 1: //登录
	{	
		unique_ptr<CLoginTask> task(new CLoginTask(clientFd, data, length));
		return task;
	}

	case 5://注册
	{
		unique_ptr<RegisterTask> task(new RegisterTask(clientFd, data, length));
		return task;
	}
	case 7://入场请求
	{
		unique_ptr<CCarEntryTask> task(new CCarEntryTask(clientFd, data, length));
		return task;
	}
	case 9://出场请求
	{
		unique_ptr<CCarLeaveTask> task(new CCarLeaveTask(clientFd, data, length));
		return task;
	}
	case 11://放行请求
	{
		unique_ptr<CAgreeLeaveTask> task(new CAgreeLeaveTask(clientFd, data, length));
		return task;
	}
	case 13://上传录制视频信息请求
	{
		unique_ptr<CUploadVideoTask> task(new CUploadVideoTask(clientFd, data, length));
		return task;
	}
	case 15://获取服务器所存储的视频日期 
	{
		unique_ptr<CVideoDateTask> task(new CVideoDateTask(clientFd, data, length));
		return task;
	}
	case 17://获取视频播放列表请求体 
	{
		unique_ptr<CVideoListTask> task(new CVideoListTask(clientFd, data, length));
		return task;
	}
	case 19://上传视频播放信息
	{
		unique_ptr<CUploadPlayinfoTask> task(new CUploadPlayinfoTask(clientFd, data, length));
		return task;
	}
	case 21://车辆信息查询请求体
	{
		unique_ptr<CParkingInfoTask> task(new CParkingInfoTask(clientFd, data, length));
		return task;
	}
	case 23://文件上传单个碎片包
	{
		unique_ptr<CFileUploadTask> task(new CFileUploadTask(clientFd, data, length));
		return task;
	}
	case 25://文件上传确认包
	{
		unique_ptr<CFileCheckTask> task(new CFileCheckTask(clientFd, data, length));
		return task;
	}
	case 27://修改车牌请求
	{
		unique_ptr<CUpdateCarNumberTask> task(new CUpdateCarNumberTask(clientFd, data, length));
		return task;
	}
	default:
		cout << "CTaskFactory未知的case:" << bussinessType << endl;
		break;
	}
	return nullptr;
}

CTaskFactory::CTaskFactory()
{
	pthread_mutex_init(&mutex, NULL);
}
