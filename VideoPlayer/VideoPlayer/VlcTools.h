#pragma once
#include <string>
#include "vlc.h"
struct VlcSize
{
	int nWidth;
	int nHeight;
};
class CVlcTools
{
public:
	CVlcTools();
	~CVlcTools();
	int SetMedia(std::string& strUrl); // utf-8±àÂë
	int SetHwnd(HWND hWnd);
	int Play();
	int Pause();
	int Stop();
	void SetPosition(float pos);
	float GetPosition();
	int SetVolume(int volume);
	int getVolume();
	VlcSize getMediaSize();
private:
	libvlc_instance_t* m_instance;
	libvlc_media_t* m_media;
	libvlc_media_player_t* m_player;
};

