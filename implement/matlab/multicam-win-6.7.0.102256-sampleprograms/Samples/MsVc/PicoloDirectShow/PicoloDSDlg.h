// PicoloDSDlg.h : header file
//
#include "helper.h"
#pragma once


// CPicoloDSDlg dialog
class CPicoloDSDlg : public CDialog
{
// Construction
public:
	CPicoloDSDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PICOLODIRECTSHOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CPicoloDS m_Picolo;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtStart();
	afx_msg void OnBnClickedBtStop();
};
