#include "MediaSource.h"

void CMediaSource::OpenFile()
{
	fd = fopen("/home/output1.h264", "rb");
	fseek(fd, 0, SEEK_END);
	filesize = ftell(fd);
	fseek(fd, 0, SEEK_SET);
}

long CMediaSource::GetH264Header(bool isEnd=false)
{
	uint32_t flag1 = 0x01000000;
	uint32_t flag2 = 0x010000;
	uint32_t tmp = 0;
	long ret = -1;

	while (filesize - ftell(fd) >= 4)
	{
		fread(&tmp, 1, 4, fd);
		if (tmp == flag1)
		{
			ret = ftell(fd);
			if (isEnd) ret -= 4;
			break;
		}	
		if ((tmp & 0x3f) == flag2)
		{
			ret = ftell(fd);
			if (isEnd) ret -= 3;
			break;
		}
		fseek(fd, ftell(fd) - 3, SEEK_SET);
	}
	return ret;
}

void CMediaSource::reset()
{
	fseek(fd, 0, SEEK_SET);
}

CMediaSource::~CMediaSource()
{
	fclose(fd);
}

Frame CMediaSource::GetOneFrame()
{
	long head = GetH264Header();
	if (head == -1) return Frame();
	long end = GetH264Header(true);
	if (end == -1) 
		end = filesize;
	fseek(fd, head, SEEK_SET);
	Frame frame(end - head);
	fread(frame.buffer.get(), 1, frame.size, fd);
	return frame;
}
