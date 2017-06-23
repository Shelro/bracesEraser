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


// GrablinkHfrTriggerView.h : interface of the CGrablinkHfrTriggerView class
//
/////////////////////////////////////////////////////////////////////////////

#include "MainFrm.h"

#if !defined(AFX_PROJECT1VIEW_H__8C4A4649_6FC6_44B7_A9D1_643714B58971__INCLUDED_)
#define AFX_PROJECT1VIEW_H__8C4A4649_6FC6_44B7_A9D1_643714B58971__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGrablinkHfrTriggerView : public CView
{
protected: // create from serialization only
    CGrablinkHfrTriggerView();
    DECLARE_DYNCREATE(CGrablinkHfrTriggerView)

// Attributes
public:
    CGrablinkHfrTriggerDoc* GetDocument();

private:

    CMainFrame* m_pMainFrame;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGrablinkHfrTriggerView)
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CGrablinkHfrTriggerView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // Application specific data
    BITMAPINFO * m_pBitmapInfo;

    CString m_strChannelStatus;
    int m_ChannelState;

// Generated message map functions
protected:
    //{{AFX_MSG(CGrablinkHfrTriggerView)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GrablinkHfrTriggerView.cpp
inline CGrablinkHfrTriggerDoc* CGrablinkHfrTriggerView::GetDocument()
    { return (CGrablinkHfrTriggerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECT1VIEW_H__8C4A4649_6FC6_44B7_A9D1_643714B58971__INCLUDED_)
