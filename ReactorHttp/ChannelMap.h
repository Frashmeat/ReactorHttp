#pragma once
#include "Channel.h"
struct ChannelMap
{
	int size;
	//channle ����
	Channel** list;
};
//��ʼ��
ChannelMap* ChannelMapInit(int size);
//�������
void ChannelMapClear(ChannelMap* map);
//���·���ռ�
bool makeMapRoom(ChannelMap* map, int newSize, int unitSize);
