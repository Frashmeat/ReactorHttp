#include "Dispatcher.h"
#include <sys/select.h>


#define Max 1024
struct SelectData
{
	fd_set readSet;
	fd_set writeSet;
};

//初始化
static void* selectInit();
//添加
static int selectAdd(Channel* channel, EventLoop* evLoop);
//删除	  
static int selectRemove(Channel* channel, EventLoop* evLoop);
//修改	  
static int selectModify(Channel* channel, EventLoop* evLoop);
//事件监听	
static int selectDispatch(EventLoop* evLoop, int timeout);
//清理	  
static int selectClear(EventLoop* evLoop);

void fdClr(Channel* channel, struct SelectData* data) {
	if (channel->events & ReadEvent) {
		FD_CLR(channel->fd, &data->readSet);
	}
	if (channel->events & WriteEvent) {
		FD_CLR(channel->fd, &data->writeSet);
	}
};

void fdSet(Channel* channel, struct SelectData* data) {
	if (channel->events & ReadEvent) {
		FD_SET(channel->fd, &data->readSet);
	}
	if (channel->events & WriteEvent) {
		FD_SET(channel->fd, &data->writeSet);
	}
}

Dispatcher SelectDispatcher = {
	selectInit,
	selectAdd,
	selectRemove,
	selectModify,
	selectDispatch,
	selectClear
};




void* selectInit()
{
	SelectData* data = new SelectData;
	FD_ZERO(&data->readSet);
	FD_ZERO(&data->writeSet);
	return data;
}

int selectAdd(Channel* channel, EventLoop* evLoop)
{
	SelectData* data = (SelectData*)evLoop->dispatcherData;
	if (channel->fd >= Max) {
		perror("fail to add");
		exit(1);
	}
	fdSet(channel, data);
	return 0;
}

int selectRemove(Channel* channel, EventLoop* evLoop)
{
	SelectData* data = (SelectData*)evLoop->dispatcherData;

	fdClr(channel, data);

	return 0;
}

int selectModify(Channel* channel, EventLoop* evLoop)
{
	SelectData* data = (SelectData*)evLoop->dispatcherData;
	fdClr(channel, data);
	fdSet(channel, data);
	return 0;

}

int selectDispatch(EventLoop* evLoop, int timeout)
{
	SelectData* data = (SelectData*)evLoop->dispatcherData;
	fd_set tempRead = data->readSet;
	fd_set tempWrite = data->writeSet;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	int ret = select(Max, &tempRead, &tempWrite, NULL, &tv);
	if (ret == -1) {
		perror("fail to start poll");
		exit(0);
	}
	for (int i = 0; i < Max + 1; i++)
	{
		if (FD_ISSET(i,&tempRead)) {
			//读操作
			eventActivate(evLoop, i, ReadEvent);
			
		}

		if (FD_ISSET(i,&tempWrite)) {
			//写操作
			eventActivate(evLoop, i, WriteEvent);
			
		}
	}

	return 0;
}

int selectClear(EventLoop* evLoop)
{
	SelectData* data = (SelectData*)evLoop->dispatcherData;
	free(data);
	return 0;
}
