#include "CFileCheckTask.h"

CFileCheckTask::CFileCheckTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}


//一个文件发送完毕后客户端发来的确认包
void CFileCheckTask::work()
{
	cout << "CFileCheckTask正在执行" << endl;
	//解析包
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), sizeof request);
	if (request.flag <= 0) {
		cout << "FileCheckRequest:flag=" << request.flag << endl;
		return;
	}

	// 请求日志记录
	{
		std::ostringstream logStream;
		std::string currentTime = CTools::getDatetime();

		logStream << "时间：" << currentTime << "\n"
			<< "功能：文件上传确认包\n"
			<< "类型：请求\n"
			<< "用户账号：" << request.account << "\n"
			<< "文件名称：" << request.filename << "\n"
			<< "确认标志：" << request.flag << "\n";
		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
	//业务处理
	//1.文件碎片保存
	FileInfo::FileKey fileKey{ request.account,request.filename };
	FileInfo* fileInfo = DataManager::allFileMap[fileKey].get();
	map<int, string> fileContextMap = fileInfo->getFileContextMap();
	//2.检查文件数据是否正确
	int res = checkFile(fileInfo);
	if (res == 1) {
		//准备返回包
		headBack.bussinessLength = sizeof(bodyBack2);
		//3.数据库记录
		string filePath = DataManager::basePath + fileInfo->getAccount() + "/pictures/" +
			fileInfo->getFilename();
		cout << "fwqPath.size()=" << filePath.size() << endl;
		auto op = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PICTURE);
		Picture pic(fileInfo->getFilename(), fileInfo->getKhdPath(), filePath, fileInfo->getCreatetime(), 
			(Picture::PICTYPE)fileInfo->getType(),fileInfo->getAccount());
		res = op->doInsert(&pic);
		strcpy(bodyBack2.filename, fileInfo->getFilename().c_str());
		if (res) {
			bodyBack2.flag = 1;
			cout << "图片插入数据库成功" << endl;
		}
		else {
			bodyBack2.flag = 0;
			cout << "图片插入数据库失败" << endl;
		}
		//4.发送返回包
		char buffer[sizeof(HEAD) + sizeof(bodyBack2)];
		memcpy(buffer, &headBack, sizeof(HEAD));
		memcpy(buffer + sizeof(HEAD), &bodyBack2, sizeof(bodyBack2));
		//数据存放共享内存
		IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
		//5.数据库操作执行完毕了，将该文件在内存的内容销毁
		fileContextMap.clear();
		pthread_mutex_lock(&DataManager::allFileMapMutex);
		DataManager::allFileMap.erase(fileKey);
		pthread_mutex_unlock(&DataManager::allFileMapMutex);

		// 日志记录 - 响应部分-文件保存成功
		{
			std::ostringstream logStream;
			std::string currentTime = CTools::getDatetime();

			logStream << "时间：" << currentTime << "\n"
				<< "功能：文件上传成功返回包\n"
				<< "类型：响应\n"
				<< "用户账号：" << request.account << "\n"
				<< "文件名：" << bodyBack2.filename << "\n"
				<< "标志0插入数据库失败1插入数据库成功：" << bodyBack2.flag << "\n"
				<< "无论标志0还是1，发出此包则代表该文件已经保存成功了，只是数据库是否有记录的问题\n";


			DataManager::writeLog(request.account, logStream.str(), currentTime);
		}
	}

}
//检查文件数据是否正确
int CFileCheckTask::checkFile(FileInfo* fileInfo) {
	map<int, string> fileContextMap = fileInfo->getFileContextMap();
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
			string dirPath = DataManager::basePath + fileInfo->getAccount() + "/pictures/";
			string filePath = dirPath + fileInfo->getFilename();
			if (CTools::createDirectoryRecursive(dirPath)) {
				cout << "创建目录成功:" << dirPath << endl;
			}
			else{
				perror("mkdir err");
			}
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
				close(wfd);
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
		int arr[100];//丢包序列数组
		int index = 0;
		//初始化
		for (int i = 0; i < 100; ++i) {
			arr[i] = -1;
		}
		//2.数量少了，需要找到丢失的文件碎片序号
		for (int i = 1; i <= fileInfo->getTotalNumber(); ++i) {
			if (fileContextMap.count(i) == 0) {
				arr[index++] = i;//标记丢失的文件碎片序号
				if (index == 100) {
					//3.1丢包太多，超100，直接构造文件丢失返回包给前置，并准备下一个丢包返回包
					HEAD headBack;
					headBack.bussinessLength = sizeof(bodyBack1);
					headBack.bussinessType = 24;
					headBack.crc = clientFd;
					strcpy(bodyBack1.filename, fileInfo->getFilename().c_str());
					memcpy(bodyBack1.arr, arr, sizeof(arr));
					//准备数据缓冲区
					char buffer[sizeof(HEAD) + sizeof(bodyBack1)];
					memcpy(buffer, &headBack, sizeof(HEAD));
					memcpy(buffer + sizeof(HEAD), &bodyBack1, sizeof(bodyBack1));
					//数据存放共享内存
					IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);

					// 日志记录 - 响应部分
					{
						std::ostringstream logStream;
						std::string currentTime = CTools::getDatetime();

						logStream << "时间：" << currentTime << "\n"
							<< "功能：文件上传丢包返回包\n"
							<< "类型：响应\n"
							<< "用户账号：" << request.account << "\n"
							<< "文件名：" << bodyBack1.filename << "\n"
							<< "有效数据个数：" << index << "\n"
							<< "丢包序号：\n";

						for (int i = 0; i < index; ++i) {

							logStream << arr[i];
							if (i < index - 1) {
								logStream << ",";
							}
							else {
								logStream << "\n";
							}
						}

						DataManager::writeLog(request.account, logStream.str(), currentTime);
					}

					index = 0;
					for (int j = 0; j < 100; ++j) {
						arr[j] = -1;
					}
				}
			}
		}
		//3.2构造文件丢失返回包给前置
		//FileBack bodyBack;
		HEAD headBack;
		headBack.bussinessLength = sizeof(bodyBack1);
		headBack.bussinessType = 24;
		headBack.crc = clientFd;
		strcpy(bodyBack1.filename, fileInfo->getFilename().c_str());
		memcpy(bodyBack1.arr, arr, sizeof(arr));
		//准备数据缓冲区
		char buffer[sizeof(HEAD) + sizeof(bodyBack1)];
		memcpy(buffer, &headBack, sizeof(HEAD));
		memcpy(buffer + sizeof(HEAD), &bodyBack1, sizeof(bodyBack1));
		//数据存放共享内存
		IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);

		// 日志记录 - 响应部分
		{
			std::ostringstream logStream;
			std::string currentTime = CTools::getDatetime();

			logStream << "时间：" << currentTime << "\n"
				<< "功能：文件上传丢包返回包\n"
				<< "类型：响应\n"
				<< "用户账号：" << request.account << "\n"
				<< "文件名：" << bodyBack1.filename << "\n"
				<< "有效数据个数：" << index << "\n"
				<< "丢包序号：\n";

			for (int i = 0; i < index; ++i) {

				logStream << arr[i];
				if (i < index - 1) {
					logStream << ",";
				}
				else {
					logStream << "\n";
				}
			}

			DataManager::writeLog(request.account, logStream.str(), currentTime);
		}

		return 0;
	}
	
}
