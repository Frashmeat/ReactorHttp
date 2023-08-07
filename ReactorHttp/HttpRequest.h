#pragma once
#include "EventLoop.h"
#include "Buffer.h"
struct RequestHeader
{
	char* key;
	char* value;
};
enum HttpRequestState
{
	ParseReqLines,
	ParseReqHeaders,
	ParseReqBody,
	ParseReqDone
};

struct HttpRequest
{
	//http ����ʽ
	char* method;
	//http �����url
	char* url;
	//http �İ汾
	char* version;
	//http ���������
	struct RequestHeader* reqHeaders;
	//http �������ݵ�����
	int reqHeaderNum;
	//��ǰ����״̬
	enum HttpRequestState curState;
};
//��ʼ��
struct HttpRequest* httpRequestInit();
//����
void httpRequestReset(struct HttpRequest* request);

void httpRequestResetEx(struct HttpRequest* request);
//�ͷ��ڴ�
void httpRequestDestory(struct HttpRequest* request);
//��ȡ��ǰ״̬
enum HttpRequestState httpRequestState(struct HttpRequest* request);
//���ͷ
void httpRequestAddHeader(struct HttpRequest* request, char* key, char* value);
//��ȡͷ
char* httpRequestGetHeader(struct HttpRequest* request, char* key);
//����������
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf);
