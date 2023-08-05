#include "TcpServer.h"
#include <arpa/inet.h>
#include "TcpConnection.h"
struct TcpServer* tcpServerInit(unsigned short port, int threadNum)
{
	struct TcpServer* tcp = (struct TcpServer*)malloc(sizeof(struct TcpServer));
	tcp->listener = listenerInit(port);
	tcp->mainLoop = eventLoopInit();
	tcp->threadNum = threadNum;
	tcp->threadPool = threadPoolInit(tcp->mainLoop, threadNum);
	return tcp;
}

struct Listener* listenerInit(unsigned short port)
{

	struct Listener* listener = (struct Listener*)malloc(sizeof(struct Listener));
	//创建FD
	int FDListener = socket(AF_INET, SOCK_STREAM, 0);
	if (FDListener == -1) {
		perror("socket fail to create");
		return nullptr;
	}
	//设置端口复用
	int opt = 1;
	int res = setsockopt(FDListener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (res == -1) {
		perror("fail to setsocketopt");
		return nullptr;
	}
	//绑定端口
	sockaddr_in lfd;
	lfd.sin_family = AF_INET;
	lfd.sin_port = htons(port);
	lfd.sin_addr.s_addr = INADDR_ANY;
	res = bind(FDListener, (sockaddr*)&lfd, sizeof lfd);
	if (res == -1) {
		perror("fail to bind");
		return nullptr;
	}
	//设置监听
	// 最高128
	res = listen(FDListener, 128);

	if (res == -1) {
		perror("fail to listen");
		return nullptr;
	}
	listener->lfd = FDListener;
	listener->port = port;
	return listener;
}

int acceptConnection(void* arg) 
{
	struct TcpServer* server = (struct TcpServer*)arg;
	//与客户端建立链接
	int cfd = accept(server->listener->lfd, NULL, NULL);

	//从线程池中取出一个工作子线程进行处理
	struct EventLoop* evLoop = takeWorkerEventLoop(server->threadPool);
	//将cfd放入tcpConnection中处理
	tcpConnectInit(cfd, evLoop);


};

void tcpServerRun(TcpServer* server)
{	
	//启动线程池
	threadPoolRun(server->threadPool);
	//添加channel
	struct Channel* channel = channelInit(server->listener->lfd, ReadEvent, acceptConnection, NULL, server);
	eventLoopAddTask(server->mainLoop, channel, ADD);
	//启动反应堆模型
	eventLoopRun(server->mainLoop);
}
