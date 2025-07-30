#pragma once
#include "CBaseOperation.h"
#include "Parking.h"
class ParkingOperation :
    public CBaseOperation
{
public:
    ParkingOperation();
    ~ParkingOperation() = default;

    // 实现其他操作接口
    int doInsert(void* object) override;
    int doUpdate(void* object) override;
private:
    // 实现结果集填充逻辑
    //传入的object是调用的默认构造函数的，用于query查询的
    void fillObjectFromResultSet(sql::ResultSet* rs, void* object) override;
};

