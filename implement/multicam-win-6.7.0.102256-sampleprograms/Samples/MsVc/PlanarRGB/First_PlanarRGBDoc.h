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


// PlanarRGBDoc.h : interface of the CPlanarRGBDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANARRGBDOC_H__0F7905FB_2F83_11D3_8993_0050042A3D34__INCLUDED_)
#define AFX_PLANARRGBDOC_H__0F7905FB_2F83_11D3_8993_0050042A3D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlanarRGBDoc : public CDocument
{
protected: // create from serialization only
	CPlanarRGBDoc();
	DECLARE_DYNCREATE(CPlanarRGBDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlanarRGBDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlanarRGBDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Euresys

public:
	INT32 m_ImageSizeX;
	INT32 m_ImageSizeY;
	INT32 m_BufferPitch;
	INT32 m_PlaneCount;
	INT32 m_ColorStorage;
	INT32 m_AcquisitionMode;

	volatile PUINT8 m_ImageBufferR;
	volatile PUINT8 m_ImageBufferG;
	volatile PUINT8 m_ImageBufferB;
	volatile BOOL m_bOngoingRefresh;

protected:
	BOOL Initialize();
	BOOL Terminate();
	BOOL Grab();

	MCHANDLE m_Channel;

public:
	void MultiCamCallBackFunction (MCCALLBACKINFO&);


// Generated message map functions
protected:
	//{{AFX_MSG(CPlanarRGBDoc)
	afx_msg void OnGrab();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANARRGBDOC_H__0F7905FB_2F83_11D3_8993_0050042A3D34__INCLUDED_)
