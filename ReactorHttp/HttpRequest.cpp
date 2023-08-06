#include "HttpRequest.h"
#include "EventLoop.h"
#define HeaderSize 12


struct HttpRequest* httpRequestInit()
{
	struct HttpRequest* request = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
	httpRequestReset(request);
	request->reqHeaders = (struct RequestHeader*)malloc(sizeof(struct RequestHeader) * HeaderSize);
	return request;
}

void httpRequestReset(struct HttpRequest* request)
{
	request->curState = ParseReqLines;
	request->url = NULL;
	request->version = NULL;
	request->method = NULL;
	request->reqHeaderNum = 0;
}
