#pragma once
#include <string>
using namespace std;
class VpView
{
public:
	VpView();
	VpView(string account, string totaltime, string currentPlaytime, string createtime, string vname, string vkhdPath, 
        string pname, string pkhdPath);
	~VpView() = default;
private:
	int id;
	string account;//所属用户
	string totaltime;//总播放时长
	string currentPlaytime;//当前播放时长
	string createtime;//创建时间
	string vname;//视频名称
	string vkhdPath;//视频在客户端存储路径
	string pname;//首帧图片名称
	string pkhdPath;//首帧图片在客户端存储路径
public:
    int getId() const;
    void setId(int id);

    string getAccount() const;
    void setAccount(string account);

    string getTotaltime() const;
    void setTotaltime(string totaltime);

    string getCurrentPlaytime() const;
    void setCurrentPlaytime(string currentPlaytime);

    string getCreatetime() const;
    void setCreatetime(string createtime);

    string getVname() const;
    void setVname(string vname);

    string getVkhdPath() const;
    void setVkhdPath(string vkhdPath);

    string getPname() const;
    void setPname(string pname);

    string getPkhdPath() const;
    void setPkhdPath(string pkhdPath);

};

