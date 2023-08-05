#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"
struct Listener
{
	int lfd;
	unsigned short port;
};

struct TcpServer
{
	//�̳߳ص�����
	int threadNum;
	//���̷߳�Ӧ��
	struct EventLoop* mainLoop;
	//�̳߳�
	struct ThreadPool* threadPool;
	//������fd �� �˿�
	struct Listener* listener;
};

//��ʼ�� TcpServer
struct TcpServer* tcpServerInit(unsigned short port, int threadNum);
//��ʼ�� Listener
struct Listener* listenerInit(unsigned short port);
//����TCPServer
void tcpServerRun(struct TcpServer* server);



