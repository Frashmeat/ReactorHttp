#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher
{
	//初始化
	void* (*init)();
	//添加
	int (*add)(struct Channel* channel, struct EventLoop* evLoop);
	//删除
	int (*remove)(struct Channel* channel, struct EventLoop* evLoop);
	//修改
	int (*modify)(struct Channel* channel, struct EventLoop* evLoop);
	//事件监听
	int (*dispatch)(struct EventLoop* evLoop,int timeout);
	//清理
	int (*clear)(struct EventLoop * evLoop);
};

