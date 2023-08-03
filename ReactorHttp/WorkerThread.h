#pragma once
#include "EventLoop.h"
#include <pthread.h>

struct WorkerThread
{
	pthread_t threadId; //�߳�id
	char name[24]; //�߳�����
	pthread_mutex_t mutex; //������
	pthread_cond_t cond; //��������
	struct EventLoop* evLoop; //��Ӧ��ģ��
};

//��ʼ��
int workThreadInit(struct WorkerThread* thread, int index);
//�����߳�
void workThreadRun(struct WorkerThread* thread);