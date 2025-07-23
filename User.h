#pragma once
#include <string>
using namespace std;
class User
{
public:
	User(string account="", string password="", string username="");
	~User();
private:
	string account;
	string username;
	string password;

public:
    string getAccount() const;
    void setAccount(string account);

    string getUsername() const;
    void setUsername(string username);

    string getPassword() const;
    void setPassword(string password);

};

