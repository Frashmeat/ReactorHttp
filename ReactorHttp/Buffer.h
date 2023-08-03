#pragma once
struct Buffer
{
	//数据组
	char* data;
	//容量
	int capacity;
	//读指针
	int readPos;
	//写指针
	int writePos;
};
//初始化
struct Buffer* bufferInit(int size);
//销毁buffer
void bufferDestory(struct Buffer* buffer);
//buffer的扩容
void bufferExtendRoom(struct Buffer* buffer,int size);
//获取可写内存容量
int bufferWriteableSize(struct Buffer* buffer);
//获取可读的内存容量
int bufferReadableSize(struct Buffer* buffer);

