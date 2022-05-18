#pragma once
#include "VlcTools.h"
#include "VideoPlayerDlg.h"

enum class Ctrl
{
	PLAY,
	PAUSE,
	STOP,
	Get_POSITION,
	GET_VOLUME
};

class CVideoController
{
public:
	CVideoController();
	~CVideoController();
	int Init(CWnd*& pWnd);
	int Invoke();
	int SetMedia(std::string strUrl);
	float VideoCtrl(Ctrl cmd);
	void SetVolume(int volume);
	void SetPosition(float positon);
	void SetHwnd(HWND hWnd);
	MediaSize getMediaSize();
	long long getMediaLen();
	std::string UnicodeToUTF8(const std::wstring& strIn);
protected:
	CVlcTools m_vlc;
	CVideoPlayerDlg m_playerDlg;
};

