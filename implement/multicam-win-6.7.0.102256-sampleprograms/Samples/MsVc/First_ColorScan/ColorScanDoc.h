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
// ColorScanDoc.h : interface of the CColorScanDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCANDOC_H__77453804_F927_45DB_B8C7_92D00D6F4004__INCLUDED_)
#define AFX_COLORSCANDOC_H__77453804_F927_45DB_B8C7_92D00D6F4004__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CColorScanDoc : public CDocument
{
protected: // create from serialization only
    CColorScanDoc();
    DECLARE_DYNCREATE(CColorScanDoc)

    // Euresys

public:
    INT32 m_ImageSizeX_Px;
    INT32 m_ImageSizeY_Ln;
    INT32 m_ColorFormat;
    INT32 m_PlaneCount;
    INT32 m_PlaneSize_Bytes;
    INT32 m_SurfacePitch_Bytes;
    INT32 m_AcquisitionMode;

    volatile PUINT8 m_DisplayImageBuffer;
    volatile BOOL m_bOngoingRefresh;

protected:
    volatile BOOL m_bLive;
    volatile BOOL m_bActive;

    MCHANDLE m_Channel;

    BOOL Initialize();
    BOOL Terminate();
    BOOL Grab();
    BOOL Live();
    BOOL StopLive();

public:
    void CallBackFunction(MCCALLBACKINFO &CbInfo);

protected:

    // Shading corrector
    BOOL m_bSHCEnabled;
    BOOL m_bSHCCalibrated;
    BOOL SHCUse(int Index);
    BOOL SHCCalibration(int Index);
    BOOL CreateSurfaceForProfile(MCHANDLE &Handle, PFLOAT32 &Buffer);
    BOOL BuildProfile(PFLOAT32 Buffer);

    // LUTs
    int m_LutIndex;
    BOOL InitializeLUTs();
    BOOL BuildLUT(int Index);
    BOOL LUTUse(int Index);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorScanDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CColorScanDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CColorScanDoc)
    afx_msg void OnGrab();
    afx_msg void OnUpdateGrab(CCmdUI* pCmdUI);
    afx_msg void OnLive();
    afx_msg void OnUpdateLive(CCmdUI* pCmdUI);
    afx_msg void OnSHC();
    afx_msg void OnUpdateSHC(CCmdUI* pCmdUI);
    afx_msg void OnSHCCalibration();
    afx_msg void OnUpdateSHCCalibration(CCmdUI* pCmdUI);
    afx_msg void OnLUT1();
    afx_msg void OnUpdateLUT1(CCmdUI* pCmdUI);
    afx_msg void OnLUT2();
    afx_msg void OnUpdateLUT2(CCmdUI* pCmdUI);
    afx_msg void OnLUT3();
    afx_msg void OnUpdateLUT3(CCmdUI* pCmdUI);
    afx_msg void OnLUT4();
    afx_msg void OnUpdateLUT4(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


void ErrorDisplay(const char *Msg, int Code);
void WINAPI GlobalCallbackFunction(PMCCALLBACKINFO CbInfo);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSCANDOC_H__77453804_F927_45DB_B8C7_92D00D6F4004__INCLUDED_)
