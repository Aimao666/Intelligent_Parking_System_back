#pragma once
#include "CBaseTask.h"
#include "OperationFactory.h"      
#include <sys/stat.h>
#include <sys/types.h>
class RegisterTask :
    public CBaseTask
{
public:
    RegisterTask(int fd, char* data, size_t len);
    ~RegisterTask() = default;
private:
    // 通过 CBaseTask 继承
    virtual void work() override;
};
