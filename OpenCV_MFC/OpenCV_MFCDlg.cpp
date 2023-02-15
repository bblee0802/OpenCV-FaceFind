
// OpenCV_MFCDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "OpenCV_MFC.h"
#include "OpenCV_MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

VideoCapture g_Cap;
CascadeClassifier g_FaceDetect;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COpenCVMFCDlg dialog

COpenCVMFCDlg::COpenCVMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENCV_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bStart = FALSE;
	m_bStopThread = FALSE;
	m_hThread = NULL;
}

void COpenCVMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COpenCVMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &COpenCVMFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COpenCVMFCDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// COpenCVMFCDlg message handlers


DWORD WINAPI ThreadProc(LPVOID lpParam) {
	COpenCVMFCDlg *main = (COpenCVMFCDlg *)lpParam;
	main->DoOpenCV();

	return 0;
}

BOOL COpenCVMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	HWND hWnd = (HWND)cvGetWindowHandle("Window1");
	::SetParent(hWnd, GetDlgItem(IDC_SCREEN)->m_hWnd);
	cvResizeWindow("Window1", 720, 480);


	MakeFaceDetect();

	if (g_Cap.open(0, CAP_MSMF)) {
		DWORD dwThreadId;
		m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadId);
		m_bStart = TRUE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COpenCVMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COpenCVMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COpenCVMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenCVMFCDlg::FindFace(void *pFrame)
{
	Mat *frame = (Mat *)pFrame;
    UMat gray, processed;
	cv::cvtColor(*frame, gray, COLOR_BGR2GRAY);

	std::vector<Rect> objects;
	g_FaceDetect.detectMultiScale(gray, objects);

	for (int i = 0; i < objects.size(); i++) {
		Rect rc = objects.at(i);
		cv::rectangle(*frame, rc, Scalar(255, 0, 0), 2);
		cv::putText(*frame, "Face", Point(rc.x, rc.y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 1);
	}
}

void COpenCVMFCDlg::DoOpenCV()
{
	if (!g_Cap.isOpened())
		return;

	Mat frame;
	for (;;)
	{
		g_Cap.read(frame);
		if (frame.empty()) {
			break;
		}
		//imshow("Window1", frame);
		FindFace(&frame);
		IplImage iplImage = cvIplImage(frame);
		cvShowImage("Window1", &iplImage);

		if (m_bStopThread)
			break;
	}
}

int COpenCVMFCDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
			
	cvNamedWindow("Window1", CV_WINDOW_NORMAL);
	HWND hWnd = (HWND)cvGetWindowHandle("Window1");
	HWND hParent = ::GetParent(hWnd);
	::ShowWindow(hParent, SW_HIDE);
	
	return 0;
}

void COpenCVMFCDlg::DoBeforeExit()
{
	m_bStopThread = TRUE;

	if (m_bStart) {
		WaitForSingleObject(m_hThread, 3000);
		g_Cap.release();
	}
}

void COpenCVMFCDlg::OnBnClickedOk()
{
	DoBeforeExit();
	CDialogEx::OnOK();
}
 
void COpenCVMFCDlg::OnBnClickedCancel()
{
	DoBeforeExit();
	CDialogEx::OnCancel();
}

void COpenCVMFCDlg::OnClose()
{
	DoBeforeExit();
	CDialogEx::OnClose();
}


void COpenCVMFCDlg::MakeFaceDetect()
{
	std::string xml = "./etc/haarcascades/haarcascade_frontalface_alt.xml";
	g_FaceDetect.load(xml);
}
