#include "OperationFactory.h"

OperationFactory* OperationFactory::instance = nullptr;
pthread_mutex_t OperationFactory::mutex;

OperationFactory* OperationFactory::getInstance()
{
    //线程锁，保证饿汉式线程安全
    pthread_mutex_lock(&mutex);
    if (instance == nullptr) {
        instance = new OperationFactory();
    }
    pthread_mutex_unlock(&mutex);
    return instance;
}
OperationFactory::OperationFactory()
{
    this->userOp = nullptr;
}
OperationFactory::~OperationFactory()
{
}
CBaseOperation* OperationFactory::createRepository(RepositoryType type)
{
    switch (type)
    {
    case OperationFactory::RepositoryType::USER:
    {
        pthread_mutex_lock(&mutex);
        if (userOp == nullptr) {
            userOp = new UserOperation();
        }
        pthread_mutex_unlock(&mutex);
        return userOp; 
    }
    case OperationFactory::RepositoryType::PICTURE:
    {
        pthread_mutex_lock(&mutex);
        if (picOp == nullptr) {
            picOp = new PictureOperation();
        }
        pthread_mutex_unlock(&mutex);
        return picOp;
    }
    default:
        break;
    }
    return nullptr;
}


