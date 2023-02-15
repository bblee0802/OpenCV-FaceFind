
// OpenCV_MFCDlg.h : header file
//

#pragma once


// COpenCVMFCDlg dialog
class COpenCVMFCDlg : public CDialogEx
{
// Construction
public:
	COpenCVMFCDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENCV_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	HANDLE m_hThread;
	BOOL m_bStart;
	BOOL m_bStopThread;
public:
	void DoOpenCV();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	void DoBeforeExit();
	void MakeFaceDetect();
	void FindFace(void *frame);
};
