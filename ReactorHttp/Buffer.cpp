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
