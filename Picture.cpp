#include "Picture.h"

Picture::Picture(string name, string khdPath, string fwqPath, string createtime, PICTYPE type, string account)
	:name(name),khdPath(khdPath),fwqPath(fwqPath),createtime(createtime),type(type),account(account)
{
}


int Picture::getId() const
{
    return id;
}

void Picture::setId(int id)
{
    this->id = id;
}

string Picture::getName() const
{
    return name;
}

void Picture::setName(string name)
{
    this->name = name;
}

string Picture::getKhdPath() const
{
    return khdPath;
}

void Picture::setKhdPath(string khdPath)
{
    this->khdPath = khdPath;
}

string Picture::getFwqPath() const
{
    return fwqPath;
}

void Picture::setFwqPath(string fwqPath)
{
    this->fwqPath = fwqPath;
}

string Picture::getCreatetime() const
{
    return createtime;
}

void Picture::setCreatetime(string createtime)
{
    this->createtime = createtime;
}

Picture::PICTYPE Picture::getType() const
{
    return type;
}

void Picture::setType(Picture::PICTYPE type)
{
    this->type = type;
}

string Picture::getAccount() const
{
    return account;
}

void Picture::setAccount(string account)
{
    this->account = account;
}
