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

void httpRequestResetEx(struct HttpRequest* request)
{
	free(request->method);
	free(request->version);
	free(request->url);
	httpRequestReset(request);

}

void httpRequestDestory(struct HttpRequest* request)
{
	if (request != NULL) {
		httpRequestResetEx(request);
		if (request->reqHeaders != NULL) {
			for (int i = 0; i < request->reqHeaderNum; i++)
			{
				free(request->reqHeaders[i].key);
				free(request->reqHeaders[i].value);
			}
			free(request->reqHeaders);
		}
		free(request);
	}
}

HttpRequestState httpRequestState(HttpRequest* request)
{
	return request->curState;
}

void httpRequestAddHeader(HttpRequest* request, char* key, char* value)
{
	if (request->reqHeaders != NULL) {
		request->reqHeaders[request->reqHeaderNum].key = key;
		request->reqHeaders[request->reqHeaderNum].value = value;
		request->reqHeaderNum++;
	}
}

char* httpRequestGetHeader(HttpRequest* request, char* key)
{
	if (request->reqHeaders != NULL) {
		for (int i = 0; i < request->reqHeaderNum; i++)
		{
			if (strncasecmp(request->reqHeaders[i].key, key, strlen(request->reqHeaders[i].key)) == 0) {
				return request->reqHeaders[i].value;
			}
		}
	}
	return NULL;
}

bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf)
{
	//读出请求行
	//保存字符串结束位置
	char* end = bufferFindCRLF(readBuf);
	//保存字符串起始位置
	char* start = readBuf->data + readBuf->readPos;
	//请求行的总长度
	int lineSize = end - start;

	if (lineSize) {
		//get /XXX/XX.txt http/1.1
		//请求 
		char* space = (char*)memmem(start, lineSize, " ", 1);
		assert(space != NULL);
		//方法字符的长度
		int methodSize = space - start;
		//将方法字符copy到request中
		request->method = (char*)malloc(methodSize + 1);
		strncpy(request->method, start, methodSize);
		request->method[methodSize] = '\0';

		start = space + 1;
		space = (char*)memmem(start, lineSize, " ", 1);
		assert(space != NULL);
		//方法字符的长度
		int urlSize = space - start;
		//将方法字符copy到request中
		request->url = (char*)malloc(urlSize + 1);
		strncpy(request->url, start, urlSize);
		request->url[urlSize] = '\0';

		start = space + 1;
		request->version = (char*)malloc(end-start);
		strncpy(request->version, start, end-start);
		request->version[end-start] = '\0';
		readBuf->readPos += lineSize;
		readBuf->readPos += 2;

		return true;
	}

	return false;
}
