#include "CFileCheckTask.h"

CFileCheckTask::CFileCheckTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}


//一个文件发送完毕后客户端发来的确认包
void CFileCheckTask::work()
{
	//解析包
	HEAD head;
	FileCheckRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), sizeof request);
	if (request.flag <= 0) {
		cout << "FileCheckRequest:flag=" << request.flag << endl;
		return;
	}
	//业务处理
	//1.文件碎片保存
	FileInfo::FileKey fileKey{ request.account,request.filename };
	FileInfo* fileInfo = DataManager::allFileMap[fileKey].get();
	map<int, string> fileContextMap = fileInfo->getFileContextMap();
	//2.检查文件数据是否正确
	int res = checkFile(fileInfo);
	if (res == 1) {
		//3.数据库记录
		string filePath = DataManager::basePath + fileInfo->getAccount() + "/pictures/" +
			fileInfo->getFilename();
		cout << "fwqPath.size()=" << filePath.size() << endl;
		auto op = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PICTURE);
		unique_ptr<Picture> pic(new Picture(fileInfo->getFilename(), fileInfo->getKhdPath(), 
			filePath, fileInfo->getCreatetime(), fileInfo->getType(),fileInfo->getAccount()));
		res = op->doInsert(pic.get());
		HEAD headBack;
		FileSuccessBack bodyBack;
		headBack.bussinessLength = sizeof(bodyBack);
		headBack.bussinessType = 26;
		headBack.crc = this->clientFd;
		strcpy(bodyBack.filename, fileInfo->getFilename().c_str());
		if (res) {
			bodyBack.flag = 1;
			cout << "图片插入数据库成功" << endl;
		}
		else {
			bodyBack.flag = 0;
			cout << "图片插入数据库失败" << endl;
		}
		//4.数据库操作执行完毕了，将该文件在内存的内容销毁
		fileContextMap.clear();
		pthread_mutex_lock(&DataManager::allFileMapMutex);
		DataManager::allFileMap.erase(fileKey);
		pthread_mutex_unlock(&DataManager::allFileMapMutex);

	}
}
//检查文件数据是否正确
int CFileCheckTask::checkFile(FileInfo* fileInfo) {
	map<int, string> fileContextMap = fileInfo->getFileContextMap();
	int arr[100];//丢包序列数组
	int index = 0;
	//初始化
	for (int i = 0; i < 100; ++i) {
		arr[i] = -1;
	}
	int totalLength = 0;
	//1.校验文件个数
	if (fileContextMap.size() == fileInfo->getTotalNumber()) {
		//2.数量正确，校验总字节数
		for (auto pair : fileContextMap) {
			totalLength += pair.second.size();
		}
		if (totalLength == fileInfo->getTotalLength()) {
			//总字节数校验通过
			//3.文件拼接
			string filePath = DataManager::basePath + fileInfo->getAccount() + "/pictures/" +
				fileInfo->getFilename();
			int wfd = open(filePath.c_str(), O_WRONLY | O_CREAT, 0777);
			if (wfd == -1) {
				perror("文件拼接open err");
			}
			else {
				for (auto pair : fileContextMap) {
					int res = write(wfd, pair.second.c_str(), pair.second.size());
					if (res != pair.second.size()) {
						cout << pair.first << "写入长度不对，预计写入:" << pair.second.size() << " 实际写入:" << res << endl;
					}
				}
			}
			return 1;
		}
		else {
			//总字节数校验不通过
			cout << "收到总字节数:" << totalLength << " 文件总字节数:" << fileInfo->getTotalLength() << endl;
			return 0;
		}
	}
	else {
		//2.数量少了，需要找到丢失的文件碎片序号
		for (int i = 1; i <= fileInfo->getTotalNumber(); ++i) {
			if (fileContextMap.count(i) == 0) {
				arr[index++] = i;//标记丢失的文件碎片序号
			}
		}
		//3.构造文件丢失返回包给前置
		HEAD headBack;
		FileBack bodyBack;
		headBack.bussinessLength = sizeof(bodyBack);
		headBack.bussinessType = 24;
		headBack.crc = clientFd;
		strcpy(bodyBack.filename, fileInfo->getFilename().c_str());
		memcpy(bodyBack.arr, arr, sizeof(arr));
		//准备数据缓冲区
		char buffer[sizeof(HEAD) + sizeof(bodyBack)];
		memcpy(buffer, &headBack, sizeof(HEAD));
		memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
		//数据存放共享内存
		IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
		return 0;
	}
	
}
