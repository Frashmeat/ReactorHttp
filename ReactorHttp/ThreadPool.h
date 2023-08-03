#pragma once
#include "EventLoop.h"
#include <assert.h>
struct ThreadPool
{
	//主反应堆模型
	struct EventLoop* mainLoop;
	//是否开启
	bool isStart;
	//线程数量
	int threadNum;
	//线程数组
	struct WorkerThread* workerThreads;
	//线程索引
	int index;

};

//初始化线程池
struct ThreadPool* threadPoolInit(struct EventLoop* evLoop,int threadNum);
//启动线程池
void threadPoolRun(struct ThreadPool* pool);
//取出子线程的反应堆的实例
struct EventLoop* takeWorkerEventLoop(struct ThreadPool* pool);