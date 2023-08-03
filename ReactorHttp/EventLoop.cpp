#include "EventLoop.h"


//对本地socket进行写数据
void taskWakeUp(struct EventLoop* evLoop) {
	const char* msg = "wake up";
	write(evLoop->socketPair[1], msg, strlen(msg));
}
//读取本地socket发送的数据
int readLocalMsg(void* arg) {
	struct EventLoop* evLoop = (EventLoop*)arg;
	char buf[256];
	read(evLoop->socketPair[1], buf, sizeof(buf));
	return 0;
}
struct EventLoop* eventLoopInit()
{
	return eventLoopInitEx(NULL);
}



struct EventLoop* eventLoopInitEx(const char* threadName)
{
	struct EventLoop* eventLoop = new EventLoop;
	eventLoop->isQuit = false;
	//线程信息
	eventLoop->threadId = pthread_self();
	pthread_mutex_init(&eventLoop->mutex, NULL);
	strcpy(eventLoop->threadName, threadName == NULL ? "MainThread" : threadName);
	//dispatcher 初始化
	eventLoop->dispatcher = &EpollDispatcher;
	eventLoop->dispatcherData = eventLoop->dispatcher->init();
	//任务链表 初始化
	eventLoop->head = eventLoop->tail = NULL;
	eventLoop->channelMap = ChannelMapInit(128);
	//socketpair的初始化 AF_UNIX 代表这是一对用于本地通信的socket
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair);
	if (ret == -1) {
		perror("fail to create socketPair");
		exit(0);
	}
	//指定规则 socketPair[0] 用于发数据  socketPair[1] 用于接收数据
	struct Channel* channel = channelInit(eventLoop->socketPair[1], ReadEvent,
		readLocalMsg, NULL, eventLoop);
	//添加到任务队列
	eventLoopAddTask(eventLoop, channel, ADD);
	return eventLoop;
}

int eventLoopRun(EventLoop* evLoop) {
	//断言evLoop 不可能为Null
	assert(evLoop != NULL);

	struct Dispatcher* dispatcher = evLoop->dispatcher;
	if (evLoop->threadId != pthread_self()) {
		return -1;
	}
	while (!evLoop->isQuit) {
		//开始监听
		dispatcher->dispatch(evLoop, 2);
		eventLoopProcessTask(evLoop);
	}
	return 0;
}

int eventActivate(EventLoop* evLoop, int fd, int event)
{
	//确定参数有效
	if (evLoop == NULL || fd < 0) {
		return -1;
	}
	//取出channel
	struct Channel* channel = evLoop->channelMap->list[fd];
	//确保channel正确
	assert(channel->fd == fd);

	//调用回调函数
	if (event & ReadEvent && channel->readCallBack) {
		channel->readCallBack(channel->arg);
	}
	if (event & WriteEvent && channel->writeCallBack) {
		channel->writeCallBack(channel->arg);
	}

	return 0;
}


int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type) {
	//加锁
	pthread_mutex_lock(&evLoop->mutex);
	//创建新节点
	//ChannelElement* newNode = new ChannelElement;
	struct ChannelElement* newNode = (struct ChannelElement*)malloc(sizeof(struct ChannelElement));
	newNode->channel = channel;
	newNode->type = type;
	newNode->next = NULL;
	//加入链表中
	//链表为空时
	if (evLoop->head == evLoop->tail) {
		evLoop->head = evLoop->tail = newNode;
	}
	else {
		//链表不为空时
		evLoop->tail->next = newNode;
		evLoop->tail = newNode;
	}
	pthread_mutex_unlock(&evLoop->mutex);

	//如果为子线程在调用 由子线程自己处理
	if (evLoop->threadId == pthread_self()) {
		//子线程
		eventLoopProcessTask(evLoop);
	}
	else {
		//主线程
		//需要用本地的socket通信 来激活子线程的 dispacher 跳出阻塞状态
		taskWakeUp(evLoop);

	}
	//如果为主线程在调用 唤醒子线程处理

	return 0;
};

int eventLoopTaskAdd(struct EventLoop* evLoop, struct Channel* channel) {

	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//判断fd是否大于map的size 如果大于就扩容
	if (channelMap->size <= fd) {
		if (!makeMapRoom(channelMap, fd, sizeof(struct Channel*))) {
			return -1;
		}
	}

	//将channel装入map中 ,同时添加到dispatcher的监听队列中
	if (channelMap->list[fd] == NULL) {
		channelMap->list[fd] = channel;
		evLoop->dispatcher->add(channel, evLoop);
	}
	return 0;
}
int eventLoopTaskDelete(EventLoop* evLoop, Channel* channel)
{
	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//判断fd是否大于map的size 没有就直接返回
	if (channelMap->size <= fd) {
		return -1;
	}
	int ret = evLoop->dispatcher->remove(channel, evLoop);
	return ret;
}
int eventLoopTaskModify(EventLoop* evLoop, Channel* channel)
{
	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//判断fd是否大于map的size 或者list 中没有对应的channel就直接返回
	if (channelMap->size <= fd || channelMap->list[fd] == NULL) {
		return -1;
	}
	int ret = evLoop->dispatcher->modify(channel, evLoop);
	return ret;
}
int destoryChannel(EventLoop* evLoop, Channel* channel)
{
	//channelMap 中删除 channel
	evLoop->channelMap->list[channel->fd] = NULL;
	//关闭channel
	close(channel->fd);
	//释放channel资源
	free(channel);
	return 0;
}
int eventLoopProcessTask(struct EventLoop* evLoop) {
	pthread_mutex_lock(&evLoop->mutex);
	struct ChannelElement* head = evLoop->head;
	while (head != NULL) {
		struct Channel* channel = head->channel;
		if (head->type == ADD) {
			//添加操作
			eventLoopTaskAdd(evLoop, channel);
		}
		else if (head->type = DELETE) {
			//删除操作
			eventLoopTaskDelete(evLoop, channel);
			//释放资源

		}
		else if (head->type == MODIFY) {
			//修改操作
			eventLoopTaskModify(evLoop, channel);
		}
		ChannelElement* temp = head;
		head = head->next;
		free(head);
	}
	pthread_mutex_unlock(&evLoop->mutex);
	return 0;
};