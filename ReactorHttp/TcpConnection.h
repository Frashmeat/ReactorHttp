#pragma once
#include "EventLoop.h"
#include "Buffer.h"
struct TcpConnection
{
	//�����ķ�Ӧ��
	struct EventLoop* evLoop;
	//���ӵ�е�channel
	struct Channel* channel;
	//��������
	struct Buffer* writeBuf;
	//д������
	struct Buffer* readBuf;
	//connection ������
	char name[32];
};
//TcpConnection ��ʼ��
struct TcpConnection* tcpConnectInit(int fd,struct EventLoop* evLoop);
