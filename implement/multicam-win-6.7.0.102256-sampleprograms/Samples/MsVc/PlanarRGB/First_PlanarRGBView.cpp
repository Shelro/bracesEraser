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


// PlanarRGBView.cpp : implementation of the CPlanarRGBView class
//

#include "StdAfx.h"
#include "First_PlanarRGB.h"

#include "First_PlanarRGBDoc.h"
#include "First_PlanarRGBView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView

IMPLEMENT_DYNCREATE(CPlanarRGBView, CScrollView)

BEGIN_MESSAGE_MAP(CPlanarRGBView, CScrollView)
	//{{AFX_MSG_MAP(CPlanarRGBView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView construction/destruction

CPlanarRGBView::CPlanarRGBView()
{
	DWORD bitmapInfoSize, i;

	// Bitmap info structure for the image
	bitmapInfoSize = sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);
	m_pBmiImage = (BITMAPINFO*)new BYTE[bitmapInfoSize];

	m_pBmiImage->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmiImage->bmiHeader.biPlanes = 1;
	m_pBmiImage->bmiHeader.biBitCount = 8;
	m_pBmiImage->bmiHeader.biCompression = BI_RGB;
	m_pBmiImage->bmiHeader.biSizeImage = 0;
	m_pBmiImage->bmiHeader.biXPelsPerMeter = 0;
	m_pBmiImage->bmiHeader.biYPelsPerMeter = 0;
	m_pBmiImage->bmiHeader.biClrUsed = 0;
	m_pBmiImage->bmiHeader.biClrImportant = 0;

	m_pBmiImage->bmiHeader.biWidth = 0;
	m_pBmiImage->bmiHeader.biHeight = 0;

	for (i = 0 ; i < 256 ; i++) {
		m_pBmiImage->bmiColors[i].rgbBlue = (BYTE)i;
		m_pBmiImage->bmiColors[i].rgbGreen = (BYTE)i;
		m_pBmiImage->bmiColors[i].rgbRed = (BYTE)i;
		m_pBmiImage->bmiColors[i].rgbReserved = 0;
	}
}

CPlanarRGBView::~CPlanarRGBView()
{
	if (m_pBmiImage)
		delete m_pBmiImage ;
}

BOOL CPlanarRGBView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView drawing

void CPlanarRGBView::OnDraw(CDC* pDC)
{
	CPlanarRGBDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	CMainFrame *mainWnd = (CMainFrame*)AfxGetMainWnd();
	
	if (!pDoc)
		return;

	m_pBmiImage->bmiHeader.biWidth = pDoc->m_ImageSizeX;
	m_pBmiImage->bmiHeader.biHeight = - (pDoc->m_ImageSizeY);
	m_pBmiImage->bmiHeader.biBitCount = 8 * pDoc->m_BufferPitch / pDoc->m_ImageSizeX;

	if (pDoc->m_ImageBufferR != NULL && pDoc->m_ImageBufferG != NULL && pDoc->m_ImageBufferB != NULL)
	{
		// Draw RED plane
		SetDIBitsToDevice(pDC->GetSafeHdc(),
			// Destination rectangle
			0, 0, pDoc->m_ImageSizeX, pDoc->m_ImageSizeY,
			// Source rectangle
			0, 0, 0, pDoc->m_ImageSizeY,
			pDoc->m_ImageBufferR, m_pBmiImage, DIB_RGB_COLORS);

		// Draw GREEN plane
		SetDIBitsToDevice(pDC->GetSafeHdc(),
			// Destination rectangle
			pDoc->m_ImageSizeX, 0, pDoc->m_ImageSizeX, pDoc->m_ImageSizeY,
			// Source rectangle
			0, 0, 0, pDoc->m_ImageSizeY,
			pDoc->m_ImageBufferG, m_pBmiImage, DIB_RGB_COLORS);

		// Draw BLUE plane
		SetDIBitsToDevice(pDC->GetSafeHdc(),
			// Destination rectangle
			0, pDoc->m_ImageSizeY, pDoc->m_ImageSizeX, pDoc->m_ImageSizeY,
			// Source rectangle
			0, 0, 0, pDoc->m_ImageSizeY,
			pDoc->m_ImageBufferB, m_pBmiImage, DIB_RGB_COLORS);

		// Update text in the status bar
		mainWnd->SetStatusText("Upper left image : RED plane | Upper right image : GREEN plane | Bottom left image : BLUE plane");
	}

	pDoc->m_bOngoingRefresh = FALSE;
}

void CPlanarRGBView::OnInitialUpdate()
{
	CPlanarRGBDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = pDoc->m_ImageSizeX;
	sizeTotal.cy = pDoc->m_ImageSizeY;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView printing

BOOL CPlanarRGBView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPlanarRGBView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPlanarRGBView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView diagnostics

#ifdef _DEBUG
void CPlanarRGBView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPlanarRGBView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPlanarRGBDoc* CPlanarRGBView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlanarRGBDoc)));
	return (CPlanarRGBDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBView message handlers
