#include "EpollServer.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include "ParserRTSP.h"
#include "MediaSession.h"
#include <map>
#include <pthread.h>

std::map<int, CMediaSession> g_sessions;


CEpollServer::CEpollServer()
{
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket error");
	}
	//设置端口复用
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//初始化CallBackInfo
	for (int i = 0; i < EVENT_SIZE; i++)
	{
		callBackInfo[i].fd = -1;
	}
}

int CEpollServer::Start(short nPort)
{
	sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(lfd, (sockaddr*)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("bind error");
	}
	if (listen(lfd, 10) == -1)
	{
		perror(" listen error");
	}
	epollfd = epoll_create(1024);
	if (epollfd == -1) {
		perror("epoll_create error");
	}
	epoll_event events[1024];
	AddEvent(lfd, EPOLLIN | EPOLLET, &CEpollServer::InitAccept, &callBackInfo[EVENT_SIZE - 1]);
	while (true)
	{
		int nReady = epoll_wait(epollfd, events, 1024, -1);
		for (int i = 0; i < nReady; i++)
		{
			CallBackInfo* info = (CallBackInfo*)events[i].data.ptr;
			(this->*(info->call_back))(&events[i]);
		}
	}
}

CEpollServer::~CEpollServer()
{
}

void CEpollServer::AddEvent(int fd, int events, CallBackFunc callBack, CallBackInfo* info)
{
	info->fd = fd;
	info->events = events;
	info->call_back = callBack;

	epoll_event ev;
	ev.events = events;
	ev.data.ptr = info;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void CEpollServer::DeleteEvent(CallBackInfo* info)
{

	epoll_event tmp;
	tmp.events = info->events;
	tmp.data.ptr = NULL;

	epoll_ctl(epollfd, EPOLL_CTL_DEL, info->fd, &tmp);

	memset(info, 0, sizeof(info));
	info->fd = -1;
	
}

void CEpollServer::UpdateEvent(int events, CallBackFunc callBack, CallBackInfo* info)
{
	info->events = events;
	info->call_back = callBack;

	epoll_event tmp;
	tmp.events = info->events;
	tmp.data.ptr = info;

	epoll_ctl(epollfd, EPOLL_CTL_DEL, info->fd, &tmp);
}

void CEpollServer::InitAccept(void* arg)
{
	int i;
	epoll_event* events = (epoll_event*)arg;
	CallBackInfo* info = (CallBackInfo*)events->data.ptr;
	int cfd = accept(info->fd, NULL, NULL);
	//寻找一个空节点
	for (i = 0; i < EVENT_SIZE; i++)
	{
		if (callBackInfo[i].fd == -1)
		{
			break;
		}
	}
	//设置端口复用
	int opt = 1;
	setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	AddEvent(cfd, EPOLLIN | EPOLLET, &CEpollServer::ReadData, &callBackInfo[i]);

}

void CEpollServer::ReadData(void* arg)
{
	epoll_event* events = (epoll_event*)arg;
	CallBackInfo* info = (CallBackInfo*)events->data.ptr;
	info->bufflen = read(info->fd, info->buff, sizeof(info->buff));
	std::cout << info->buff;
	if (info->bufflen < 0)
	{
		//错误处理
	} 
	else if (info->bufflen == 0)
	{
		//对方关闭套接字
		DeleteEvent(info);
		close(info->fd);
	}
	else
	{
		//处理数据

		RtspParser resp(info->buff);
		Method method = NONE;
		if (!resp["method"].compare("OPTIONS")) method = OPTIONS;
		else if (!resp["method"].compare("DESCRIBE")) method = DESCRIBE;
		else if (!resp["method"].compare("SETUP")) method = SETUP;
		else if (!resp["method"].compare("PLAY")) method = PLAY;
		else if (!resp["method"].compare("TEARDOWN")) method = TEARDOWN;
		else if (!resp["method"].compare("GET_PARAMETER")) method = GET_PARAMETER;

		
		switch (method)
		{
		case OPTIONS:
		{
			uint mCSeq = atoi(resp["cseq"].c_str());
			memset((void*)info->buff, 0, info->bufflen);
			snprintf(info->buff, sizeof(info->buff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %u\r\n"
				"Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
				"\r\n", mCSeq);
			info->bufflen = strlen(info->buff);
			write(info->fd, info->buff, info->bufflen);
			break;
		}
		case DESCRIBE:
		{
			std::string sdp = CMediaSession::getSdp();
			uint mCSeq = atoi(resp["cseq"].c_str());	
			memset((void*)info->buff, 0, info->bufflen);
			snprintf(info->buff, sizeof(info->buff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %u\r\n"
				"Content-Length: %u\r\n"
				"Content-Type: application/sdp\r\n"
				"\r\n"
				"%s",
				mCSeq,
				(unsigned int)sdp.size(),
				sdp.c_str());
			info->bufflen = strlen(info->buff);
			write(info->fd, info->buff, info->bufflen);
			break;
		}
		case SETUP:
		{
			short rtpPort, rtcpPort;
			uint mCSeq = atoi(resp["cseq"].c_str());
			int pos = resp["transport"].find("client_port");
			sscanf(resp["transport"].c_str() + pos, "%*[^=]=%hu-%hu",
				&rtpPort, &rtcpPort);

			memset((void*)info->buff, 0, info->bufflen);
			//创建session
			CMediaSession session = CMediaSession();
			struct sockaddr_in sa;
			int len = sizeof(sa);
			if (!getpeername(info->fd, (struct sockaddr*)&sa, (socklen_t*)&len))
			{
				session.m_nIp = sa.sin_addr.s_addr;
			}
			session.m_nRtpPort = rtpPort;
			g_sessions.insert(std::pair<int, CMediaSession>(session.getSessionId(), session));
			snprintf(info->buff, sizeof(info->buff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %u\r\n"
				"Transport: RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu\r\n"
				"Session: %08x\r\n"
				"\r\n",
				mCSeq,
				rtpPort,
				rtcpPort,
				50000,
				50001,
				session.getSessionId());
			info->bufflen = strlen(info->buff);
			write(info->fd, info->buff, info->bufflen);
			break;
		}
		case PLAY:
		{
			uint sessionId = strtol(resp["session"].c_str(),nullptr,16);
			//启动发送线程
			auto it = g_sessions.find(sessionId);
			CMediaSession& session = it->second;
			pthread_t id;
			//创建函数线程，并且指定函数线程要执行的函数
			if (session.isAlive == false)
			{
				int res = pthread_create(&id, NULL, &CMediaSession::SendFrameEntry, &session);
				session.isAlive = true;
			}
			
			uint mCSeq = atoi(resp["cseq"].c_str());
			memset((void*)info->buff, 0, info->bufflen);
			snprintf(info->buff, sizeof(info->buff),
				"RTSP/1.0 200 OK\r\n"
				"CSeq: %d\r\n"
				"Range: npt=0.000-\r\n"
				"Session: %08x; timeout=60\r\n"
				"\r\n",
				mCSeq,
				session.getSessionId());
			info->bufflen = strlen(info->buff);
			write(info->fd, info->buff, info->bufflen);
			break;
		}
		case TEARDOWN:
		{
			DeleteEvent(info);
			uint sessionId = strtol(resp["session"].c_str(), nullptr, 16);
			auto it = g_sessions.find(sessionId);
			sleep(3); //待优化
			it->second.isClose = true;
			
			g_sessions.erase(sessionId);
			break;
		}
		case GET_PARAMETER:
			break;
		case RTCP:
			break;
		case NONE:
			break;
		default:
			break;
		}
	}
}

void CEpollServer::WriteData(void* arg)
{
}
