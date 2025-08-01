#pragma once
#include <string>
using namespace std;

class Picture
{
public:
	enum PICTYPE
	{
		ENTRY = 1,
		LEAVE,
		SNAP,
		FIRSTFRAME
	};
	Picture() = default;
	Picture(string name, string khdPath, string fwqPath, string createtime, PICTYPE type, string account);
	~Picture() = default;
private:
	int id;//图片序号
	string name;//图片名称
	string khdPath;//图片客户端路径
	string fwqPath;//图片服务器路径
	string createtime;//创建时间
	PICTYPE type;//图片类型 1入场 2出场 3 监控截图
	string account;//用户账号
public:
	//getter and setter
    int getId() const;
    void setId(int id);

    string getName() const;
    void setName(string name);

    string getKhdPath() const;
    void setKhdPath(string khdPath);

    string getFwqPath() const;
    void setFwqPath(string fwqPath);

    string getCreatetime() const;
    void setCreatetime(string createtime);

	PICTYPE getType() const;
    void setType(PICTYPE type);

    string getAccount() const;
    void setAccount(string account);

};

