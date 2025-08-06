#pragma once
#include "CBaseTask.h"
#include <pthread.h>
#include <memory>
#include "CLoginTask.h"
#include "RegisterTask.h"
#include "CFileCheckTask.h"
#include "CFileUploadTask.h"
#include "CCarEntryTask.h"
#include "CCarLeaveTask.h"
#include "CAgreeLeaveTask.h"
#include "CUploadVideoTask.h"
#include "CUpdateCarNumberTask.h"
#include "CVideoDateTask.h"
#include "CVideoListTask.h"
#include "CUploadPlayinfoTask.h"
#include "CParkingInfoTask.h"
#include "COffLineTask.h"
#include "CHeartServerTask.h"
using namespace std;
class CTaskFactory
{
public:
	~CTaskFactory() = default;
	static CTaskFactory* getInstance();
	unique_ptr<CBaseTask> createTask(int clientFd, int bussinessType, char* data, size_t length);
private:
	CTaskFactory();
	static CTaskFactory* instance;
	static pthread_mutex_t mutex;

};

