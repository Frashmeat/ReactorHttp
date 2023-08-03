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

//定义任务列表的节点
struct ChannelElement
{
	//对channel的操作
	int type;
	//channel 本体
	Channel* channel;
	//下一个节点
	ChannelElement* next;

};
struct EventLoop
{
	//是否开启
	bool isQuit;
	//分发器
	struct Dispatcher* dispatcher;
	//分发器所对应的参数
	void* dispatcherData;
	//任务列表
	struct ChannelElement* head; //头
	struct ChannelElement* tail; //尾
	//Channel对应的map
	struct ChannelMap* channelMap;

	//线程信息
	//线程id
	pthread_t threadId;
	//线程名称
	char threadName[12];
	//互斥锁
	pthread_mutex_t mutex;
	//本地socketpair 用于唤醒子线程的dispatcher 跳出阻塞状态
	int socketPair[2];

};

//初始化
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(const char* threadName);

//eventLoop 运行函数
int eventLoopRun(struct EventLoop* evLoop);
// 调用 fd 对应channel的回调函数
int eventActivate(struct EventLoop* evLoop, int fd, int event);
//添加任务到任务队列
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);
//处理任务列表的任务
int eventLoopProcessTask(struct EventLoop* evLoop);
//任务列表中的添加操作
int eventLoopTaskAdd(struct EventLoop* evLoop, struct Channel* channel);
//任务列表中的删除操作
int eventLoopTaskDelete(struct EventLoop* evLoop, struct Channel* channel);
//任务列表中的修改操作
int eventLoopTaskModify(struct EventLoop* evLoop, struct Channel* channel);
//任务列表中的删除操作后的 资源释放
int destoryChannel(struct EventLoop* evLoop ,struct Channel* channel);
