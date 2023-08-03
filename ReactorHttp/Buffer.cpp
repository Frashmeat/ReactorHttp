#include "Buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
