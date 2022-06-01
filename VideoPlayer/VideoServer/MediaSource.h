#pragma once
#include <cstdint>
#include <memory>

struct Frame
{
	Frame(uint32_t size = 0)
		:buffer(new uint8_t[size + 1], std::default_delete< uint8_t[]>())
	{
		this->size = size;
		type = 0;
		timestamp = 0;
	}

	std::shared_ptr<uint8_t> buffer; /* 帧数据 */
	uint32_t size;				     /* 帧大小 */
	uint8_t  type;				     /* 帧类型 */
	uint32_t timestamp;		  	     /* 时间戳 */
};

class CMediaSource
{
public:
	FILE* fd;
	long filesize;
public:
	void OpenFile();
	Frame GetOneFrame();
	long GetH264Header(bool isEnd);
	void reset();
	~CMediaSource();
};

