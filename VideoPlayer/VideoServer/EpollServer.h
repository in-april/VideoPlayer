#pragma once

#define EVENT_SIZE 1024
#define BUFFER_SIZE 4096

class CEpollServer;
typedef void (CEpollServer::*CallBackFunc)(void* arg);

//�¼��б���Ľṹ��
typedef struct call_back_info
{
	int fd;
	int events;
	CallBackFunc call_back;
	char buff[BUFFER_SIZE];
	int bufflen;
} CallBackInfo;

class CEpollServer
{
public:
	CEpollServer();
	int Start(short nPort);
	~CEpollServer();


	void AddEvent(int fd, int events, CallBackFunc callBack, CallBackInfo* info);
	void DeleteEvent(CallBackInfo* info);
	void UpdateEvent(int events, CallBackFunc callBack, CallBackInfo* info);

	//�ص�����
	void InitAccept(void* arg);
	void ReadData(void* arg);
	void WriteData(void* arg);

private:
	int lfd;
	int epollfd;
	CallBackInfo callBackInfo[EVENT_SIZE];
};

