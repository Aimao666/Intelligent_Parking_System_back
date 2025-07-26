#include "CBaseOperation.h"

CBaseOperation::CBaseOperation()
{
	this->connection = DBConnection::getInstance();
	this->conn = connection->getConnection();
}




int CBaseOperation::doDelete(const std::string& whereSql)
{
    string sql = "delete from " + tablename + whereSql;
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException e) {
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

int CBaseOperation::executeUpdate(string sql)
{
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    conn->setAutoCommit(false);//开启事务
    int rs = 0;
    try {
        rs = pstmt->executeUpdate();
        conn->commit();//事务提交
    }
    catch (SQLException e) {
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

string CBaseOperation::getTablename() const
{
	return tablename;
}
