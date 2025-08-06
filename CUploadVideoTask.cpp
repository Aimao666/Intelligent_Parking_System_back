#include "CUploadVideoTask.h"

CUploadVideoTask::CUploadVideoTask(int fd, char* data, size_t len)
    :CBaseTask(fd, data, len)
{
    headBack.bussinessLength = sizeof(bodyBack);
}

void CUploadVideoTask::work()
{
    cout << "CUploadVideoTask正在执行" << endl;
	//数据解析
	UploadVideoRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);


	Video videoInfo(request.account, request.vname, request.vpath, request.totaltime, request.createtime, request.pname, request.ppath);

	//将视频信息插入数据库
	int rows = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::VIDEO)->doInsert(&videoInfo);
	if (rows > 0) {
		cout << "视频信息保存成功" << endl;
		bodyBack.flag = 1;
		strcpy(bodyBack.message, "视频信息保存成功");
	}
	else {
		cout << "视频信息保存失败" << endl;
		bodyBack.flag = 0;
		strcpy(bodyBack.message, "视频信息保存失败");
	}
	//准备数据缓冲区
	char buffer[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buffer, &headBack, sizeof(HEAD));
	memcpy(buffer + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//数据存放共享内存
	IPCManager::getInstance()->saveData(buffer, sizeof(buffer), 2);

    // 请求日志记录
    {
        std::ostringstream logStream;
        std::string currentTime = CTools::getDatetime();

        logStream << "时间：" << currentTime << "\n"
            << "功能：上传视频录制信息\n"
            << "类型：请求\n"
            << "用户账号：" << request.account << "\n"
            << "视频名称：" << request.vname << "\n"
            << "视频路径：" << request.vpath << "\n"
            << "视频总时长：" << request.totaltime << "\n"
            << "视频创建时间：" << request.createtime << "\n"
            << "视频首帧图片名称：" << request.pname << "\n"
            << "视频首帧图片路径：" << request.ppath << "\n";

        DataManager::writeLog(request.account, logStream.str(), currentTime);
    }

    // 响应日志记录
    {
        std::ostringstream logStream;
        std::string currentTime = CTools::getDatetime();

        logStream << "时间：" << currentTime << "\n"
            << "功能：上传视频录制信息\n"
            << "类型：响应\n"
            << "用户账号：" << request.account << "\n"
            << "结果标志：" << bodyBack.flag << "\n"
            << "说明：" << bodyBack.message << "\n";

        DataManager::writeLog(request.account, logStream.str(), currentTime);
    }
}
