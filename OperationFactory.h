#pragma once
#include <pthread.h>
#include "UserOperation.h"
#include "CBaseOperation.h"
#include "PictureOperation.h"
#include "ParkingOperation.h"
#include "VideoOperation.h"
#include "LogOperation.h"
class OperationFactory
{
public:
	~OperationFactory();
	static OperationFactory* getInstance();
	enum class RepositoryType { USER, PICTURE, PARKING, VIDEO, LOG};
	CBaseOperation* createRepository(RepositoryType type);
private:
	OperationFactory();
	static OperationFactory* instance;
	static pthread_mutex_t mutex;
	UserOperation* userOp;
	PictureOperation* picOp;
	ParkingOperation* parkOp;
	VideoOperation* vdOp;
	LogOperation* logOp;
};

