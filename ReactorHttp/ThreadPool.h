#pragma once
#include "EventLoop.h"
#include <assert.h>
struct ThreadPool
{
	//����Ӧ��ģ��
	struct EventLoop* mainLoop;
	//�Ƿ���
	bool isStart;
	//�߳�����
	int threadNum;
	//�߳�����
	struct WorkerThread* workerThreads;
	//�߳�����
	int index;

};

//��ʼ���̳߳�
struct ThreadPool* threadPoolInit(struct EventLoop* evLoop,int threadNum);
//�����̳߳�
void threadPoolRun(struct ThreadPool* pool);
//ȡ�����̵߳ķ�Ӧ�ѵ�ʵ��
struct EventLoop* takeWorkerEventLoop(struct ThreadPool* pool);