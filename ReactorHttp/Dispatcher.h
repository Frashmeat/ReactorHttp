#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher
{
	//��ʼ��
	void* (*init)();
	//���
	int (*add)(Channel* channel, struct EventLoop* evLoop);
	//ɾ��
	int (*remove)(Channel* channel, struct EventLoop* evLoop);
	//�޸�
	int (*modify)(Channel* channel, struct EventLoop* evLoop);
	//�¼�����
	int (*dispatch)(struct EventLoop* evLoop,int timeout);
	//����
	int (*clear)(struct EventLoop * evLoop);
};

