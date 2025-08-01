#include "User.h"

User::User(string account, string username, int totalNum)
    :account(account),password(password), totalNum(totalNum)
{
}


string User::getAccount() const
{
    return account;
}

void User::setAccount(string account)
{
    this->account = account;
}

string User::getPassword() const
{
    return password;
}

void User::setPassword(string password)
{
    this->password = password;
}


int User::getTotalNum() const
{
    return totalNum;
}

void User::setTotalNum(int totalNum)
{
    this->totalNum = totalNum;
}

