#include "CVideoDateTask.h"

CVideoDateTask::CVideoDateTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
    headBack.bussinessLength = sizeof(bodyBack);
    headBack.bussinessType = 16;
    headBack.crc = this->clientFd;
}

void CVideoDateTask::work()
{
    cout << "CVideoDateTask获取存在的视频日期" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    VideoDateRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    int flag = request.queryFlag;
    bodyBack.queryFlag = request.queryFlag;
    bodyBack.year = request.year;
    bodyBack.count = 0;
    string postfix = "FROM video_info WHERE account = '" + string(request.account) + "' AND YEAR(createtime) = '"
        +CTools::itos(request.year)+"' ORDER BY date;";
    string sql;
    if (flag) {
        //按月
        sql = "SELECT DISTINCT create_month AS date " + postfix;
    }
    else {
        //按天
        sql = "SELECT DISTINCT create_date AS date " + postfix;
    }
    pthread_mutex_lock(&DBConnection::mutex);
    sql::PreparedStatement *pstmt = DBConnection::getInstance()->getConnection()->prepareStatement(sql);
    sql::ResultSet* rs;
    try {
        rs = pstmt->executeQuery();
        while (rs->next()) {
            strcpy(bodyBack.dates[bodyBack.count++], rs->getString("date").c_str());
        }
    }
    catch (sql::SQLException e) {
        cerr << "SQLException:" << e.what() << "\nsql=" << sql << endl;
        delete rs;
        delete pstmt;
        pthread_mutex_unlock(&DBConnection::mutex);
    }
    delete rs;
    delete pstmt;
    pthread_mutex_unlock(&DBConnection::mutex);
    cout << "+++++CVideoDateBack详细信息+++++" << endl;
    cout << "count=" << bodyBack.count << " queryFlag = " << bodyBack.queryFlag << endl;
    cout << "year=" << bodyBack.year << endl;

    //准备数据缓冲区
    char buffer[sizeof(HEAD) + sizeof(bodyBack)];
    memcpy(buffer, &headBack, sizeof(HEAD));
    memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
    //数据存放共享内存
    IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
