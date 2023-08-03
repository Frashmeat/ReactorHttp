#pragma once
#include "Channel.h"
struct ChannelMap
{
	int size;
	//channle 容器
	Channel** list;
};
//初始化
ChannelMap* ChannelMapInit(int size);
//清空容器
void ChannelMapClear(ChannelMap* map);
//重新分配空间
bool makeMapRoom(ChannelMap* map, int newSize, int unitSize);
