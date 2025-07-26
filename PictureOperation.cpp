#include "PictureOperation.h"

PictureOperation::PictureOperation()
{
    tablename = "picture_info";
}

int PictureOperation::doInsert(void* object)
{
    Picture* ptr = (Picture*)object;
    string sql = "insert into " + tablename + " (`account`,`name`,`khdPath`,`fwqPath`,`createtime`,`type`) values (?,?,?,?,?,?)";
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getAccount());
    pstmt->setString(2, ptr->getName());
    pstmt->setString(3, ptr->getKhdPath());
    pstmt->setString(4, ptr->getFwqPath());
    pstmt->setString(5, ptr->getCreatetime());
    pstmt->setInt(6, ptr->getType());
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException e) {
        std::cerr << "SQL Exception in insert: " << e.what() << std::endl;
        if (this->conn != nullptr) {
            conn->rollback();//事务回滚
            conn->setAutoCommit(true);//关闭事务
        }
        delete pstmt;
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    return rs;
}

int PictureOperation::doUpdate(void* object)
{
    Picture* ptr = (Picture*)object;
    string sql = "update " + tablename + " set `account`=?,`name`=?,`khdPath`=?,`fwqPath`=?,`createtime`=?,`type`=? where `id`=?;";
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getAccount());
    pstmt->setString(2, ptr->getName());
    pstmt->setString(3, ptr->getKhdPath());
    pstmt->setString(4, ptr->getFwqPath());
    pstmt->setString(5, ptr->getCreatetime());
    pstmt->setInt(6, ptr->getType());
    pstmt->setInt(7, ptr->getId());
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException e) {
        std::cerr << "SQL Exception in update: " << e.what() << std::endl;
        if (this->conn != nullptr) {
            conn->rollback();//事务回滚
            conn->setAutoCommit(true);//关闭事务
        }
        delete pstmt;
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    return rs;
}

void PictureOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object)
{
    Picture* ptr = static_cast<Picture*>(object);
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
        else if (colName == "type") {
            ptr->setType(rs->getInt(i));
        }
        else if (colName == "name") {
            ptr->setName(rs->getString(i));
        }
        else if (colName == "khdPath") {
            ptr->setKhdPath(rs->getString(i));
        }
        else if (colName == "fwqPath") {
            ptr->setFwqPath(rs->getString(i));
        }
        else if (colName == "createtime") {
            ptr->setCreatetime(rs->getString(i));
        }
        else {
            std::cerr << "Unknown column: " << colName << std::endl;
        }
    }
}
