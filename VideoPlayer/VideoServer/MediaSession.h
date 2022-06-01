#pragma once
#include <string>
#include "Rtp.h"
#include "MediaSource.h"

struct RtpHeader
{
    RtpHeader()
    {
        version = 2;
        payloadType = 96;
        csrcLen = 0;
        extension = 0;
        padding = 0;
        marker = 0;
        seq = 0;
        timestamp = 0;
        ssrc = 0x23349288;
    }
    /* byte 0 */
    uint8_t csrcLen : 4;
    uint8_t extension : 1;
    uint8_t padding : 1;
    uint8_t version : 2;

    /* byte 1 */
    uint8_t payloadType : 7;
    uint8_t marker : 1;

    /* bytes 2,3 */
    uint16_t seq;

    /* bytes 4-7 */
    uint32_t timestamp;

    /* bytes 8-11 */
    uint32_t ssrc;
};

class CMediaSession
{
public:
	short m_nRtpPort;
	int m_nIp;
	std::string m_Sdp;
	uint32_t m_sessionId;
    RtpHeader header;
    Frame data;
    uint32_t timestamp;
    uint16_t sn;
    bool isAlive;
    bool isClose;
public:
    CMediaSession();
	static std::string getSdp();
	int getSessionId();

	void SendFrame();
	static void* SendFrameEntry(void* arg);
};

