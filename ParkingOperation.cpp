#include "ParkingOperation.h"

ParkingOperation::ParkingOperation()
{
    tablename = "parking_info";
}
//入场
int ParkingOperation::doInsert(void* object)
{
    Parking* ptr = static_cast<Parking*>(object);
    string sql = "insert into " + tablename + " (`account`,`carNumber`,`entryTime`,`entryPosition`,`entryPicId`) values (?,?,?,?,?)";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getAccount());
    pstmt->setString(2, ptr->getCarNumber());
    pstmt->setString(3, ptr->getEntryTime());
    pstmt->setString(4, ptr->getEntryPosition());
    pstmt->setInt(5, ptr->getEntryPicId());
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException& e) {
        std::cerr << "SQL Exception in insert: " << e.what() << std::endl;
        if (this->conn != nullptr) {
            conn->rollback();//事务回滚
            conn->setAutoCommit(true);//关闭事务
            pthread_mutex_unlock(&DBConnection::mutex);
        }
        delete pstmt;
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    pthread_mutex_unlock(&DBConnection::mutex);
    return rs;
}
//出场
int ParkingOperation::doUpdate(void* object)
{
    Parking* ptr = static_cast<Parking*>(object);
    string sql = "update " + tablename + " set `leaveTime`=?,`leavePosition`=?,`leavePicId`=?,`dueCost`=? where `id`=?;";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getLeaveTime());
    pstmt->setString(2, ptr->getLeavePosition());
    pstmt->setInt(3, ptr->getLeavePicId());
    pstmt->setInt(4, ptr->getDueCost());
    pstmt->setInt(5, ptr->getId());
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException& e) {
        std::cerr << "SQL Exception in update: " << e.what() << std::endl;
        if (this->conn != nullptr) {
            conn->rollback();//事务回滚
            conn->setAutoCommit(true);//关闭事务
        }
        delete pstmt;
        pthread_mutex_unlock(&DBConnection::mutex);
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    pthread_mutex_unlock(&DBConnection::mutex);
    return rs;
}

void ParkingOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object)
{

    Parking* ptr = static_cast<Parking*>(object);
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
        else if (colName == "entryTime") {
            ptr->setEntryTime(rs->getString(i));
        }
        else if (colName == "entryPosition") {
            ptr->setEntryPosition(rs->getString(i));
        }
        else if (colName == "leavePicId") {
            ptr->setLeavePicId(rs->getInt(i));
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
