#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using handleFunc = int (*)(void* arg);
enum FDEvent
{
	TimeOut = 0x01,
	ReadEvent = 0x02,
	WriteEvent = 0x04
};
struct Channel
{
	//文件描述符
	int fd;
	//事件
	int events;
	//回调函数
	handleFunc readCallBack; //读回调
	handleFunc writeCallBack; //写回调
	//回调函数的参数
	void* arg;
};

//初始化 channle 
Channel* channelInit(int fd,int events,handleFunc readFunc,handleFunc writeFunc,void* arg);
//允许写事件
void writeEventEnable(Channel* channle, bool flag);
//获取写事件是否被激活
bool isWriteEventEnable(Channel * channle);
