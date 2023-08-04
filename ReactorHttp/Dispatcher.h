#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher
{
	//��ʼ��
	void* (*init)();
	//���
	int (*add)(struct Channel* channel, struct EventLoop* evLoop);
	//ɾ��
	int (*remove)(struct Channel* channel, struct EventLoop* evLoop);
	//�޸�
	int (*modify)(struct Channel* channel, struct EventLoop* evLoop);
	//�¼�����
	int (*dispatch)(struct EventLoop* evLoop,int timeout);
	//����
	int (*clear)(struct EventLoop * evLoop);
};

