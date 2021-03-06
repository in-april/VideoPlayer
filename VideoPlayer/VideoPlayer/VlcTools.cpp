#include "pch.h"
#include "VlcTools.h"

CVlcTools::CVlcTools()
{
	m_instance = libvlc_new(0, nullptr);
	m_media = nullptr;
	m_player = nullptr;
	m_hWnd = NULL;
}

CVlcTools::~CVlcTools()
{
	if (m_player)
	{
		libvlc_media_player_release(m_player);
		m_player = nullptr;
	}
	if (m_media)
	{
		libvlc_media_release(m_media);
		m_media = nullptr;
	}
	if (m_instance)
	{
		libvlc_release(m_instance);
		m_instance = nullptr;
	}
}

int CVlcTools::SetMedia(std::string& strUrl)
{
	if (!m_instance || !m_hWnd) return -1;
	if (strUrl == m_url) return 0;
	else m_url = strUrl;

	if (m_media != nullptr)
	{
		libvlc_media_release(m_media);
	}
	m_media = libvlc_media_new_location(m_instance, m_url.c_str());
	if (!m_media) return -2;

	if (m_player != nullptr)
	{
		libvlc_media_player_release(m_player);
	}
	m_player = libvlc_media_player_new_from_media(m_media);
	if (!m_player) return -3;

	libvlc_media_player_set_hwnd(m_player, m_hWnd);
	return 0;
}

int CVlcTools::SetHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
	return 0;
}

int CVlcTools::Play()
{
	if (!m_instance || !m_media || !m_player) return -1;
	return libvlc_media_player_play(m_player);
}

int CVlcTools::Pause()
{
	if (!m_instance || !m_media || !m_player) return -1;
	libvlc_media_player_pause(m_player);
	return 0;
}

int CVlcTools::Stop()
{
	if (!m_instance || !m_media || !m_player) return -1;
	libvlc_media_player_stop(m_player);
	return 0;
}

void CVlcTools::SetPosition(float pos)
{
	if (!m_instance || !m_media || !m_player) return;
	libvlc_media_player_set_position(m_player, pos);
}

float CVlcTools::GetPosition()
{
	if (!m_instance || !m_media || !m_player) return -1.0;
	return libvlc_media_player_get_position(m_player);
}

int CVlcTools::SetVolume(int volume)
{
	if (!m_instance || !m_media || !m_player) return -1;
	return libvlc_audio_set_volume(m_player, volume);
}

int CVlcTools::GetVolume()
{
	if (!m_instance || !m_media || !m_player) return -1;
	return libvlc_audio_get_volume(m_player);
}

long long CVlcTools::GetMediaLen()
{
	if (!m_instance || !m_media || !m_player) return -1;
	return libvlc_media_player_get_length(m_player);
}

MediaSize CVlcTools::getMediaSize()
{
	if (!m_instance || !m_media || !m_player) return {-1,-1};
	int width = libvlc_video_get_width(m_player);
	int height = libvlc_video_get_height(m_player);
	return { width, height };
}

std::string CVlcTools::UnicodeToUTF8(const std::wstring& strIn)
{
	std::string str;
	int len = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
	str.resize(len);
	::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), len, NULL, NULL);
	return str;
}
