#include "CParkingInfoTask.h"

CParkingInfoTask::CParkingInfoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
	headBack.bussinessLength = sizeof(bodyBack);
}

void CParkingInfoTask::work()
{
	cout << "CParkingInfoTask车辆信息查询" << endl;
	//数据解析
	memcpy(&head, taskData, sizeof(HEAD));
	ParkingInfoRequest request;
	memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
	CBaseOperation* ppvop = OperationFactory::getInstance()->createRepository(OperationFactory::RepositoryType::PARKING_PICTURE_VIEW);
	string prefix1 = "select * from ";
	string prefix2 = "select count(*) from ";
	string sql = ppvop->getTablename() + " where account='" + string(request.account) + "'";
	if (strcmp(request.carNumber, "")) {
		sql = sql + " and carNumber like '%" + string(request.carNumber) + "%'";
	}
	if (strcmp(request.entryTime, "")) {
		sql = sql + " and entryTime >= '" + string(request.entryTime) + "'";
	}
	if (strcmp(request.leaveTime, "")) {
		sql = sql + " and leaveTime <= '" + string(request.leaveTime) + "'";
	}
	//查询总数据数
	string countsql = sql;
	countsql = prefix2 + countsql;
	cout << "countsql=" << countsql << endl;
	pthread_mutex_lock(&DBConnection::mutex);
	sql::PreparedStatement* pstmt = nullptr;
	sql::ResultSet* rs;
	int count = 0;
	try{
		pstmt = DBConnection::getInstance()->getConnection()->prepareStatement(countsql);
		rs = pstmt->executeQuery(); 
		if(rs->next()) {
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


	//分页查询
	int pageSize = sizeof(bodyBack.parkingInfoDataArr) / sizeof(ParkingInfoData);//每一页的数据量
	bodyBack.totalPage = (count + pageSize - 1) / pageSize;//总页数
	int offset = (request.currentPage - 1) * pageSize;//偏移量
	sql = prefix1 + sql + " limit " + CTools::itos(pageSize) + " offset " + CTools::itos(offset);
	cout << "sql=" << sql << endl;
	auto resVec = ppvop->query<ParkingPictureView>(sql);
	bodyBack.num = 0;
	if (resVec) {
		bodyBack.num = resVec->size();
		for (int i = 0; i < resVec->size(); ++i) {
			auto& ele = (*resVec)[i];
			bodyBack.parkingInfoDataArr[i].id = ele->getId();
			bodyBack.parkingInfoDataArr[i].dueCost = ele->getDueCost();
			bodyBack.parkingInfoDataArr[i].reallyCost = ele->getReallyCost();
			strcpy(bodyBack.parkingInfoDataArr[i].carNumber, ele->getCarNumber().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].entryTime, ele->getEntryTime().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].leaveTime, ele->getLeaveTime().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].entryPicName, ele->getEntryPicName().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].entryPicPath, ele->getEntryPicKhdPath().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].leavePicName, ele->getLeavePicName().c_str());
			strcpy(bodyBack.parkingInfoDataArr[i].leavePicPath, ele->getLeavePicKhdPath().c_str());
		}
	}

	cout << "+++++++ParkingInfoBack详细信息+++++++" << endl;
	cout << "请求页=" << request.currentPage << "总页数=" << bodyBack.totalPage << " 有效数据个数=" << bodyBack.num << endl;
	cout << "sql=" << sql << endl;
	for (int i = 0; i < bodyBack.num; i++) {
		cout << "++++++++++++++++++++" << endl;
		cout << "id=" << bodyBack.parkingInfoDataArr[i].id <<" carNumber=" << bodyBack.parkingInfoDataArr[i].carNumber << endl;
		cout << "entryTime=" << bodyBack.parkingInfoDataArr[i].entryTime << " leaveTime=" << bodyBack.parkingInfoDataArr[i].leaveTime << endl;
		cout << "leavePicName=" << bodyBack.parkingInfoDataArr[i].leavePicName << " leavePicPath=" << bodyBack.parkingInfoDataArr[i].leavePicPath << endl;
		cout << "entryPicName=" << bodyBack.parkingInfoDataArr[i].entryPicName << " entryPicPath=" << bodyBack.parkingInfoDataArr[i].entryPicPath << endl;
		cout << "dueCost=" << bodyBack.parkingInfoDataArr[i].dueCost << " reallyCost=" << bodyBack.parkingInfoDataArr[i].reallyCost << endl;
		cout << "++++++++++++++++++++" << endl;
	}
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
			<< "目标请求页：" << request.currentPage << "\n"
			<< "车牌号：" << request.carNumber << "\n"
			<< "入场时间：" << request.entryTime << "\n"
			<< "出场时间：" << request.leaveTime << "\n";

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
			const ParkingInfoData& pinfo = bodyBack.parkingInfoDataArr[i];

			logStream << "停车记录id：" << pinfo.id << "\n"
				<< "  车牌号：" << pinfo.carNumber << "\n"
				<< "  入场图片名称：" << pinfo.entryPicName << "\n"
				<< "  入场图片路径：" << pinfo.entryPicPath << "\n"
				<< "  出场图片名称：" << pinfo.leavePicName << "\n"
				<< "  出场图片路径：" << pinfo.leavePicPath << "\n"
				<< "  入场时间：" << pinfo.entryTime << "\n"
				<< "  出场时间：" << pinfo.leaveTime << "\n"
				<< "  应付金额：" << pinfo.dueCost << "\n"
				<< "  实付金额：" << pinfo.reallyCost << "\n";
		}

		DataManager::writeLog(request.account, logStream.str(), currentTime);
	}
}
