#pragma once
#include "CBaseOperation.h"
#include "User.h"

class UserOperation : public CBaseOperation {
public:
    UserOperation();
    ~UserOperation();

    // 实现其他操作接口
    int doInsert(void* object) override;
    int doUpdate(void* object) override;
    int doDelete(const string& whereSql = "") override;
private:
    // 实现结果集填充逻辑
    //传入的object是调用的默认构造函数的
    void fillObjectFromResultSet(sql::ResultSet* rs, void* object) override;
};