#include <cstdio>
#include "CThreadPool.h"
#include "IPCManager.h"
#include "protocol.h"
#include <string.h>
#include "CTaskFactory.h"
int main()
{
    CThreadPool* pool = new CThreadPool();
    IPCManager* ipc = IPCManager::getInstance();
    int msgid = ipc->initMsg(20001);
    int semid = ipc->initSem(20001, 48, 1);
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    size_t shm_size = block_num * (sizeof(int) + MAX_BODY_LENGTH);
    cout << "block_num大小: " << block_num << endl;
    cout << "共享内存大小: " << shm_size << endl;
    int shmid = ipc->initShm(20001, block_num * (sizeof(int) + MAX_BODY_LENGTH));
    if (msgid < 0 || semid < 0 || shmid < 0) {
        cout << "ipc分配失败" << endl;
        return 0;
    }
    cout << "shmid=" << shmid << "getShmid()" << ipc->getShmid() << endl;
    void* shmaddr = shmat(shmid, NULL, 0);//用于接收共享内存块起始地址
    if (shmaddr == (void*)-1) {
        perror("shmat 失败");
        cout << "无法连接共享内存，程序退出" << endl;
        return -1;
    }
    int indexArr[block_num];//索引区
    while (1) {
        //主线程负责只读消息队列，然后提交任务到线程池，
        //任务线程处理业务数据->写入共享内存->发送消息队列到前置
        IPCManager::MSGBUF msgbuf;
        msgbuf.mtype = 1;

        //接收到消息队列消息，最后一个参数0表示默认阻塞模式, IPC_NOWAIT表示非阻塞
        if (msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), msgbuf.mtype, 0) < 0) {
            perror("msgrcv err");
            continue;
        }
        cout << "接收到消息队列消息:" << msgbuf.mtext<< endl;
        int index = atoi(msgbuf.mtext);//知道索引区可写的下标
         // 检查索引范围
        if (index < 0 || index >= block_num) {
            cerr << "无效索引: " << index << endl;
            continue;
        }

        ipc->sem_p(semid, index);
        memcpy(indexArr, shmaddr, sizeof(indexArr));
        //判断目标区域是否真的可读:0可写，1可读
        if (indexArr[index] == 1) {
            cout << "目标区域可读" << endl;
            //拷贝数据
            char shmBuffer[MAX_BODY_LENGTH];//实际内存块
            memcpy(shmBuffer, (char*)shmaddr + sizeof(int)* block_num + index * MAX_BODY_LENGTH, MAX_BODY_LENGTH);
            HEAD head;
            memcpy(&head, shmBuffer, sizeof(HEAD));
            int bodyLen = head.bussinessLength;
            auto task = CTaskFactory::getInstance()->createTask(0, head.bussinessType, shmBuffer, sizeof(HEAD) + head.bussinessLength);
            //任务给到线程池
            pool->pushTask(move(task));
        }
        ipc->sem_v(semid, index);
    }
    return 0;
}