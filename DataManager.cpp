#include "DataManager.h"

const string DataManager::basePath= "./AppData/data/";
map<FileInfo::FileKey, unique_ptr<FileInfo>> DataManager::allFileMap;
pthread_mutex_t DataManager::allFileMapMutex;