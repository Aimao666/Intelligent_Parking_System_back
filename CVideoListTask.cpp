#include "CVideoListTask.h"

CVideoListTask::CVideoListTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
    headBack.bussinessLength = sizeof(bodyBack);
}

void CVideoListTask::work()
{
    cout << "CVideoListTask获取视频列表" << endl;
    //数据解析
    VideoListRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    string sql;
    int pageSize = sizeof(bodyBack.videoDataArr) / sizeof(VideoData);//每一页的数据量
    int offset = (request.requestPage - 1) * pageSize;//偏移量
    bodyBack.num = 0;//有效数据个数
    string postfix =  " limit " + CTools::itos(pageSize) + " offset " + CTools::itos(offset);
    if (request.queryFlag) {
        //按月
        sql = "from video_info where account='" + string(request.account) + "' and create_month='" + string(request.dateTime) +
            "' ORDER BY createtime asc";
            
    }
    else {
        //按天
        sql = "from video_info where account='" + string(request.account) + "' and create_date='" + string(request.dateTime) +
            "' ORDER BY createtime asc";
    }
    //查询总数据数
    string countsql = "select count(*) " + sql;
    cout << "countsql=" << countsql << endl;
    pthread_mutex_lock(&DBConnection::mutex);
    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* rs;
    int count = 0;
    try {
        pstmt = DBConnection::getInstance()->getConnection()->prepareStatement(countsql);
        rs = pstmt->executeQuery();
        if (rs->next()) {
            count = rs->getInt(1);//总数据量
        }
    }
    catch (SQLException& e) {
        cerr << "SQLException:" << e.what() << endl;
        cerr << "sql=" << countsql << endl;
        delete rs;
        delete pstmt;
        pthread_mutex_unlock(&DBConnection::mutex);
    }
    delete rs;
    delete pstmt;
    pthread_mutex_unlock(&DBConnection::mutex);
    bodyBack.totalPage = (count + pageSize - 1) / pageSize;//总页数


    //分页查询
    sql = "select * " + sql + postfix;
    cout << "sql=" << sql << endl;
    auto resVec = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::VIDEO)->query<Video>(sql);
    if (resVec) {
        bodyBack.num = resVec->size();
        for (int i = 0; i < resVec->size(); i++) {
            auto& ele = (*resVec)[i];
            bodyBack.videoDataArr[i].videoId = ele->getId();
            strcpy(bodyBack.videoDataArr[i].createtime, ele->getCreatetime().c_str());
            strcpy(bodyBack.videoDataArr[i].currentPlaytime, ele->getCurrentPlaytime().c_str());
            strcpy(bodyBack.videoDataArr[i].vname, ele->getVname().c_str());
            strcpy(bodyBack.videoDataArr[i].videoPath, ele->getVpath().c_str());
            strcpy(bodyBack.videoDataArr[i].picturePath, ele->getPpath().c_str());
            strcpy(bodyBack.videoDataArr[i].totaltime, ele->getTotaltime().c_str());

        }
    }
    cout << "+++++++VideoListBack详细信息+++++++" << endl;
    cout << "请求页=" << request.requestPage << "总页数=" << bodyBack.totalPage << " 有效数据个数=" << bodyBack.num << endl;
    cout << "sql=" << sql << endl;
    cout << "++++++++++++++++++++" << endl;

    //准备数据缓冲区
    char buffer[sizeof(HEAD) + sizeof(bodyBack)];
    memcpy(buffer, &headBack, sizeof(HEAD));
    memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
    //数据存放共享内存
    IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);

    // 日志记录 - 请求部分
    {
        std::ostringstream logStream;
        std::string currentTime = CTools::getDatetime();

        logStream << "时间：" << currentTime << "\n"
            << "功能：获取视频列表\n"
            << "类型：请求\n"
            << "用户账号：" << request.account << "\n"
            << "目标请求页：" << request.requestPage << "\n"
            << "查询标志0按天查，1按月查：" << request.queryFlag << "\n"
            << "查询时间：" << request.dateTime << "\n";

        DataManager::writeLog(request.account, logStream.str(), currentTime);
    }

    // 日志记录 - 响应部分
    {
        std::ostringstream logStream;
        std::string currentTime = CTools::getDatetime();

        logStream << "时间：" << currentTime << "\n"
            << "功能：获取视频列表\n"
            << "类型：响应\n"
            << "用户账号：" << request.account << "\n"
            << "总页数：" << bodyBack.totalPage << "\n"
            << "有效数据个数：" << bodyBack.num << "\n"
            << "数据列表：\n";

        for (int i = 0; i < bodyBack.num; ++i) {
            const VideoData& video = bodyBack.videoDataArr[i];

            logStream << "视频id：" << video.videoId << "\n"
                << "  视频总时长：" << video.totaltime << "\n"
                << "  视频名称：" << video.vname << "\n"
                << "  视频路径：" << video.videoPath << "\n"
                << "  上次播放时长：" << video.currentPlaytime << "\n"
                << "  视频创建时间：" << video.createtime << "\n"
                << "  视频首帧图片路径：" << video.picturePath << "\n\n";
        }

        DataManager::writeLog(request.account, logStream.str(), currentTime);
    }
}
