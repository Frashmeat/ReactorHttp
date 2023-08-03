#include "ThreadPool.h"

ThreadPool* threadPoolInit(struct EventLoop* evLoop, int threadNum)
{
    struct ThreadPool* threadPool = new struct ThreadPool;
    threadPool->index = 0;
    threadPool->isStart = false;
    threadPool->mainLoop = evLoop;
    threadPool->workerThreads = new struct WorkerThread[threadNum];
    threadPool->threadNum = threadNum;
    return threadPool;
}

void threadPoolRun(struct ThreadPool* pool)
{
    //断言确保pool不为空 同时isStart为false
    assert(pool && !pool->isStart);
    //确保为主线程
    if (pool->mainLoop->threadId != pthread_self()) {
        exit(0);
    }
    pool->isStart = true;
    //初始化 线程池的 线程列表
    if (pool->threadNum)
    {
        for (int i = 0; i < pool->threadNum; i++)
        {
            workThreadInit(&pool->workerThreads[i], i);
            workThreadRun(&pool->workerThreads[i]);
        }
    }

}

struct EventLoop* takeWorkerEventLoop(struct ThreadPool* pool)
{
    assert(pool->isStart);
    if (pool->mainLoop->threadId != pthread_self()) {
        exit(0);
    }
    struct EventLoop* evLoop = pool->mainLoop;
    //获取当前pool中 index对应的evLoop实例
    if (pool->threadNum > 0) {
        evLoop = pool->workerThreads[pool->index].evLoop;
        //更新index
        pool->index = ++pool->index % pool->threadNum;
    }
    return evLoop;
}
