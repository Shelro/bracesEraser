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


// DominoSnapshotTriggerDoc.h : interface of the CDominoSnapshotTriggerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_)
#define AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DominoSnapshotTrigger.h"
#include "multicam.h"

//---------------------------------------------------------------------------
// Callback function declaration
void WINAPI GlobalCallback (PMCSIGNALINFO SigInfo);

class CDominoSnapshotTriggerDoc : public CDocument
{

// Application specific data
public:
    MCHANDLE m_Channel;
    void Callback (PMCSIGNALINFO SigInfo);
    PVOID m_pCurrent;
    int m_SizeX;
    int m_SizeY;
    int m_BufferPitch;
    volatile BOOL m_bScreenRefreshCompleted;

protected: // create from serialization only
    CDominoSnapshotTriggerDoc();
    DECLARE_DYNCREATE(CDominoSnapshotTriggerDoc)

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDominoSnapshotTriggerDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CDominoSnapshotTriggerDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // Generated message map functions


protected:
    //{{AFX_MSG(CDominoSnapshotTriggerDoc)
    afx_msg void OnGo();
    afx_msg void OnStop();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECT1DOC_H__A2C9A054_8AFA_4C3E_A5AA_2E1A44CD3916__INCLUDED_)
