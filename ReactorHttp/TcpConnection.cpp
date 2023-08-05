#include "TcpConnection.h"

int processRead(void* arg) {
    struct TcpConnection* tcpCon = (struct TcpConnection*)arg;
    //接收数据
    int count = bufferSocketRead(tcpCon->readBuf, tcpCon->channel->fd);
    if (count > 0) {
        //接收到了数据


    }
    else {
        //断开连接,释放内存

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
