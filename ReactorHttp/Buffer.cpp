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
    //初始化所有的数据为0
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
    //1.内存够用 -不需要扩容
    if (bufferWriteableSize(buffer) >= size) {
        return;
    }
    //2.内存不够,但是合并后够用 -不需要扩容
    else if (buffer->readPos + bufferWriteableSize(buffer) >= size) {
        int readable = bufferReadableSize(buffer);
        memcpy(buffer->data, buffer->data + buffer->readPos, readable);
        buffer->readPos = 0;
        buffer->writePos = readable;
    }
    //3.总大小不够用 -需要扩容
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
    //判断扩容 确保内存够用
    bufferExtendRoom(buffer, size);
    //拷贝data 到 buffer
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
    //初始化iovec 数组元素
    int writeable = bufferWriteableSize(buffer);
    vec[0].iov_base = buffer->data + buffer->writePos;
    vec[0].iov_len = writeable;
    char* tmpBuf = (char *)malloc(40960);
    vec[1].iov_base = tmpBuf;
    vec[1].iov_len = 40960;
    //读取到buffer 内存vec[0] -> buffer.data
    int ret = readv(fd, vec, 2);
    if (ret == -1) {
        //失败
        return -1;
    }
    if (ret <= writeable) {
        //buffer够用
        buffer->writePos += ret;
    }
    else {
        //buffer不够用
        buffer->writePos = buffer->capacity;
        bufferAppendData(buffer, tmpBuf, ret - writeable);
    }
    //释放 tmpBuf
    free(tmpBuf);
    return ret;
}
