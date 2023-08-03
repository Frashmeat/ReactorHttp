#pragma once
struct Buffer
{
	//������
	char* data;
	//����
	int capacity;
	//��ָ��
	int readPos;
	//дָ��
	int writePos;
};
//��ʼ��
struct Buffer* bufferInit(int size);
//����buffer
void bufferDestory(struct Buffer* buffer);
//buffer������
void bufferExtendRoom(struct Buffer* buffer, int size);
//��ȡ��д�ڴ�����
int bufferWriteableSize(struct Buffer* buffer);
//��ȡ�ɶ����ڴ�����
int bufferReadableSize(struct Buffer* buffer);
//д�ڴ� 1.ֱ��д�� 2.�����׽��ֵ�����
int bufferAppendData(struct Buffer* buffer, const char* data, int size);
int bufferAppendString(struct Buffer* buffer, const char* data);
int bufferSocketRead(struct Buffer* buffer, int fd);

