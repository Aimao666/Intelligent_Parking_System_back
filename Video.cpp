#include "Video.h"

Video::Video()
{
}

Video::Video(string account, string vname, string vpath, string totaltime, string createtime, string pname, string ppath, 
    string currentPlaytime, int id):account(account), vname(vname), vpath(vpath), totaltime(totaltime), currentPlaytime(currentPlaytime),
    createtime(createtime), pname(pname), ppath(ppath), id(id)
{
}

int Video::getId() const
{
    return id;
}

void Video::setId(int id)
{
    this->id = id;
}

string Video::getAccount() const
{
    return account;
}

void Video::setAccount(string account)
{
    this->account = account;
}

string Video::getVname() const
{
    return vname;
}

void Video::setVname(string vname)
{
    this->vname = vname;
}

string Video::getVpath() const
{
    return vpath;
}

void Video::setVpath(string vpath)
{
    this->vpath = vpath;
}

string Video::getTotaltime() const
{
    return totaltime;
}

void Video::setTotaltime(string totaltime)
{
    this->totaltime = totaltime;
}

string Video::getCurrentPlaytime() const
{
    return currentPlaytime;
}

void Video::setCurrentPlaytime(string currentPlaytime)
{
    this->currentPlaytime = currentPlaytime;
}

string Video::getCreatetime() const
{
    return createtime;
}

void Video::setCreatetime(string createtime)
{
    this->createtime = createtime;
}

string Video::getPname() const
{
    return pname;
}

void Video::setPname(string pname)
{
    this->pname = pname;
}

string Video::getPpath() const
{
    return ppath;
}

void Video::setPpath(string ppath)
{
    this->ppath = ppath;
}

string Video::getCreatedate() const
{
    return create_date;
}

void Video::setCreatedate(string createdate)
{
    create_date = createdate;
}

string Video::getCreatemonth() const
{
    return create_month;
}

void Video::setCreatemonth(string createmonth)
{
    create_month = createmonth;
}

