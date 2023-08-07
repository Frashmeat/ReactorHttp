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
void bufferExtendRoom(struct Buffer* buffer, int size);
//获取可写内存容量
int bufferWriteableSize(struct Buffer* buffer);
//获取可读的内存容量
int bufferReadableSize(struct Buffer* buffer);
//写数据内存 1.直接写入 2.接收套接字的数据
int bufferAppendData(struct Buffer* buffer, const char* data, int size);
//写字符串到内存
int bufferAppendString(struct Buffer* buffer, const char* data);
//通过socket接收数据
int bufferSocketRead(struct Buffer* buffer, int fd);
//基于\r\n 读出一行
char* bufferFindCRLF(struct Buffer* buffer);
