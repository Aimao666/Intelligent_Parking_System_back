#include "VideoOperation.h"

VideoOperation::VideoOperation()
{
	tablename = "video_info";
}

int VideoOperation::doInsert(void* object)
{
    Video* ptr = static_cast<Video*>(object);
    string sql = "insert into " + tablename + " (`account`,`vname`,`vpath`,`pname`,`ppath`,`totaltime`,`currentPlaytime`,`createtime`)" +
        "values (?,?,?,?,?,?,?,?)";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getAccount());
    pstmt->setString(2, ptr->getVname());
    pstmt->setString(3, ptr->getVpath());
    pstmt->setString(4, ptr->getPname());
    pstmt->setString(5, ptr->getPpath());
    pstmt->setString(6, ptr->getTotaltime());
    pstmt->setString(7, ptr->getCurrentPlaytime());
    pstmt->setString(8, ptr->getCreatetime());
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

int VideoOperation::doUpdate(void* object)
{
    Video* ptr = static_cast<Video*>(object);
    string sql = "update " + tablename + " set `currentPlaytime`=? where `id`=?;";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getCurrentPlaytime());
    pstmt->setInt(2, ptr->getId());
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
        pthread_mutex_unlock(&DBConnection::mutex);
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    pthread_mutex_unlock(&DBConnection::mutex);
    return rs;
}

void VideoOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object)
{
    Video* ptr = static_cast<Video*>(object);
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
        else if (colName == "vname") {
            ptr->setVname(rs->getString(i));
        }
        else if (colName == "vpath") {
            ptr->setVpath(rs->getString(i));
        }
        else if (colName == "pname") {
            ptr->setPname(rs->getString(i));
        }
        else if (colName == "ppath") {
            ptr->setPname(rs->getString(i));
        }
        else if (colName == "totaltime") {
            ptr->setTotaltime(rs->getString(i));
        }
        else if (colName == "currentPlaytime") {
            ptr->setCurrentPlaytime(rs->getString(i));
        }
        else if (colName == "createtime") {
            ptr->setCreatetime(rs->getString(i));
        }
        else {
            std::cerr << "VideoOperation Unknown column: " << colName << std::endl;
        }
    }
}
