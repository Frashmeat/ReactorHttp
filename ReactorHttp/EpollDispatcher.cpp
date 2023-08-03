#include "Dispatcher.h"
#include <sys/epoll.h>


#define Max 520
struct EpollData
{
	int epfd;
	epoll_event* events;
};

//��ʼ��
static void* epollInit();
//���
static int epollAdd(Channel* channel, EventLoop* evLoop);
//ɾ��
static int epollRemove(Channel* channel, EventLoop* evLoop);
//�޸�
static int epollModify(Channel* channel, EventLoop* evLoop);
//�¼�����
static int epollDispatch(EventLoop* evLoop,int timeout);
//����
static int epollClear(EventLoop* evLoop);
//ctl
static int epollCtl(Channel* channel, EventLoop* evLoop, int _op);

Dispatcher EpollDispatcher = {
	epollInit,
	epollAdd,
	epollRemove,
	epollModify,
	epollDispatch,
	epollClear
};


int epollCtl(Channel* channel, EventLoop* evLoop, int _op)
{
	EpollData* data = (EpollData*)evLoop->dispatcherData;
	epoll_event ev;
	ev.data.fd = channel->fd;
	int events = 0;
	if (channel->events & ReadEvent) {
		events |= EPOLLIN;
	}
	if (channel->events & WriteEvent) {
		events |= EPOLLOUT;
	}
	ev.events = events;
	int ret = epoll_ctl(data->epfd, _op, channel->fd, &ev);
	return ret;
}

void* epollInit()
{
	//EpollData* data = new EpollData;
	EpollData* data = (struct EpollData*)malloc(sizeof (struct EpollData));
	data->epfd = epoll_create(10);
	if (data->epfd == -1) {
		perror("fail to init epoll");
		exit(0);
	}
	//data->events = new epoll_event[Max];
	data->events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*Max);
	return data;
}

int epollAdd(Channel* channel, EventLoop* evLoop)
{
	
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
	if (ret == -1) {
		perror("fail to add epoll");
		exit(0);
	}
	return ret;
}

int epollRemove(Channel* channel, EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
	if (ret == -1) {
		perror("fail to delete epoll");
		exit(0);
	}
	return ret;
}

int epollModify(Channel* channel, EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
	if (ret == -1) {
		perror("fail to modify epoll");
		exit(0);
	}
	return ret;
}

int epollDispatch(EventLoop* evLoop,int timeout)
{
	EpollData* data = (EpollData*)evLoop->dispatcherData;
	int count = epoll_wait(data->epfd, data->events, Max, timeout * 1000);
	for (int i = 0; i < count; i++)
	{
		int event = data->events[i].events;
		int fd = data->events[i].data.fd;
		if (event & EPOLLERR || event & EPOLLHUP) {
			//ɾ����Ӧ�Ľڵ�

			continue;
		}
		if (event & EPOLLIN) {
			//������
			eventActivate(evLoop, fd, ReadEvent);

		}
		if (event & EPOLLOUT) {
			//д����
			eventActivate(evLoop, fd, WriteEvent);
		}
	}

	return 0;
}

int epollClear(EventLoop* evLoop)
{
	EpollData* data = (EpollData*)evLoop->dispatcherData;
	free(data->events);
	close(data->epfd);
	free(data);
	return 0;
}
