#include "FileInfo.h"

FileInfo::FileInfo(string account, string filename, int totalNumber, int totalLength, int clientFd, int type, 
    string createtime, string khdPath):account(account),filename(filename),totalNumber(totalNumber),
    totalLength(totalLength),clientFd(clientFd),type(type),createtime(createtime),khdPath(khdPath)
{
	pthread_mutex_init(&fileContextMapMutex, NULL);
}

FileInfo::~FileInfo()
{
    pthread_mutex_destroy(&this->fileContextMapMutex);
}


map<int, string>& FileInfo::getFileContextMap()
{
	// TODO: 在此处插入 return 语句
	return this->fileContextMap;
}

string FileInfo::getAccount() const
{
    return account;
}

string FileInfo::getFilename() const
{
    return filename;
}

int FileInfo::getTotalNumber() const
{
    return totalNumber;
}

int FileInfo::getTotalLength() const
{
    return totalLength;
}

int FileInfo::getClientFd() const
{
	return this->clientFd;
}

int FileInfo::getType() const
{
    return type;
}

string FileInfo::getCreatetime() const
{
    return createtime;
}

string FileInfo::getKhdPath() const
{
    return khdPath;
}

