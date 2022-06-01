#pragma once
#include <stdint.h>
#include <stdlib.h>

#define RTP_HEADER_SIZE         12
#define RTP_MAX_PKT_SIZE        1300



class CUDPSocket
{
public:
    int nPort;
    int fd;
    CUDPSocket();
    ~CUDPSocket();
    int Init(int port);
    int SendPacket(int nIp, short nPort, uint8_t* buf, int buflen);
};



