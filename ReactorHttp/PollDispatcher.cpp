#include <poll.h>
#include "Dispatcher.h"


#define Max 1024
struct PollData
{
	int maxfd;
	pollfd fds[Max];
};

//��ʼ��
static void* pollInit();
//���
static int pollAdd(Channel* channel, EventLoop* evLoop);
//ɾ��
static int pollRemove(Channel* channel, EventLoop* evLoop);
//�޸�
static int pollModify(Channel* channel, EventLoop* evLoop);
//�¼�����
static int pollDispatch(EventLoop* evLoop,int timeout);
//����
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
			//������
			eventActivate(evLoop, data->fds[i].fd, ReadEvent);

		}

		else if (data->fds[i].revents & POLLOUT) {
			//д����
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
