#include "Buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/uio.h>
Buffer* bufferInit(int size)
{
    struct Buffer* buffer = new struct Buffer;
    buffer->data = new char[size];
    buffer->capacity = size;
    buffer->readPos = buffer->writePos = 0;
    //��ʼ�����е�����Ϊ0
    memset(buffer->data, 0, size);
    return buffer;
}

void bufferDestory(Buffer* buffer)
{
    if (buffer != NULL) {
        if (buffer->data != NULL) {
            free(buffer->data);
        }
    }
    free(buffer);
}

void bufferExtendRoom(Buffer* buffer,int size)
{
    //1.�ڴ湻�� -����Ҫ����
    if (bufferWriteableSize(buffer) >= size) {
        return;
    }
    //2.�ڴ治��,���Ǻϲ����� -����Ҫ����
    else if (buffer->readPos + bufferWriteableSize(buffer) >= size) {
        int readable = bufferReadableSize(buffer);
        memcpy(buffer->data, buffer->data + buffer->readPos, readable);
        buffer->readPos = 0;
        buffer->writePos = readable;
    }
    //3.�ܴ�С������ -��Ҫ����
    else {
        void* temp = realloc(buffer->data, buffer->capacity + size);
        if (temp == NULL) {
            return;
        }
        memset(temp + buffer->capacity, 0, size);
        buffer->data = (char *)temp;
        buffer->capacity = buffer->capacity + size;
    }
}

int bufferWriteableSize(Buffer* buffer)
{
    return buffer->capacity - buffer->writePos;
}

int bufferReadableSize(Buffer* buffer)
{
    return buffer->writePos - buffer->readPos;
}

int bufferAppendData(struct Buffer* buffer, const char* data, int size)
{
    if (buffer == NULL || data == NULL || size <= 0) {
        return -1;
    }
    //�ж����� ȷ���ڴ湻��
    bufferExtendRoom(buffer, size);
    //����data �� buffer
    memcpy(buffer + buffer->writePos,data,size);
    buffer->writePos += size;
    return 0;
}

int bufferAppendString(struct Buffer* buffer, const char* data)
{
    int len = strlen(data);
    return bufferAppendData(buffer, data, len);
}

int bufferSocketRead(struct Buffer* buffer, int fd)
{ 
    struct iovec vec[2];
    //��ʼ��iovec ����Ԫ��
    int writeable = bufferWriteableSize(buffer);
    vec[0].iov_base = buffer->data + buffer->writePos;
    vec[0].iov_len = writeable;
    char* tmpBuf = (char *)malloc(40960);
    vec[1].iov_base = tmpBuf;
    vec[1].iov_len = 40960;
    //��ȡ��buffer �ڴ�vec[0] -> buffer.data
    int ret = readv(fd, vec, 2);
    if (ret == -1) {
        //ʧ��
        return -1;
    }
    if (ret <= writeable) {
        //buffer����
        buffer->writePos += ret;
    }
    else {
        //buffer������
        buffer->writePos = buffer->capacity;
        bufferAppendData(buffer, tmpBuf, ret - writeable);
    }
    //�ͷ� tmpBuf
    free(tmpBuf);
    return ret;
}
