#include "LogOperation.h"

LogOperation::LogOperation()
{
    tablename = "log_info";
}

int LogOperation::doInsert(void* object)
{
    LogInfo* ptr = static_cast<LogInfo*>(object);
    string sql;
    if (ptr->getAccount() == "") {
        sql = "insert into " + tablename + " (`info`,`createtime`) values (?,?)";
    }
    else {
        sql = "insert into " + tablename + " (`info`,`createtime`,`account`) values (?,?,?)";
    }
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getInfo());
    pstmt->setString(2, ptr->getCreatetime());
    if (ptr->getAccount() != "")pstmt->setString(3, ptr->getAccount());
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


void LogOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object)
{
    LogInfo* ptr = static_cast<LogInfo*>(object);
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
        else if (colName == "info") {
            ptr->setInfo(rs->getString(i));
        }
        else if (colName == "createtime") {
            ptr->setCreatetime(rs->getString(i));
        }
        else {
            std::cerr << "LogOperation Unknown column: " << colName << std::endl;
        }
    }
}
