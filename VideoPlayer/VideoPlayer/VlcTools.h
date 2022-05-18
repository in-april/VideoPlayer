#pragma once
#include <string>
#include "vlc.h"
struct MediaSize
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
	int GetVolume();
	long long GetMediaLen();
	MediaSize getMediaSize();
	std::string UnicodeToUTF8(const std::wstring& strIn);

private:
	libvlc_instance_t* m_instance;
	libvlc_media_t* m_media;
	libvlc_media_player_t* m_player;
	std::string m_url;
	HWND m_hWnd;
};

