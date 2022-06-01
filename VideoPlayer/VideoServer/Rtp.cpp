#include "Rtp.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

CUDPSocket::CUDPSocket()
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
}

CUDPSocket::~CUDPSocket()
{
}

int CUDPSocket::Init(int port)
{
    this->nPort = port;
    struct sockaddr_in addr;
    memset(&addr, 0x0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        perror("udp bind error");
        close(fd);
        return -1;
    }
    return 0;
}
FILE* pFile = fopen("111.bin", "ab+");
int CUDPSocket::SendPacket(int nIp, short nPort, uint8_t* buf, int buflen)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(nPort);
    address.sin_addr.s_addr = nIp;

    int ret = sendto(fd, buf, buflen, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in));

    fwrite(buf, 1, buflen, pFile);
    return ret;
    
}

