#include "MediaSession.h"
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
extern CUDPSocket rtp;

CMediaSession::CMediaSession():m_sessionId(rand()), 
        timestamp(0), isAlive(false), isClose(false),
        sn(0)
{
}

std::string CMediaSession::getSdp()
{

    std::string ip = "127.0.0.1";
    char buf[2048] = { 0 };

    snprintf(buf, sizeof(buf),
        "v=0\r\n"
        "o=- 9%ld 1 IN IP4 %s\r\n"
        "t=0 0\r\n"
        "a=control:*\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=framerate:24\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=control:track0\r\n",
        (long)time(NULL), ip.c_str());

    return buf;
}

int CMediaSession::getSessionId()
{
    return m_sessionId;
}

void CMediaSession::SendFrame()
{
    CMediaSource* pSource = new CMediaSource();
    pSource->OpenFile();
    Frame frame = pSource->GetOneFrame();
    //创建头
    
    while (frame.size != 0)
    {
        RtpHeader header;
        std::string sendData;
        int framePos = 0;
        header.timestamp = htonl(timestamp);
        timestamp += 90000 / 25;
        if (frame.size > RTP_MAX_PKT_SIZE) //分片
        {
            int count = frame.size / 1300;
            if (frame.size % RTP_MAX_PKT_SIZE) count++;
            for (int i = 1; i <= count; i++)
            {
                int dataPos = 0;
                
                uint8_t nal[2];
                nal[0] = (*frame.buffer & 0x60) | 28;
                
                header.seq = htons(sn++);

                if (i == 1) //第一片
                {
                    nal[1] = *frame.buffer & 0x1F; //帧数据一个个字节的后5bit
                    nal[1] |= 0x80;
                    sendData.resize(sizeof(header) + 2 + RTP_MAX_PKT_SIZE);
                    memcpy((void*)sendData.c_str(), &header, sizeof(header)); dataPos += sizeof(header);
                    memcpy((void*)sendData.c_str() + dataPos, &nal, 2); dataPos += 2; framePos += 1;
                    memcpy((void*)sendData.c_str() + dataPos, frame.buffer.get() + framePos, RTP_MAX_PKT_SIZE); framePos += RTP_MAX_PKT_SIZE;
                }
                else if (i == count) //最后一片
                {
                    nal[1] = *frame.buffer & 0x1F; //帧数据一个个字节的后5bit
                    nal[1] |= 0x40;
                    sendData.resize(sizeof(header) + 2 + frame.size % RTP_MAX_PKT_SIZE);
                    memcpy((void*)sendData.c_str(), &header, sizeof(header)); dataPos += sizeof(header);
                    memcpy((void*)sendData.c_str() + dataPos, &nal, 2); dataPos += 2;
                    memcpy((void*)sendData.c_str() + dataPos, frame.buffer.get() + framePos, frame.size % RTP_MAX_PKT_SIZE - 1); framePos += RTP_MAX_PKT_SIZE;
                }
                else //中间
                {
                    nal[1] = *frame.buffer & 0x1F; //帧数据一个个字节的后5bit
                    nal[1] |= 0x00;
                    sendData.resize(sizeof(header) + 2 + RTP_MAX_PKT_SIZE);
                    memcpy((void*)sendData.c_str(), &header, sizeof(header)); dataPos += sizeof(header);
                    memcpy((void*)sendData.c_str() + dataPos, &nal, 2); dataPos += 2;
                    memcpy((void*)sendData.c_str() + dataPos, frame.buffer.get() + framePos, RTP_MAX_PKT_SIZE); framePos += RTP_MAX_PKT_SIZE;
                }

                //发送
                rtp.SendPacket(m_nIp, m_nRtpPort, (uint8_t*)sendData.c_str(), sendData.size());
                sendData.clear();
            }


            int pktNum = frame.size / RTP_MAX_PKT_SIZE;       // 有几个完整的包
            int remainPktSize = frame.size % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
            int i, pos = 1;
        }
        else //不分片
        {
            sendData.resize(sizeof(header) + frame.size);
            int pos = 0;
            header.seq = htons(sn++);
            memcpy((void*)sendData.c_str(), &header, sizeof(header)); pos += sizeof(header);
            memcpy((void*)sendData.c_str() + pos, frame.buffer.get(), frame.size);

            //发送
            rtp.SendPacket(m_nIp, m_nRtpPort, (uint8_t*)sendData.c_str(), sendData.size());
            
        }
        usleep(1000 * 1000 / 25);
        frame = pSource->GetOneFrame();
        //检测是否退出
        if (isClose) return;
    }
    

    delete pSource;
}

void* CMediaSession::SendFrameEntry(void* arg)
{
    sleep(1);
    CMediaSession* thiz = (CMediaSession*)arg;
    thiz->SendFrame();
    return nullptr;
}
