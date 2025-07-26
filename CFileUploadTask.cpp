#include "CFileUploadTask.h"


CFileUploadTask::CFileUploadTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}


//文件上传业务，接收一个文件碎片
void CFileUploadTask::work()
{
	cout << "CFileUploadTask正在执行" << endl;
	HEAD head;
	FileInfoRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), sizeof request);
	FileInfo::FileKey fileKey{ request.account,request.filename };

	//根据当前碎片有效字节数来从请求包中取数据
	int fileLength = request.fileLength;
	char context[fileLength];
	memcpy(context, request.context, fileLength);
	
	pthread_mutex_lock(&DataManager::allFileMapMutex);
	if (DataManager::allFileMap.find(fileKey) == DataManager::allFileMap.end()) {
		//该包是第一个包，创建文件信息类
		DataManager::allFileMap.insert(make_pair(fileKey, unique_ptr<FileInfo>(new FileInfo(request.account, request.filename,
			request.totalNumber, request.totalLength, clientFd,request.type,request.createtime,request.khdPath))));
		/*DataManager::allFileMap[fileKey] = unique_ptr<FileInfo>(new FileInfo(request.account, request.filename,
			request.totalNumber,request.totalLength, clientFd));*/
	}
	pthread_mutex_unlock(&DataManager::allFileMapMutex);

	//保存文件碎片
	pthread_mutex_lock(&DataManager::allFileMap[fileKey]->fileContextMapMutex);
	auto resPair = DataManager::allFileMap[fileKey]->getFileContextMap().insert(pair<int, string>(request.fileIndex, context));
	//DataManager::allFileMap[fileKey]->getFileContextMap()[request.fileIndex] = string(context);
	pthread_mutex_unlock(&DataManager::allFileMap[fileKey]->fileContextMapMutex);
	if (resPair.second) {
		cout << DataManager::allFileMap[fileKey]->getFilename() << "文件序号:" << resPair.first->first << "保存成功" << endl;
	}
	else {
		cout << DataManager::allFileMap[fileKey]->getFilename() << "文件序号:" << resPair.first->first << "保存失败" << endl;
	}
}