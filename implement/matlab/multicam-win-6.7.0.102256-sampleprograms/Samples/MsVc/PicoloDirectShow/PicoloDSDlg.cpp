// PicoloDSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Helper.h"
#include "PicoloDirectShow.h"
#include "PicoloDSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPicoloDSDlg dialog




CPicoloDSDlg::CPicoloDSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPicoloDSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPicoloDSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPicoloDSDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_START, &CPicoloDSDlg::OnBnClickedBtStart)
	ON_BN_CLICKED(IDC_BT_STOP, &CPicoloDSDlg::OnBnClickedBtStop)
END_MESSAGE_MAP()


// CPicoloDSDlg message handlers

BOOL CPicoloDSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Picolo.SetVideoStandard(ESF_STANDARD_PAL);
	
	// Get The static frame for display
	CWnd * ctl = GetDlgItem(IDC_DISPLAY);
	m_Picolo.Initialize(L"record.avi",ctl->m_hWnd);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPicoloDSDlg::OnPaint()
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
		// Window need redraw
		HRESULT hr = m_Picolo.Paint();
		// If a failure occurs, stop acquisition
		if (hr != S_OK) m_Picolo.Stop();

		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPicoloDSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPicoloDSDlg::OnBnClickedBtStart()
{
	// Start raw acquisition and display
	m_Picolo.Start();
}


void CPicoloDSDlg::OnBnClickedBtStop()
{
	// Stop acquisition
	m_Picolo.Stop();
}
