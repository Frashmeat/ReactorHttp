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
    //����ȷ��pool��Ϊ�� ͬʱisStartΪfalse
    assert(pool && !pool->isStart);
    //ȷ��Ϊ���߳�
    if (pool->mainLoop->threadId != pthread_self()) {
        exit(0);
    }
    pool->isStart = true;
    //��ʼ�� �̳߳ص� �߳��б�
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
    //��ȡ��ǰpool�� index��Ӧ��evLoopʵ��
    if (pool->threadNum > 0) {
        evLoop = pool->workerThreads[pool->index].evLoop;
        //����index
        pool->index = ++pool->index % pool->threadNum;
    }
    return evLoop;
}
