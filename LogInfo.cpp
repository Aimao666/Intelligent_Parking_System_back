#include "LogInfo.h"

LogInfo::LogInfo()
{
}

LogInfo::LogInfo(string account, string info, string createtime)
    :account(account), info(info), createtime(createtime)
{
}

int LogInfo::getId() const
{
    return id;
}

void LogInfo::setId(int id)
{
    this->id = id;
}

string LogInfo::getCreatetime() const
{
    return createtime;
}

void LogInfo::setCreatetime(string createtime)
{
    this->createtime = createtime;
}

string LogInfo::getInfo() const
{
    return info;
}

void LogInfo::setInfo(string info)
{
    this->info = info;
}

string LogInfo::getAccount() const
{
    return account;
}

void LogInfo::setAccount(string account)
{
    this->account = account;
}

