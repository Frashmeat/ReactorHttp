#include "Dispatcher.h"
#include <sys/select.h>


#define Max 1024
struct SelectData
{
	fd_set readSet;
	fd_set writeSet;
};

//��ʼ��
static void* selectInit();
//���
static int selectAdd(Channel* channel, EventLoop* evLoop);
//ɾ��	  
static int selectRemove(Channel* channel, EventLoop* evLoop);
//�޸�	  
static int selectModify(Channel* channel, EventLoop* evLoop);
//�¼�����	
static int selectDispatch(EventLoop* evLoop, int timeout);
//����	  
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
			//������
			eventActivate(evLoop, i, ReadEvent);
			
		}

		if (FD_ISSET(i,&tempWrite)) {
			//д����
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
