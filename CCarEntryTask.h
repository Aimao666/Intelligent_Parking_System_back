#pragma once
#include "CBaseTask.h"
#include "OperationFactory.h"
class CCarEntryTask :
    public CBaseTask
{
public:
	CCarEntryTask(int fd, char* data, size_t len);
	~CCarEntryTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
	HEAD headBack;
	CarEntryBack bodyBack;
};