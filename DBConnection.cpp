#include "DBConnection.h"
DBConnection* DBConnection::instance=nullptr;
pthread_mutex_t DBConnection::mutex;
pthread_mutex_t DBConnection::createMutex;
string DBConnection::localhost;//连接远程数据库ip地址，因为代码在服务器运行所以是127
string DBConnection::localuser;//数据库账户
string DBConnection::localpassword;//数据库密码
string DBConnection::localschema;//数据库逻辑容器
string DBConnection::localport;//数据库端口号
string DBConnection::hostname;
DBConnection* DBConnection::getInstance()
{
    //线程锁，保证饿汉式线程安全
    pthread_mutex_lock(&createMutex);
    if (instance == nullptr) {
        instance = new DBConnection();
    }
    pthread_mutex_unlock(&createMutex);
    return instance;

}

DBConnection::DBConnection()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&createMutex, NULL); 
    //获取驱动单例
    this->driver = sql::mysql::get_driver_instance();
    this->path = "tcp://" + localhost + ":" + localport+"?useUnicode=true&characterEncoding=UTF-8";
    cout << "path=" << path << endl;
    //创建一个连接
	//‌带参数调用‌：释放旧资源并管理新指针。
    this->conn.reset(driver->connect(path, localuser, localpassword));
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

int DBConnection::loadConfig(std::string configPath)
{
    char path[256];
    cout << "pwd=" << getcwd(path, 256) << endl;
    rr::RrConfig config;
    bool ret = config.ReadConfig(configPath);
    if (ret == false) {
        printf("ReadConfig is Error,Cfg=%s", configPath.c_str());
        return 0;
    }
    localhost = config.ReadString("MYSQL", "localhost", "");
    localuser = config.ReadString("MYSQL", "localuser", "");
    localpassword = config.ReadString("MYSQL", "localpassword", "");
    localschema = config.ReadString("MYSQL", "localschema", "");
    localport = config.ReadString("MYSQL", "localport", "");
    hostname = config.ReadString("MYSQL", "hostname", "");

    std::cout << "localhost=" << localhost << std::endl;
    std::cout << "localuser=" << localuser << std::endl;
    std::cout << "localpassword=" << localpassword << std::endl;
    std::cout << "localschema=" << localschema << std::endl;
    std::cout << "localport=" << localport << std::endl;
    std::cout << "hostname=" << hostname << std::endl;
    return 1;
}

