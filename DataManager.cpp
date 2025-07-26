#include "DataManager.h"

const string DataManager::basePath= "/root/projects/AppData/Intelligent_Parking_System/data/";
map<FileInfo::FileKey, unique_ptr<FileInfo>> DataManager::allFileMap;
pthread_mutex_t DataManager::allFileMapMutex;