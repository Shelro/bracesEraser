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
// ColorScanView.h : interface of the CColorScanView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCANVIEW_H__50B3F4D1_AC91_4D59_80E8_3722DFFF8139__INCLUDED_)
#define AFX_COLORSCANVIEW_H__50B3F4D1_AC91_4D59_80E8_3722DFFF8139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CColorScanView : public CScrollView
{
protected: // create from serialization only
    CColorScanView();
    DECLARE_DYNCREATE(CColorScanView)

    // Attributes
public:
    CColorScanDoc* GetDocument();

    // Euresys
protected:
    BITMAPINFO *m_pBmiImage;

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorScanView)
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual void OnInitialUpdate(); // called first time after construct
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CColorScanView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CColorScanView)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ColorScanView.cpp
inline CColorScanDoc* CColorScanView::GetDocument()
{ return (CColorScanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSCANVIEW_H__50B3F4D1_AC91_4D59_80E8_3722DFFF8139__INCLUDED_)
