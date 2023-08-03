#include "Channel.h"

struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
	Channel* channel = (Channel*)malloc(sizeof (struct Channel));
	channel->fd = fd;
	channel->events = events;
	channel->readCallBack = readFunc;
	channel->writeCallBack = writeFunc;
	channel->arg = arg;
	return channel;
}

void writeEventEnable(struct Channel* channle, bool flag)
{
	if (flag) {
		channle->events |= WriteEvent;
	}
	else {
		channle->events &= ~WriteEvent;
	}
}

bool isWriteEventEnable(struct Channel* channle)
{
	return channle->events & WriteEvent;
}
