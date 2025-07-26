#pragma once
#include "CBaseOperation.h"
#include "Picture.h"
class PictureOperation :
    public CBaseOperation
{
public:
    PictureOperation();
    ~PictureOperation()=default;

    // 实现其他操作接口
    int doInsert(void* object) override;
    int doUpdate(void* object) override;
private:
    // 实现结果集填充逻辑
    //传入的object是调用的默认构造函数的，用于query查询的
    void fillObjectFromResultSet(sql::ResultSet* rs, void* object) override;
};

