#pragma once
#include "CBaseOperation.h"
#include "ParkingPictureView.h"
class PPViewOperation :
    public CBaseOperation
{
public:
    PPViewOperation();
    ~PPViewOperation() = default;

private:
    // 实现结果集填充逻辑
    //传入的object是调用的默认构造函数的，用于query查询的
    void fillObjectFromResultSet(sql::ResultSet* rs, void* object) override;
};
