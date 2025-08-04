#pragma once
#include <string>
using namespace std;
class Video
{
public:
	Video();
	Video(string account, string vname, string vpath, string totaltime, string createtime, string pname, string ppath,
        string currentPlaytime="00:00:00", int id=-1);
	~Video()=default;
private:
	int id;
	string account;//所属用户
	string vname;//视频名称
	string vpath;//视频在客户端存储路径
	string totaltime;//总播放时长
	string currentPlaytime;//当前播放时长
	string createtime;//创建时间
    string pname;//视频名称
    string ppath;//视频在客户端存储路径
    string create_date;//创建日期
    string create_month;//创建月份

public:
    int getId() const;
    void setId(int id);

    string getAccount() const;
    void setAccount(string account);

    string getVname() const;
    void setVname(string vname);

    string getVpath() const;
    void setVpath(string vpath);

    string getTotaltime() const;
    void setTotaltime(string totaltime);

    string getCurrentPlaytime() const;
    void setCurrentPlaytime(string currentPlaytime);

    string getCreatetime() const;
    void setCreatetime(string createtime);

    string getPname() const;
    void setPname(string pname);

    string getPpath() const;
    void setPpath(string ppath);

    string getCreatedate() const;
    void setCreatedate(string createdate);

    string getCreatemonth() const;
    void setCreatemonth(string createmonth);

};

