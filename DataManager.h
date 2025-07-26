#pragma once
#include "FileInfo.h"
#include <string>
using namespace std;
class DataManager
{
public:
	//数据基路径
	static const string basePath;
	//用来存储所有文件,key为用户账号+文件名的一个结构体,value为文件信息类指针
	static map<FileInfo::FileKey, unique_ptr<FileInfo>> allFileMap;
	static pthread_mutex_t allFileMapMutex;
private:
	DataManager() = delete;
	~DataManager() = delete;
};

