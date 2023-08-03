#include "Channel.h"

Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
	Channel* channel = new Channel;
	channel->fd = fd;
	channel->events = events;
	channel->readCallBack = readFunc;
	channel->writeCallBack = writeFunc;
	channel->arg = arg;
	return channel;
}

void writeEventEnable(Channel* channle, bool flag)
{
	if (flag) {
		channle->events |= WriteEvent;
	}
	else {
		channle->events &= ~WriteEvent;
	}
}

bool isWriteEventEnable(Channel* channle)
{
	return channle->events & WriteEvent;
}
