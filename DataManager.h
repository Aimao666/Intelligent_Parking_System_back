#pragma once
#include "FileInfo.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "CTools.h"
#include <unistd.h>
#include "OperationFactory.h"
using namespace std;
class DataManager
{
public:
	//数据基路径
	static string basePath;
	//用来存储所有文件,key为用户账号+文件名的一个结构体,value为文件信息类指针
	static map<FileInfo::FileKey, unique_ptr<FileInfo>> allFileMap;
	static pthread_mutex_t allFileMapMutex;

	static pthread_mutex_t writeLogMutex;
	static int writeLog(string account, string info, string createtime = CTools::getDatetime(), string filepath = "");
private:
	DataManager() = delete;
	~DataManager() = delete;
};

