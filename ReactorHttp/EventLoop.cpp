#include "EventLoop.h"


//�Ա���socket����д����
void taskWakeUp(struct EventLoop* evLoop) {
	const char* msg = "wake up";
	write(evLoop->socketPair[1], msg, strlen(msg));
}
//��ȡ����socket���͵�����
int readLocalMsg(void* arg) {
	struct EventLoop* evLoop = (EventLoop*)arg;
	char buf[256];
	read(evLoop->socketPair[1], buf, sizeof(buf));
	return 0;
}
struct EventLoop* eventLoopInit()
{
	return eventLoopInitEx(NULL);
}



struct EventLoop* eventLoopInitEx(const char* threadName)
{
	struct EventLoop* eventLoop = new EventLoop;
	eventLoop->isQuit = false;
	//�߳���Ϣ
	eventLoop->threadId = pthread_self();
	pthread_mutex_init(&eventLoop->mutex, NULL);
	strcpy(eventLoop->threadName, threadName == NULL ? "MainThread" : threadName);
	//dispatcher ��ʼ��
	eventLoop->dispatcher = &EpollDispatcher;
	eventLoop->dispatcherData = eventLoop->dispatcher->init();
	//�������� ��ʼ��
	eventLoop->head = eventLoop->tail = NULL;
	eventLoop->channelMap = ChannelMapInit(128);
	//socketpair�ĳ�ʼ�� AF_UNIX ��������һ�����ڱ���ͨ�ŵ�socket
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair);
	if (ret == -1) {
		perror("fail to create socketPair");
		exit(0);
	}
	//ָ������ socketPair[0] ���ڷ�����  socketPair[1] ���ڽ�������
	struct Channel* channel = channelInit(eventLoop->socketPair[1], ReadEvent,
		readLocalMsg, NULL, eventLoop);
	//��ӵ��������
	eventLoopAddTask(eventLoop, channel, ADD);
	return eventLoop;
}

int eventLoopRun(EventLoop* evLoop) {
	//����evLoop ������ΪNull
	assert(evLoop != NULL);

	struct Dispatcher* dispatcher = evLoop->dispatcher;
	if (evLoop->threadId != pthread_self()) {
		return -1;
	}
	while (!evLoop->isQuit) {
		//��ʼ����
		dispatcher->dispatch(evLoop, 2);
		eventLoopProcessTask(evLoop);
	}
	return 0;
}

int eventActivate(EventLoop* evLoop, int fd, int event)
{
	//ȷ��������Ч
	if (evLoop == NULL || fd < 0) {
		return -1;
	}
	//ȡ��channel
	struct Channel* channel = evLoop->channelMap->list[fd];
	//ȷ��channel��ȷ
	assert(channel->fd == fd);

	//���ûص�����
	if (event & ReadEvent && channel->readCallBack) {
		channel->readCallBack(channel->arg);
	}
	if (event & WriteEvent && channel->writeCallBack) {
		channel->writeCallBack(channel->arg);
	}

	return 0;
}


int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type) {
	//����
	pthread_mutex_lock(&evLoop->mutex);
	//�����½ڵ�
	//ChannelElement* newNode = new ChannelElement;
	struct ChannelElement* newNode = (struct ChannelElement*)malloc(sizeof(struct ChannelElement));
	newNode->channel = channel;
	newNode->type = type;
	newNode->next = NULL;
	//����������
	//����Ϊ��ʱ
	if (evLoop->head == evLoop->tail) {
		evLoop->head = evLoop->tail = newNode;
	}
	else {
		//����Ϊ��ʱ
		evLoop->tail->next = newNode;
		evLoop->tail = newNode;
	}
	pthread_mutex_unlock(&evLoop->mutex);

	//���Ϊ���߳��ڵ��� �����߳��Լ�����
	if (evLoop->threadId == pthread_self()) {
		//���߳�
		eventLoopProcessTask(evLoop);
	}
	else {
		//���߳�
		//��Ҫ�ñ��ص�socketͨ�� ���������̵߳� dispacher ��������״̬
		taskWakeUp(evLoop);

	}
	//���Ϊ���߳��ڵ��� �������̴߳���

	return 0;
};

int eventLoopTaskAdd(struct EventLoop* evLoop, struct Channel* channel) {

	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//�ж�fd�Ƿ����map��size ������ھ�����
	if (channelMap->size <= fd) {
		if (!makeMapRoom(channelMap, fd, sizeof(struct Channel*))) {
			return -1;
		}
	}

	//��channelװ��map�� ,ͬʱ��ӵ�dispatcher�ļ���������
	if (channelMap->list[fd] == NULL) {
		channelMap->list[fd] = channel;
		evLoop->dispatcher->add(channel, evLoop);
	}
	return 0;
}
int eventLoopTaskDelete(EventLoop* evLoop, Channel* channel)
{
	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//�ж�fd�Ƿ����map��size û�о�ֱ�ӷ���
	if (channelMap->size <= fd) {
		return -1;
	}
	int ret = evLoop->dispatcher->remove(channel, evLoop);
	return ret;
}
int eventLoopTaskModify(EventLoop* evLoop, Channel* channel)
{
	int fd = channel->fd;
	struct ChannelMap* channelMap = evLoop->channelMap;
	//�ж�fd�Ƿ����map��size ����list ��û�ж�Ӧ��channel��ֱ�ӷ���
	if (channelMap->size <= fd || channelMap->list[fd] == NULL) {
		return -1;
	}
	int ret = evLoop->dispatcher->modify(channel, evLoop);
	return ret;
}
int destoryChannel(EventLoop* evLoop, Channel* channel)
{
	//channelMap ��ɾ�� channel
	evLoop->channelMap->list[channel->fd] = NULL;
	//�ر�channel
	close(channel->fd);
	//�ͷ�channel��Դ
	free(channel);
	return 0;
}
int eventLoopProcessTask(struct EventLoop* evLoop) {
	pthread_mutex_lock(&evLoop->mutex);
	struct ChannelElement* head = evLoop->head;
	while (head != NULL) {
		struct Channel* channel = head->channel;
		if (head->type == ADD) {
			//��Ӳ���
			eventLoopTaskAdd(evLoop, channel);
		}
		else if (head->type = DELETE) {
			//ɾ������
			eventLoopTaskDelete(evLoop, channel);
			//�ͷ���Դ

		}
		else if (head->type == MODIFY) {
			//�޸Ĳ���
			eventLoopTaskModify(evLoop, channel);
		}
		ChannelElement* temp = head;
		head = head->next;
		free(head);
	}
	pthread_mutex_unlock(&evLoop->mutex);
	return 0;
};