#include "CBaseTask.h"

CBaseTask::CBaseTask(int fd, char* data, size_t len)
    : clientFd(fd), dataLen(len) {
    if (len > 0) {
        taskData = new char[len];
        memcpy(taskData, data, len);
    }
    else {
        dataLen = 0;
        taskData = nullptr;
    }
}
CBaseTask::CBaseTask(int shmIndex)
{
    this->shmIndex = shmIndex;
    dataLen = 0;
    taskData = nullptr;
}
CBaseTask::~CBaseTask()
{
    if (dataLen > 0 && taskData) {
        delete[] taskData;
    }
}

int CBaseTask::getClientFd() const
{
    return clientFd;
}

char* CBaseTask::getTaskData() const
{
    return taskData;
}

size_t CBaseTask::getDataLen() const
{
    return dataLen;
}

