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
	//http 请求方式
	char* method;
	//http 请求的url
	char* url;
	//http 的版本
	char* version;
	//http 请求的内容
	struct RequestHeader* reqHeaders;
	//http 请求内容的数量
	int reqHeaderNum;
	//当前处理状态
	enum HttpRequestState curState;
};
//初始化
struct HttpRequest* httpRequestInit();
//重置
void httpRequestReset(struct HttpRequest* request);

void httpRequestResetEx(struct HttpRequest* request);
//释放内存
void httpRequestDestory(struct HttpRequest* request);
//获取当前状态
enum HttpRequestState httpRequestState(struct HttpRequest* request);
//添加头
void httpRequestAddHeader(struct HttpRequest* request, char* key, char* value);
//获取头
char* httpRequestGetHeader(struct HttpRequest* request, char* key);
//解析请求行
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf);
