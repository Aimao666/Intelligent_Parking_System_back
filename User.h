#pragma once
#include <string>
using namespace std;
class User
{
public:

    User(string account = "", string password = "", int totalNum = 2000);//默认2000个停车位
	~User()=default;
private:
	string account;
	int totalNum;
	string password;

public:
    string getAccount() const;
    void setAccount(string account);


    string getPassword() const;
    void setPassword(string password);

    int getTotalNum() const;
    void setTotalNum(int totalNum);

};

