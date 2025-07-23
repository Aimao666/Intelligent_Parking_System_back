#include "User.h"

User::User(string account, string password, string username)
    :account(account),password(password),username(username)
{
}

User::~User()
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

string User::getUsername() const
{
    return username;
}

void User::setUsername(string username)
{
    this->username = username;
}

string User::getPassword() const
{
    return password;
}

void User::setPassword(string password)
{
    this->password = password;
}

