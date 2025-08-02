#pragma once
#include <string>
using namespace std;
class LogInfo
{
public:
	LogInfo();
	LogInfo(string account, string info, string createtime);
	~LogInfo() = default;
private:
	int id;
	string createtime;//创建时间
	string info;//日志信息
	string account;//操作用户
public:
    int getId() const;
    void setId(int id);

    string getCreatetime() const;
    void setCreatetime(string createtime);

    string getInfo() const;
    void setInfo(string info);

    string getAccount() const;
    void setAccount(string account);

};

