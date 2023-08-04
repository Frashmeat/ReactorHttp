#pragma once
#include "Dispatcher.h"
#include "ChannelMap.h"
#include "WorkerThread.h"
#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>

extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;
enum ElementType
{
	ADD,
	DELETE,
	MODIFY
};

//���������б�Ľڵ�
struct ChannelElement
{
	//��channel�Ĳ���
	int type;
	//channel ����
	Channel* channel;
	//��һ���ڵ�
	ChannelElement* next;

};
struct EventLoop
{
	//�Ƿ���
	bool isQuit;
	//�ַ���
	struct Dispatcher* dispatcher;
	//�ַ�������Ӧ�Ĳ���
	void* dispatcherData;
	//�����б�
	struct ChannelElement* head; //ͷ
	struct ChannelElement* tail; //β
	//Channel��Ӧ��map
	struct ChannelMap* channelMap;

	//�߳���Ϣ
	//�߳�id
	pthread_t threadId;
	//�߳�����
	char threadName[12];
	//������
	pthread_mutex_t mutex;
	//����socketpair ���ڻ������̵߳�dispatcher ��������״̬
	int socketPair[2];

};

//��ʼ��
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(const char* threadName);

//eventLoop ���к���
int eventLoopRun(struct EventLoop* evLoop);
// ���� fd ��Ӧchannel�Ļص�����
int eventActivate(struct EventLoop* evLoop, int fd, int event);
//��������������
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);
//���������б������
int eventLoopProcessTask(struct EventLoop* evLoop);
//�����б��е���Ӳ���
int eventLoopTaskAdd(struct EventLoop* evLoop, struct Channel* channel);
//�����б��е�ɾ������
int eventLoopTaskDelete(struct EventLoop* evLoop, struct Channel* channel);
//�����б��е��޸Ĳ���
int eventLoopTaskModify(struct EventLoop* evLoop, struct Channel* channel);
//�����б��е�ɾ��������� ��Դ�ͷ�
int destoryChannel(struct EventLoop* evLoop ,struct Channel* channel);
