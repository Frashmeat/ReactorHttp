#include "TcpConnection.h"

int processRead(void* arg) {
    struct TcpConnection* tcpCon = (struct TcpConnection*)arg;
    //��������
    int count = bufferSocketRead(tcpCon->readBuf, tcpCon->channel->fd);
    if (count > 0) {
        //���յ�������


    }
    else {
        //�Ͽ�����,�ͷ��ڴ�

    }
};

TcpConnection* tcpConnectInit(int fd, EventLoop* evLoop)
{
    struct TcpConnection* tcpCon = (struct TcpConnection*)malloc(sizeof(struct TcpConnection));
    tcpCon->evLoop = evLoop;

    tcpCon->readBuf = bufferInit(10240);
    tcpCon->writeBuf = bufferInit(10240);

    sprintf(tcpCon->name, "Connection-%d", fd);

    tcpCon->channel = channelInit(fd, ReadEvent, processRead, NULL, tcpCon);
    eventLoopAddTask(evLoop, tcpCon->channel, ADD);
    return tcpCon;
}
