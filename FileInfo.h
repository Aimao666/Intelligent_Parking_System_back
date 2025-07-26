#pragma once
#include <string>
#include <map>
#include <memory>
using namespace std;

//文件信息类
class FileInfo
{
public:
	typedef struct FileKey {
		std::string account;
		std::string filename;
		// 实现比较运算符，用于map排序
		bool operator<(const FileKey& other) const {
			//优先根据账号排序
			if (account != other.account)
				return account < other.account;
			return filename < other.filename;
		}
	}FileKey;
	FileInfo(string account, string filename, int totalNumber, int totalLength, int clientFd, int type, string createtime, string khdPath);
	~FileInfo();
	map<int, string>& getFileContextMap();
	pthread_mutex_t fileContextMapMutex;
private:
	string account;//账号
	string filename;//文件名
	int totalNumber;//原文件碎片总个数
	int totalLength;//原文件总字节数
	int clientFd;//客户端文件描述符

	int type;//图片类型 1入场 2出场 3 监控截图 4视频首帧
	string createtime;//图片创建时间
	string khdPath;//客户端图片路径

	//key为文件碎片序号，从1开始计数;value是文件二进制内容，最多1000个字节
	map<int, string> fileContextMap;//存放该文件所有文件碎片

public:

	//Getter
	string getAccount() const;

	string getFilename() const;

	int getTotalNumber() const;

	int getTotalLength() const;

	int getClientFd()const;

    int getType() const;

    string getCreatetime() const;

    string getKhdPath() const;

};

