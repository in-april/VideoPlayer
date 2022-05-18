
// VideoPlayerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VideoPlayer.h"
#include "VideoPlayerDlg.h"
#include "afxdialogex.h"
#include "VideoController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoPlayerDlg 对话框



CVideoPlayerDlg::CVideoPlayerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_VIDEO, m_video);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_volume);
	DDX_Control(pDX, IDC_SLIDER_POS, m_pos);
	DDX_Control(pDX, IDC_EDIT_URL, m_url);
	DDX_Control(pDX, IDC_BTN_PLAY, m_BtnPlay);
}

BEGIN_MESSAGE_MAP(CVideoPlayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CVideoPlayerDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVideoPlayerDlg::OnBnClickedBtnStop)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CVideoPlayerDlg 消息处理程序

BOOL CVideoPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_status = 0;

	m_pos.SetRange(0, 100);
	m_volume.SetRange(0, 100);
	for (int i = 0; i < 100; i += 5)
	{
		m_pos.SetTic(i);
		m_volume.SetTic(i);
	}
	//m_pos.SetTicFreq(10);
	m_volume.SetTicFreq(1);
	m_volume.SetPos(100);

	pVideoController->SetHwnd(m_video.GetSafeHwnd());


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoPlayerDlg::OnBnClickedBtnPlay()
{
	if (m_status == 0)
	{
		CString input;
		m_url.GetWindowText(input);
		std::string mediaPath = pVideoController->UnicodeToUTF8(input.GetBuffer());
		SetTimer(0, 500, NULL);
		pVideoController->SetMedia(mediaPath);
		pVideoController->VideoCtrl(Ctrl::PLAY);
		m_BtnPlay.SetWindowText(_T("暂停"));
		m_status = 2;
	}
	else if (m_status == 1)
	{
		m_BtnPlay.SetWindowText(_T("暂停"));
		m_status = 2;
		pVideoController->VideoCtrl(Ctrl::PLAY);
	}
	else if(m_status == 2)
	{
		m_BtnPlay.SetWindowText(_T("播放"));
		m_status = 1;
		pVideoController->VideoCtrl(Ctrl::PAUSE);
	}
}


void CVideoPlayerDlg::OnBnClickedBtnStop()
{
	m_BtnPlay.SetWindowText(_T("播放"));
	m_status = 0;
	pVideoController->VideoCtrl(Ctrl::STOP);
	m_pos.SetPos(0);
	KillTimer(0);
}


void CVideoPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0)
	{
		long long mediaLen = pVideoController->getMediaLen();
		float pos = pVideoController->VideoCtrl(Ctrl::Get_POSITION);
		long long cur = mediaLen * pos;
		if (pos != -1.0f)
		{
			CString str;
			str.Format(_T("%02d:%02d:%02d/%02d:%02d:%02d"),
				int(cur / 3600000), int(cur / 60000 % 60), int(cur / 1000 % 60),
				int(mediaLen / 3600000), int(mediaLen / 60000 % 60), int(mediaLen / 1000 % 60));
			SetDlgItemText(IDC_STATIC_TIME, str);
		}
		m_pos.SetPos(pos * 100);
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CVideoPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode == SB_THUMBTRACK)
	{
		int id = pScrollBar->GetDlgCtrlID();
		switch (id)
		{
		case IDC_SLIDER_POS:
			m_pos.SetPos(nPos);
			pVideoController->SetPosition((float)nPos/100);
			break;
		case IDC_SLIDER_VOLUME:
		{
			CString str;
			str.Format(_T("音量:%d%%"), nPos);
			SetDlgItemText(IDC_STATIC_VOLUME, str);
			pVideoController->SetVolume(nPos);
			break;
		}
		default:
			break;
		}
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

