#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <string>
#include <iostream>
#include <pthread.h>
using namespace std;
using namespace sql;
using namespace sql::mysql;
const string localhost = "127.0.0.1";//连接远程数据库ip地址，因为代码在服务器运行所以是127
const string localuser = "root";//数据库账户
const string localpassword = "Password_123";//数据库密码
const string localschema = "Intelligent Parking System";//数据库逻辑容器
const string localport = "3306";//数据库端口号
class DBConnection
{
public:
	~DBConnection();
	static DBConnection* getInstance();
	sql::Connection* getConnection();
	void disConnection();//断开数据库连接
	static pthread_mutex_t mutex;
private:
	DBConnection();
	static DBConnection* instance;
	MySQL_Driver* driver;//继承自Driver
	string path;
	unique_ptr<Connection> conn;
};

