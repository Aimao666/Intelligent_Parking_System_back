#pragma once
#include "CBaseTask.h"
#include "FileInfo.h"      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//一个文件发送完毕后客户端发来的确认包
class CFileCheckTask :
    public CBaseTask
{
public:
	CFileCheckTask(int fd, char* data, size_t len);
	~CFileCheckTask() = default;
private:
	int checkFile(FileInfo* fileInfo);
	// 通过 CBaseTask 继承
	virtual void work() override;
	FileBack bodyBack1;
	FileSuccessBack bodyBack2;
	FileCheckRequest request;
};

