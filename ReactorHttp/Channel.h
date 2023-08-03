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
	//�ļ�������
	int fd;
	//�¼�
	int events;
	//�ص�����
	handleFunc readCallBack; //���ص�
	handleFunc writeCallBack; //д�ص�
	//�ص������Ĳ���
	void* arg;
};

//��ʼ�� channle 
Channel* channelInit(int fd,int events,handleFunc readFunc,handleFunc writeFunc,void* arg);
//����д�¼�
void writeEventEnable(Channel* channle, bool flag);
//��ȡд�¼��Ƿ񱻼���
bool isWriteEventEnable(Channel * channle);
