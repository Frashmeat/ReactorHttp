#pragma once
#include "EventLoop.h"
#include "Buffer.h"
struct TcpConnection
{
	//所属的反应堆
	struct EventLoop* evLoop;
	//最初拥有的channel
	struct Channel* channel;
	//读缓冲区
	struct Buffer* writeBuf;
	//写缓冲区
	struct Buffer* readBuf;
	//connection 的名称
	char name[32];
};
//TcpConnection 初始化
struct TcpConnection* tcpConnectInit(int fd,struct EventLoop* evLoop);
