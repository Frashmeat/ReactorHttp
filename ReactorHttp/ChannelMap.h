#pragma once
#include "Channel.h"
struct ChannelMap
{
	int size;
	//channle ����
	Channel** list;
};
//��ʼ��
struct ChannelMap* ChannelMapInit(int size);
//�������
void ChannelMapClear(struct ChannelMap* map);
//���·���ռ�
bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize);
