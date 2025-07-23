#pragma once
#include <pthread.h>
#include "UserOperation.h"
#include "CBaseOperation.h"
class OperationFactory
{
public:
	~OperationFactory();
	static OperationFactory* getInstance();
	enum class RepositoryType { USER };
	CBaseOperation* createRepository(RepositoryType type);
private:
	OperationFactory();
	static OperationFactory* instance;
	static pthread_mutex_t mutex;
	UserOperation* userOp;
	
};

