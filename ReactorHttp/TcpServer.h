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
	//线程池的数量
	int threadNum;
	//主线程反应堆
	struct EventLoop* mainLoop;
	//线程池
	struct ThreadPool* threadPool;
	//监听的fd 和 端口
	struct Listener* listener;
};

//初始化 TcpServer
struct TcpServer* tcpServerInit(unsigned short port, int threadNum);
//初始化 Listener
struct Listener* listenerInit(unsigned short port);
//启动TCPServer
void tcpServerRun(struct TcpServer* server);



