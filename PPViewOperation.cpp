#include "PPViewOperation.h"

PPViewOperation::PPViewOperation()
{
	tablename = "parking_picture_view";
}

void PPViewOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object)
{
    ParkingPictureView* ptr = static_cast<ParkingPictureView*>(object);
    sql::ResultSetMetaData* meta = rs->getMetaData();//rs管理生命周期，无需释放
    const int colCount = meta->getColumnCount();

    for (int i = 1; i <= colCount; ++i) {
        //sql语句查询不一定是全字段查询，可能是部分字段查询，如果直接rs->get获取未查询字段的话会抛异常
        const std::string colName = meta->getColumnName(i);

        if (colName == "account") {
            ptr->setAccount(rs->getString(i));
        }
        else if (colName == "id") {
            ptr->setId(rs->getInt(i));
        }
        else if (colName == "dueCost") {
            ptr->setDueCost(rs->getInt(i));
        }
        else if (colName == "reallyCost") {
            ptr->setReallyCost(rs->getInt(i));
        }
        else if (colName == "entryPicId") {
            ptr->setEntryPicId(rs->getInt(i));
        }
        else if (colName == "entryPicName") {
            ptr->setEntryPicName(rs->getString(i));
        }
        else if (colName == "entryPicKhdPath") {
            ptr->setEntryPicKhdPath(rs->getString(i));
        }
        else if (colName == "entryTime") {
            ptr->setEntryTime(rs->getString(i));
        }
        else if (colName == "entryPosition") {
            ptr->setEntryPosition(rs->getString(i));
        }
        else if (colName == "leavePicId") {
            ptr->setLeavePicId(rs->getInt(i));
        }
        else if (colName == "leavePicName") {
            ptr->setLeavePicName(rs->getString(i));
        }
        else if (colName == "leavePicKhdPath") {
            ptr->setLeavePicKhdPath(rs->getString(i));
        }
        else if (colName == "leaveTime") {
            ptr->setLeaveTime(rs->getString(i));
        }
        else if (colName == "leavePosition") {
            ptr->setLeavePosition(rs->getString(i));
        }
        else if (colName == "carNumber") {
            ptr->setCarNumber(rs->getString(i));

        }
        else {
            std::cerr << "ParkingOperation Unknown column: " << colName << std::endl;
        }
    }
}
