/*
+-------------------------------- DISCLAIMER ---------------------------------+
|                                                                             |
| This application program is provided to you free of charge as an example.   |
| Despite the considerable efforts of Euresys personnel to create a usable    |
| example, you should not assume that this program is error-free or suitable  |
| for any purpose whatsoever.                                                 |
|                                                                             |
| EURESYS does not give any representation, warranty or undertaking that this |
| program is free of any defect or error or suitable for any purpose. EURESYS |
| shall not be liable, in contract, in torts or otherwise, for any damages,   |
| loss, costs, expenses or other claims for compensation, including those     |
| asserted by third parties, arising out of or in connection with the use of  |
| this program.                                                               |
|                                                                             |
+-----------------------------------------------------------------------------+
*/
// ColorScan.h : main header file for the COLORSCAN application
//

#if !defined(AFX_COLORSCAN_H__F5594859_B0E8_4461_AC9D_43C5B819FDC1__INCLUDED_)
#define AFX_COLORSCAN_H__F5594859_B0E8_4461_AC9D_43C5B819FDC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// Euresys:
// Include MultiCam header file
//
#include <multicam.h>

/////////////////////////////////////////////////////////////////////////////
// Euresys :
// Application-specific defines
//
// Window title
#define EURESYS_SAMPLE_PROGRAM_WINDOW_TITLE "QUICKPACK ColorScan"
// Application name
#define EURESYS_SAMPLE_PROGRAM_NAME "             QUICKPACK ColorScan\n"
// CAM file
#define EURESYS_CAM_FILENAME		"NUCL7500D_L7500RP.cam"
// Driver index
#define EURESYS_DRIVER_INDEX		0
// Virtual COM port name
#define EURESYS_VIRTUAL_COM_PORT	"COM3"


/////////////////////////////////////////////////////////////////////////////
// CColorScanApp:
// See ColorScan.cpp for the implementation of this class
//

class CColorScanApp : public CWinApp
{
public:
    CColorScanApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorScanApp)
public:
    virtual BOOL InitInstance();
    virtual BOOL ExitInstance();
    //}}AFX_VIRTUAL

    // Implementation
    //{{AFX_MSG(CColorScanApp)
    afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSCAN_H__F5594859_B0E8_4461_AC9D_43C5B819FDC1__INCLUDED_)
