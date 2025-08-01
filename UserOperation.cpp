#include "UserOperation.h"

UserOperation::UserOperation()
    :CBaseOperation()
{
    tablename = "user_info";
}

////失败返回nullptr
//unique_ptr<vector<unique_ptr<User>>> UserOperation::query(string sql)
//{
//    if (sql == "")sql = "select * from "+tablename;
//    unique_ptr<vector<unique_ptr<User>>> vec(new vector<unique_ptr<User>>());
//    PreparedStatement* pstmt = conn->prepareStatement(sql);
//    ResultSet* rs;
//    try {
//        rs = pstmt->executeQuery();
//        while (rs->next()) {
//            unique_ptr<User> ele(new User());
//            ResultSetMetaData *meta=rs->getMetaData();//rs管理生命周期，无需释放
//            for (int i = 1; i <= meta->getColumnCount(); ++i) {
//                //sql语句查询不一定是全字段查询，可能是部分字段查询，如果直接rs->get获取未查询字段的话会抛异常
//                if (meta->getColumnName(i) == "account") {
//                    ele->setAccount(rs->getString(meta->getColumnName(i)));
//                }
//                else if(meta->getColumnName(i) == "username"){
//                    ele->setUsername(rs->getString(meta->getColumnName(i)));
//                }
//                else if (meta->getColumnName(i) == "password") {
//                    ele->setPassword(rs->getString(meta->getColumnName(i)));
//                }
//                else
//                {
//                    cout << sql << "预料之外的字段:" << meta->getColumnName(i) << endl;
//                }
//            }
//            vec->push_back(move(ele));
//        }
//        delete rs;
//        delete pstmt;
//        return vec;//直接返回就行vec会被视为将亡值触发移动构造
//    }
//    catch(SQLException e){
//        std::cerr << "SQL Exception: " << e.what() << std::endl;
//        //释放资源
//        delete rs;
//        delete pstmt;
//        return nullptr;  // 明确表示失败
//    }
//}

//传入的object是调用的默认构造函数的
void UserOperation::fillObjectFromResultSet(sql::ResultSet* rs, void* object) {
    User* ptr = static_cast<User*>(object);
    sql::ResultSetMetaData* meta = rs->getMetaData();//rs管理生命周期，无需释放
    const int colCount = meta->getColumnCount();

    for (int i = 1; i <= colCount; ++i) {
        //sql语句查询不一定是全字段查询，可能是部分字段查询，如果直接rs->get获取未查询字段的话会抛异常
        const std::string colName = meta->getColumnName(i);

        if (colName == "account") {
            ptr->setAccount(rs->getString(i));
        }
        else if (colName == "totalNum") {
            ptr->setTotalNum(rs->getInt(i));
        }
        else if (colName == "password") {
            ptr->setPassword(rs->getString(i));
        }
        else {
            std::cerr << "UserOperation Unknown column: " << colName << std::endl;
        }
    }
}

//失败返回0
int UserOperation::doInsert(void* object)
{
    User* ptr = (User*)object;
    string sql = "insert into " + tablename + " (`account`,`password`,`totalNum`) values (?,?,?)";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(1, ptr->getAccount());
    pstmt->setString(2, ptr->getPassword());
    pstmt->setInt(3, ptr->getTotalNum());
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
        pthread_mutex_unlock(&DBConnection::mutex);
        return 0;
    }
    delete pstmt;
    conn->setAutoCommit(true);//关闭事务
    pthread_mutex_unlock(&DBConnection::mutex);
    return rs;
}

int UserOperation::doUpdate(void* object)
{
    User* ptr = (User*)object;
    string sql = "update " + tablename + " set `password`=?,`totalNum`=? where `account`=?";
    pthread_mutex_lock(&DBConnection::mutex);
    PreparedStatement* pstmt = conn->prepareStatement(sql);
    pstmt->setString(3, ptr->getAccount());
    pstmt->setString(1, ptr->getPassword());
    pstmt->setInt(3, ptr->getTotalNum());
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


