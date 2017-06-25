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


// PlanarRGB.h : main header file for the PlanarRGB application
//

#if !defined(AFX_PLANARRGB_H__0F7905F5_2F83_11D3_8993_0050042A3D34__INCLUDED_)
#define AFX_PLANARRGB_H__0F7905F5_2F83_11D3_8993_0050042A3D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// Euresys :
// Application-specific settings
//
#define EURESYS_SAMPLE_PROGRAM_WINDOW_TITLE "MultiCam - First_PlanarRGB sample program"

#define EURESYS_DRIVER_INDEX 0
#define EURESYS_CONNECTOR MC_Connector_M
#define EURESYS_SAMPLE_PROGRAM_CAM_FILENAME "Colibri2048CL_L2048RGM"


/////////////////////////////////////////////////////////////////////////////
// Euresys :
// Include MultiCam header file
//
#include <multicam.h>



/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBApp:
// See PlanarRGB.cpp for the implementation of this class
//

class CPlanarRGBApp : public CWinApp
{
public:
	CPlanarRGBApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlanarRGBApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPlanarRGBApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANARRGB_H__0F7905F5_2F83_11D3_8993_0050042A3D34__INCLUDED_)
