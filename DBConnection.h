#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include "RrConfig.h"
using namespace std;
using namespace sql;
using namespace sql::mysql;

class DBConnection
{
public:
	~DBConnection();
	static DBConnection* getInstance();
	sql::Connection* getConnection();
	void disConnection();//断开数据库连接
	static pthread_mutex_t mutex;
	static int loadConfig(std::string configPath);
private:
	DBConnection();
	static pthread_mutex_t createMutex;
	static DBConnection* instance;
	MySQL_Driver* driver;//继承自Driver
	string path;
	unique_ptr<Connection> conn;

	static string localhost;//连接远程数据库ip地址，因为代码在服务器运行所以是127
	static string localuser;//数据库账户
	static string localpassword;//数据库密码
	static string localschema;//数据库逻辑容器
	static string localport;//数据库端口号
	static string hostname;
};

