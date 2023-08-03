#include "WorkerThread.h"

int workThreadInit(WorkerThread* thread, int index)
{
    thread->threadId = NULL;
    thread->evLoop = NULL;

    sprintf(thread->name, "SubThread-%d", index);

    pthread_mutex_init(&thread->mutex, NULL);
    pthread_cond_init(&thread->cond, NULL);

    return 0;
}

void* SubThreadRunning(void* arg) {
    struct WorkerThread* thread = (struct WorkerThread*)arg;
    pthread_mutex_lock(&thread->mutex);
    thread->evLoop = eventLoopInitEx(thread->name);
    pthread_mutex_unlock(&thread->mutex);
    //发送信号 evLoop 已经初始化完成 
    pthread_cond_signal(&thread->cond);
    eventLoopRun(thread->evLoop);
    return NULL;
}

void workThreadRun(WorkerThread* thread)
{
    //创建并启动
    pthread_create(&thread->threadId, NULL, SubThreadRunning, thread);
    //利用条件变量阻塞线程 确保evLoop 不为NULL
    pthread_mutex_lock(&thread->mutex);
    while (thread->evLoop == NULL) {
        pthread_cond_wait(&thread->cond, &thread->mutex);
    }
    pthread_mutex_unlock(&thread->mutex);

}
