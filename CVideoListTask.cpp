#include "CVideoListTask.h"

CVideoListTask::CVideoListTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
    headBack.bussinessLength = sizeof(bodyBack);
    headBack.bussinessType = 18;
    headBack.crc = this->clientFd;
}

void CVideoListTask::work()
{
    cout << "CVideoListTask获取视频列表" << endl;
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    VideoListRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    string sql;
    bodyBack.currentPage = request.requestPage;//请求目标页
    int pageSize = sizeof(bodyBack.videoDataArr) / sizeof(VideoData);//每一页的数据量
    int offset = (request.requestPage - 1) * pageSize;//偏移量
    bodyBack.num = 0;//有效数据个数
    if (request.queryFlag) {
        //按月
        sql = "select * from video_info where account='" + string(request.account) + "' and create_month='" +
            string(request.dateTime) + "' ORDER BY createtime asc limit " + CTools::itos(pageSize) + " offset " + CTools::itos(offset);
    }
    else {
        //按天
        sql = "select * from video_info where account='" + string(request.account) + "' and create_date='" +
            string(request.dateTime) + "' ORDER BY createtime asc limit " + CTools::itos(pageSize) + " offset " + CTools::itos(offset);
    }
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
    cout << "请求页=" << bodyBack.currentPage << " 有效数据个数=" << bodyBack.num << endl;
    cout << "sql=" << sql << endl;
    cout << "++++++++++++++++++++" << endl;

    //准备数据缓冲区
    char buffer[sizeof(HEAD) + sizeof(bodyBack)];
    memcpy(buffer, &headBack, sizeof(HEAD));
    memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
    //数据存放共享内存
    IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);
}
