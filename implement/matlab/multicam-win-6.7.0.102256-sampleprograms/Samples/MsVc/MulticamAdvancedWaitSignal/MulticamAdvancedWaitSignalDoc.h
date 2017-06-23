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


// MulticamAdvancedWaitSignalDoc.h : interface of the CMulticamAdvancedWaitSignalDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_)
#define AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MulticamAdvancedWaitSignal.h"
#include "multicam.h"

//---------------------------------------------------------------------------
// LiveThread function declaration
UINT LiveThreadProc( LPVOID Param );

class CMulticamAdvancedWaitSignalDoc : public CDocument
{

// Application specific data
public:
    MCHANDLE m_Channel;
    void HandleSignal (PMCSIGNALINFO SigInfo);
    PVOID m_pCurrent;
    int m_SizeX;
    int m_SizeY;
    int m_BufferPitch;
    volatile BOOL m_bScreenRefreshCompleted;
	BOOL m_RunLoop;

protected: // create from serialization only
    CMulticamAdvancedWaitSignalDoc();
    DECLARE_DYNCREATE(CMulticamAdvancedWaitSignalDoc)

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMulticamAdvancedWaitSignalDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMulticamAdvancedWaitSignalDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // Generated message map functions


protected:
    //{{AFX_MSG(CMulticamAdvancedWaitSignalDoc)
    afx_msg void OnGo();
    afx_msg void OnStop();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_)
