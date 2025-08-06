#include "CFileUploadTask.h"


CFileUploadTask::CFileUploadTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}


//文件上传业务，接收一个文件碎片
void CFileUploadTask::work()
{
	cout << "CFileUploadTask文件碎片上传" << endl;
	FileInfoRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
	FileInfo::FileKey fileKey{ request.account,request.filename };

	//根据当前碎片有效字节数来从请求包中取数据
	int fileLength = request.fileLength;
	if (fileLength > request.totalLength||fileLength>sizeof(request.context)) {
		cout << "当前碎片有效数据长度超长，fileLength=" << fileLength << endl;
		return;
	}
	char context[fileLength];
	memcpy(context, request.context, fileLength);
	FileInfo* fileInfo = nullptr;
	pthread_mutex_lock(&DataManager::allFileMapMutex);
	if (DataManager::allFileMap.find(fileKey) == DataManager::allFileMap.end()) {
		//该包是第一个包，创建文件信息类
		cout << "第一包" << endl;
		fileInfo = new FileInfo(request.account, request.filename,
			request.totalNumber, request.totalLength, clientFd, request.type, request.createtime, request.khdPath);
		DataManager::allFileMap.insert(make_pair(fileKey, unique_ptr<FileInfo>(fileInfo)));
		/*DataManager::allFileMap[fileKey] = unique_ptr<FileInfo>(new FileInfo(request.account, request.filename,
			request.totalNumber,request.totalLength, clientFd));*/
	}
	else {
		fileInfo = DataManager::allFileMap[fileKey].get();
	}
	pthread_mutex_unlock(&DataManager::allFileMapMutex);
	//保存文件碎片
	pthread_mutex_lock(&fileInfo->fileContextMapMutex);
	//!!!!!!有坑！！！！！string的构造函数会认为context是一个C字符串（以'\0'结尾），
	//所以如果我们的二进制数据中包含'\0'，那么string只会复制到第一个'\0'之前的内容，导致二进制数据被截断
	string contextStr(context, fileLength);
	auto resPair = fileInfo->getFileContextMap().insert(pair<int, string>(request.fileIndex, contextStr));
	//DataManager::allFileMap[fileKey]->getFileContextMap()[request.fileIndex] = string(context);
	pthread_mutex_unlock(&fileInfo->fileContextMapMutex);
	if (resPair.second) {
		cout << fileInfo->getFilename() << "文件序号:" << resPair.first->first << "保存成功" << endl;
	}
	else {
		cout << fileInfo->getFilename() << "文件序号:" << resPair.first->first << "保存失败，已存在，原先保存字节长度:" <<
			resPair.first->second.size() << endl;
	}

	// 请求日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：文件碎片上传\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n"
			<< "文件名称：" << request.filename << "\n"
			<< "文件客户端路径：" << request.khdPath << "\n"
			<< "文件总碎片个数：" << request.totalNumber << "\n"
			<< "文件总大小：" << request.totalLength << "\n"
			<< "文件创建时间：" << request.createtime << "\n"
			<< "文件类型1入场2出场3截图：" << request.type << "\n"
			<< "文件序号：" << request.fileIndex << "\n"
			<< "碎片有效长度：" << request.fileLength << "\n";
		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}

}