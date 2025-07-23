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
    int semid = ipc->initSem(20001, 24, 1);
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    int shmid = ipc->initShm(20001, block_num * (sizeof(int) + sizeof(MAX_BODY_LENGTH)));
    if (msgid < 0 || semid < 0 || shmid < 0) {
        cout << "ipc分配失败" << endl;
        return 0;
    }

    void* shmaddr = NULL;//用于接收共享内存块起始地址
    int indexArr[block_num];//索引区
    char shmBuffer[MAX_BODY_LENGTH];//实际内存块
    while (1) {
        //主线程负责只读消息队列，然后提交任务到线程池，
        //任务线程处理业务数据->写入共享内存->发送消息队列到前置
        IPCManager::MSGBUF msgbuf;
        msgbuf.mtype = 1;

        //1.接收到消息队列消息，最后一个参数0表示默认阻塞模式, IPC_NOWAIT表示非阻塞
        if (msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), msgbuf.mtype, 0) < 0) {
            perror("msgrcv err");
            continue;
        }
        cout << "接收到消息队列消息" << endl;
        int index = atoi(msgbuf.mtext);//知道索引区可写的下标
        //2.先获取信号量
        ipc->sem_p(semid, index);
        //3.去共享内存找到对应数据取出来交给任务
        shmaddr = shmat(shmid, NULL, 0);
        memcpy(indexArr, shmaddr, sizeof(indexArr));
        //判断目标区域是否真的可读:0可写，1可读
        if (indexArr[index] == 1) {
            //开始获取真正的数据
            memcpy(shmBuffer, (char*)shmaddr + sizeof(indexArr) + index * sizeof(shmBuffer), sizeof(shmBuffer));
        }
        ipc->sem_v(semid, index);
        HEAD head;
        memcpy(&head, shmBuffer, sizeof(HEAD));
        int bodyLen = head.bussinessLength;
        auto task = CTaskFactory::getInstance()->createTask(0, head.bussinessType, shmBuffer, sizeof(HEAD) + head.bussinessLength);
        //任务给到线程池
        pool->pushTask(move(task));
    }
    return 0;
}