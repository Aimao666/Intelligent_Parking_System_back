#pragma once
#include "CBaseTask.h"
#include "FileInfo.h"
//文件上传业务，接收一个文件碎片
class CFileUploadTask :
	public CBaseTask
{
public:
	CFileUploadTask(int fd, char* data, size_t len);
	~CFileUploadTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};