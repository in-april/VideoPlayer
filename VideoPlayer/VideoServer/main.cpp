#include "EpollServer.h"
#include "Rtp.h"

CUDPSocket rtp;
CUDPSocket rtcp;

int main()
{
    rtp.Init(50000);
    rtcp.Init(50001);
    CEpollServer* server = new CEpollServer();
    server->Start(554);
}