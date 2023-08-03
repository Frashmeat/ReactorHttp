#pragma once
#include "EventLoop.h"
#include <pthread.h>

struct WorkerThread
{
	pthread_t threadId; //线程id
	char name[24]; //线程名称
	pthread_mutex_t mutex; //互斥锁
	pthread_cond_t cond; //条件变量
	struct EventLoop* evLoop; //反应堆模型
};

//初始化
int workThreadInit(struct WorkerThread* thread, int index);
//启动线程
void workThreadRun(struct WorkerThread* thread);