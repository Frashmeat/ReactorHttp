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
    //�����ź� evLoop �Ѿ���ʼ����� 
    pthread_cond_signal(&thread->cond);
    eventLoopRun(thread->evLoop);
    return NULL;
}

void workThreadRun(WorkerThread* thread)
{
    //����������
    pthread_create(&thread->threadId, NULL, SubThreadRunning, thread);
    //�����������������߳� ȷ��evLoop ��ΪNULL
    pthread_mutex_lock(&thread->mutex);
    while (thread->evLoop == NULL) {
        pthread_cond_wait(&thread->cond, &thread->mutex);
    }
    pthread_mutex_unlock(&thread->mutex);

}
