
// VideoPlayerDlg.h: 头文件
//

#pragma once


// CVideoPlayerDlg 对话框
class CVideoPlayerDlg : public CDialogEx
{
// 构造
public:
	CVideoPlayerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOPLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_video;
	CSliderCtrl m_volume;
	CSliderCtrl m_pos;
	CEdit m_url;
	CButton m_BtnPlay;
	bool m_status = false;
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
