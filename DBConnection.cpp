#include "DBConnection.h"
DBConnection* DBConnection::instance=nullptr;
pthread_mutex_t DBConnection::mutex;
DBConnection* DBConnection::getInstance()
{
    //线程锁，保证饿汉式线程安全
    pthread_mutex_lock(&mutex);
    if (instance == nullptr) {
        instance = new DBConnection();
    }
    pthread_mutex_unlock(&mutex);
    return instance;

}

DBConnection::DBConnection()
{
    pthread_mutex_init(&mutex, NULL);
    //获取驱动单例
    this->driver = sql::mysql::get_driver_instance();
    this->path = "tcp://" + localhost + ":" + localport;
    cout << "path=" << path << endl;
    //创建一个连接
	//‌带参数调用‌：释放旧资源并管理新指针。
    conn.reset(driver->connect(path, localuser, localpassword));
    //设置连接数据库
    this->conn->setSchema(localschema);

}
DBConnection::~DBConnection()
{
    disConnection();
}


sql::Connection* DBConnection::getConnection()
{
    return this->conn.get();
}

void DBConnection::disConnection()
{
    if (this->conn) {
        this->conn->close();
        this->conn.reset();//‌无参数调用‌：释放当前资源并将内部指针置为nullptr。
    }
}

