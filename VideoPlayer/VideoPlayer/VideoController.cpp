#include "pch.h"
#include "VideoController.h"

CVideoController::CVideoController()
{
	m_playerDlg.pVideoController = this;
}

CVideoController::~CVideoController()
{
}

int CVideoController::Init(CWnd*& pWnd)
{
    pWnd = &m_playerDlg;
    return 0;
}

int CVideoController::Invoke()
{
	INT_PTR nResponse = m_playerDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}
    return nResponse;
}

int CVideoController::SetMedia(std::string strUrl)
{
	return m_vlc.SetMedia(strUrl);
}

float CVideoController::VideoCtrl(Ctrl cmd)
{
	switch (cmd)
	{
	case Ctrl::PLAY:
		return m_vlc.Play();
		break;
	case Ctrl::PAUSE:
		return m_vlc.Pause();
		break;
	case Ctrl::STOP:
		return m_vlc.Stop();
		break;
	case Ctrl::Get_POSITION:
		return m_vlc.GetPosition();
		break;
	case Ctrl::GET_VOLUME:
		return m_vlc.GetVolume();
		break;
	default:
		break;
	}
	return -1.0;
}

void CVideoController::SetVolume(int volume)
{
	m_vlc.SetVolume(volume);
}

void CVideoController::SetPosition(float positon)
{
	m_vlc.SetPosition(positon);
}

void CVideoController::SetHwnd(HWND hWnd)
{
	m_vlc.SetHwnd(hWnd);
}

MediaSize CVideoController::getMediaSize()
{
	return m_vlc.getMediaSize();
}

long long CVideoController::getMediaLen()
{
	return m_vlc.GetMediaLen();
}

std::string CVideoController::UnicodeToUTF8(const std::wstring& strIn)
{
	return m_vlc.UnicodeToUTF8(strIn);
}
