#include <poll.h>
#include "Dispatcher.h"


#define Max 1024
struct PollData
{
	int maxfd;
	pollfd fds[Max];
};

//初始化
static void* pollInit();
//添加
static int pollAdd(Channel* channel, EventLoop* evLoop);
//删除
static int pollRemove(Channel* channel, EventLoop* evLoop);
//修改
static int pollModify(Channel* channel, EventLoop* evLoop);
//事件监听
static int pollDispatch(EventLoop* evLoop,int timeout);
//清理
static int pollClear(EventLoop* evLoop);

Dispatcher PollDispatcher = {
	pollInit,
	pollAdd,
	pollRemove,
	pollModify,
	pollDispatch,
	pollClear
};




void* pollInit()
{
	PollData* data = new PollData;
	data->maxfd = 0;
	for (int i = 0; i < Max; i++)
	{
		data->fds[i].fd = -1;
		data->fds[i].events = 0;
		data->fds[i].revents = 0;
	}
	return data;
}

int pollAdd(Channel* channel, EventLoop* evLoop)
{

	PollData* data = (PollData*)evLoop->dispatcherData;
	int events = 0;
	if (channel->events & ReadEvent) {
		events |= POLLIN;
	}
	if (channel->events & WriteEvent) {
		events |= POLLOUT;
	}
	int i = 0;
	for (; i < Max; i++)
	{
		if (data->fds[i].fd == -1) {
			data->fds[i].fd = channel->fd;
			data->fds[i].events = events;
			data->maxfd = i > data->maxfd ? i : data->maxfd;
			break;
		}
	}
	if (i >= Max) {
		return -1;
	}

	return 0;
}

int pollRemove(Channel* channel, EventLoop* evLoop)
{
	PollData* data = (PollData*)evLoop->dispatcherData;
	int events = 0;
	int i = 0;
	for (; i < Max; i++)
	{
		if (data->fds[i].fd == channel->fd) {
			data->fds[i].fd = -1;
			data->fds[i].events = 0;
			data->fds[i].revents = 0;
			break;
		}
	}
	if (i >= Max) {
		return -1;
	}

	return 0;
}

int pollModify(Channel* channel, EventLoop* evLoop)
{
	PollData* data = (PollData*)evLoop->dispatcherData;
	int events = 0;
	if (channel->events & ReadEvent) {
		events |= POLLIN;
	}
	if (channel->events & WriteEvent) {
		events |= POLLOUT;
	}
	int i = 0;
	for (; i < Max; i++)
	{
		if (data->fds[i].fd == channel->fd) {
			data->fds[i].events = events;
			break;
		}
	}
	if (i >= Max) {
		return -1;
	}

	return 0;

}

int pollDispatch(EventLoop* evLoop, int timeout)
{
	PollData* data = (PollData*)evLoop->dispatcherData;
	int ret = poll(data->fds, data->maxfd + 1, timeout * 1000);
	if (ret == -1) {
		perror("fail to start poll");
		exit(0);
	}
	for (int i = 0; i < data->maxfd+1; i++)
	{
		if (data->fds[i].fd == -1) {
			continue;
		}

		if (data->fds[i].revents & POLLIN) {
			//读操作
			eventActivate(evLoop, data->fds[i].fd, ReadEvent);

		}

		else if (data->fds[i].revents & POLLOUT) {
			//写操作
			eventActivate(evLoop, data->fds[i].fd, WriteEvent);
		}
	}

	return 0;
}

int pollClear(EventLoop* evLoop)
{
	PollData* data = (PollData*)evLoop->dispatcherData;
	free(data);
	return 0;
}
