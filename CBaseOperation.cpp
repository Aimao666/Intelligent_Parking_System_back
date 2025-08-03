#include "CBaseOperation.h"

CBaseOperation::CBaseOperation()
{
	this->connection = DBConnection::getInstance();
	this->conn = connection->getConnection();
}




int CBaseOperation::doDelete(const std::string& whereSql)
{
    string sql = "delete from " + tablename + whereSql;
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException& e) {
        std::cerr << "SQL Exception in delete: " << e.what() << std::endl;
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

int CBaseOperation::doInsert(void* pocoPtr)
{
    return 0;
}

int CBaseOperation::doUpdate(void* pocoPtr)
{
    return 0;
}

int CBaseOperation::executeUpdate(string sql)
{
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException& e) {
        std::cerr << "SQL Exception in executeUpdate: " << e.what() << std::endl;
        if (this->conn != nullptr) {
            conn->rollback();//事务回滚
            conn->setAutoCommit(true);//关闭事务
        }
        pthread_mutex_unlock(&DBConnection::mutex);
        delete pstmt;
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    pthread_mutex_unlock(&DBConnection::mutex);
    return rs;
}

string CBaseOperation::getTablename() const
{
	return tablename;
}
