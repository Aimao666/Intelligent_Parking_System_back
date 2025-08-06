#include "DataManager.h"

string DataManager::basePath= "../AppData/data/";
map<FileInfo::FileKey, unique_ptr<FileInfo>> DataManager::allFileMap;
pthread_mutex_t DataManager::allFileMapMutex;
pthread_mutex_t DataManager::writeLogMutex;
int DataManager::writeLog(string account, string info, string createtime, string filepath)
{
	string bathLogPath = basePath + "log/";
	filepath = (filepath == "" ? bathLogPath : filepath);
	pthread_mutex_lock(&writeLogMutex);
	if (!CTools::createDirectoryRecursive(filepath)) {
		cout << "日志目录创建失败" << endl;
		pthread_mutex_unlock(&writeLogMutex);
		return 0;
	}
	string filename = filepath + CTools::getDatetime("%Y-%m-%d.txt");
	int wfd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (wfd == -1) {
		perror((filename+" open err ").c_str());
		pthread_mutex_unlock(&writeLogMutex);
		return 0;
	}
	else {
		int res = write(wfd, info.c_str(), info.size());
		pthread_mutex_unlock(&writeLogMutex);
		if (res == info.size()) {
			cout << "日志写入完毕" << endl;
			//插入数据库
			LogInfo log(account,info, createtime);
			int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::LOG)->doInsert(&log);
			if (rows > 0) {
				close(wfd);
				return 1;
			}
			else
			{
				cerr << "日志插入数据库失败" << endl;
				close(wfd);
				return 0;
			}
		}
		else {
			perror("write err");
			cerr << "写入日志失败或者写入信息不全res=" << res << ",期望写入" << info.size() << endl;
			close(wfd);
			return 0;
		}
	}
}
