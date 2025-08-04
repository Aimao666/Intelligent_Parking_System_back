#include "COffLineTask.h"
#include "CThreadPool.h"
#include "CFileCheckTask.h"
COffLineTask::COffLineTask(int fd, char* data, size_t len) 
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
}

void COffLineTask::work()
{
	cout << "COffLineTask客户端下线" << endl;
	//数据解析
	OffLineRequest request;
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	string account(request.account);
	bodyBack.flag = 1;
	if (head.crc != 0) {
		//主动下线
		pthread_mutex_lock(&DataManager::allFileMapMutex);
		for (auto& pair : DataManager::allFileMap) {
			//检查是否存在未完成的文件上传业务，如存在则进行check
			if (pair.first.account == account) {
				bodyBack.flag = 0;//需要先完成文件上传业务才允许客户端下线
				auto& fileInfo = pair.second;
				FileCheckRequest fileCheck;
				fileCheck.flag = 1;
				strcpy(fileCheck.account, fileInfo->getAccount().c_str());
				strcpy(fileCheck.filename, fileInfo->getFilename().c_str());
				HEAD checkHead;
				checkHead.crc = clientFd;
				checkHead.bussinessType = 25;
				checkHead.bussinessLength = sizeof(FileCheckRequest);
				char buf[sizeof(HEAD) + sizeof(FileCheckRequest)];
				memcpy(buf, &checkHead, sizeof(checkHead));
				memcpy(buf + sizeof(HEAD), &fileCheck, sizeof(fileCheck));
				CThreadPool::getInstance()->pushTask(unique_ptr<CBaseTask>(new CFileCheckTask(clientFd, buf, sizeof(buf))));
			}
		}
		pthread_mutex_unlock(&DataManager::allFileMapMutex);
		if (bodyBack.flag) {
			strcpy(bodyBack.message, "允许下线");
		}
		else {
			strcpy(bodyBack.message, "文件上传未完成，不允许下线");
		}

		//准备数据缓冲区
		char buffer[sizeof(HEAD) + sizeof(bodyBack)];
		memcpy(buffer, &headBack, sizeof(HEAD));
		memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
		//数据存放共享内存
		IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
	}
	else {
		//心跳超时下线
		pthread_mutex_lock(&DataManager::allFileMapMutex);
		for (auto& pair : DataManager::allFileMap) {
			if (pair.first.account == account) {
				//删除该账号对应的所有文件
				DataManager::allFileMap.erase(pair.first);
			}
		}
		pthread_mutex_unlock(&DataManager::allFileMapMutex);
		return;
	}
}
