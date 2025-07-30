#pragma once
#include "DBConnection.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <memory>
class CBaseOperation {
public:
    CBaseOperation();
    virtual ~CBaseOperation() = default;

    // 通用查询模板方法
    //查询到或者没查到都会返回容器智能指针，但是如果报了SQLException则返回nullptr
    template <typename T>
    std::unique_ptr<std::vector<std::unique_ptr<T>>> query(const std::string& sql = "") {
        std::unique_ptr<std::vector<std::unique_ptr<T>>>results(new std::vector<std::unique_ptr<T>>());
        //sql为空则走默认的全表查询，不为空则sql是什么就执行什么，千万不要传错sql了
        std::string finalSql = sql.empty() ? "SELECT * FROM " + tablename : sql;

        sql::ResultSet* rs = nullptr;
        pthread_mutex_lock(&DBConnection::mutex);
        sql::PreparedStatement* pstmt = conn->prepareStatement(finalSql);
        try {
            rs = pstmt->executeQuery();
            while (rs->next()) {
                std::unique_ptr<T>obj(new T());
                fillObjectFromResultSet(rs, obj.get());
                //std::unique_ptr<T> obj = CBaseOperation::template createFromResultSet(rs);
                if (obj) {
                    results->push_back(std::move(obj));//需要使用move才能触发移动构造，否则触发的是被删除的拷贝构造
                }
            }
        }
        catch (sql::SQLException& e) {
            std::cerr << "SQL Exception in query: " << e.what() << std::endl;
            delete rs;
            delete pstmt;
            pthread_mutex_unlock(&DBConnection::mutex);
            return nullptr;
        }

        delete rs;
        delete pstmt;
        pthread_mutex_unlock(&DBConnection::mutex);
        return results;//直接返回就行vec会被视为将亡值触发移动构造
    }


    // 其他通用操作接口
    virtual int doInsert(void* pocoPtr) = 0;
    virtual int doUpdate(void* pocoPtr) = 0;

    //实现的基本的DML操作，不对传入的sql进行任何校验，直接执行，返回受影响的行数
    virtual int executeUpdate(string sql);
    virtual int doDelete(const std::string& whereSql = "");//应传入where子句

    //getter
    std::string getTablename() const;

protected:
    // 纯虚函数 - 由子类实现具体对象的创建逻辑
    virtual void fillObjectFromResultSet(sql::ResultSet* rs, void* object) = 0;
    DBConnection* connection;
    sql::Connection* conn;
    std::string tablename;

};