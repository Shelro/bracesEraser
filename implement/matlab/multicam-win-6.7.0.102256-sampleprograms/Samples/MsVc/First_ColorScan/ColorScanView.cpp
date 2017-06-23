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
// ColorScanView.cpp : implementation of the CColorScanView class
//

#include "stdafx.h"
#include "ColorScan.h"

#include "ColorScanDoc.h"
#include "ColorScanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorScanView

IMPLEMENT_DYNCREATE(CColorScanView, CScrollView)

BEGIN_MESSAGE_MAP(CColorScanView, CScrollView)
    //{{AFX_MSG_MAP(CColorScanView)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorScanView construction/destruction

CColorScanView::CColorScanView()
{
    DWORD bitmapInfoSize, i;

    // Bitmap info structure for the image
    bitmapInfoSize = sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD);
    m_pBmiImage = (BITMAPINFO *) new BYTE[bitmapInfoSize];

    m_pBmiImage->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_pBmiImage->bmiHeader.biPlanes = 1;
    m_pBmiImage->bmiHeader.biCompression = BI_RGB;
    m_pBmiImage->bmiHeader.biSizeImage = 0;
    m_pBmiImage->bmiHeader.biXPelsPerMeter = 0;
    m_pBmiImage->bmiHeader.biYPelsPerMeter = 0;
    m_pBmiImage->bmiHeader.biClrUsed = 0;
    m_pBmiImage->bmiHeader.biClrImportant = 0;

    m_pBmiImage->bmiHeader.biWidth = 0;
    m_pBmiImage->bmiHeader.biHeight = 0;
    m_pBmiImage->bmiHeader.biBitCount = 0;

    for (i = 0 ; i < 256 ; i++)
    {
        m_pBmiImage->bmiColors[i].rgbBlue = (BYTE)i;
        m_pBmiImage->bmiColors[i].rgbGreen = (BYTE)i;
        m_pBmiImage->bmiColors[i].rgbRed = (BYTE)i;
        m_pBmiImage->bmiColors[i].rgbReserved = 0;
    }
}

CColorScanView::~CColorScanView()
{
    if (m_pBmiImage != NULL)
        delete m_pBmiImage;
}

BOOL CColorScanView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CColorScanView drawing

void CColorScanView::OnDraw(CDC* pDC)
{
    CColorScanDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    m_pBmiImage->bmiHeader.biWidth = pDoc->m_ImageSizeX_Px;
    m_pBmiImage->bmiHeader.biHeight = - (pDoc->m_ImageSizeY_Ln * pDoc->m_PlaneCount);
    m_pBmiImage->bmiHeader.biBitCount = 8 * (pDoc->m_SurfacePitch_Bytes / pDoc->m_ImageSizeX_Px) / pDoc->m_PlaneCount;

    if (pDoc->m_DisplayImageBuffer != NULL)
    {
        SetDIBitsToDevice(pDC->GetSafeHdc(),
            0, 0, pDoc->m_ImageSizeX_Px, pDoc->m_ImageSizeY_Ln * pDoc->m_PlaneCount,
            0, 0, 0, pDoc->m_ImageSizeY_Ln * pDoc->m_PlaneCount,
            pDoc->m_DisplayImageBuffer, m_pBmiImage, DIB_RGB_COLORS);
    }

    pDoc->m_bOngoingRefresh = FALSE;
}

void CColorScanView::OnInitialUpdate()
{
    CColorScanDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CScrollView::OnInitialUpdate();

    CSize sizeTotal;
    // TODO: calculate the total size of this view
    sizeTotal.cx = pDoc->m_ImageSizeX_Px;
    sizeTotal.cy = pDoc->m_ImageSizeY_Ln * pDoc->m_PlaneCount;
    SetScrollSizes(MM_TEXT, sizeTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CColorScanView diagnostics

#ifdef _DEBUG
void CColorScanView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CColorScanView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}

CColorScanDoc* CColorScanView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorScanDoc)));
    return (CColorScanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColorScanView message handlers
