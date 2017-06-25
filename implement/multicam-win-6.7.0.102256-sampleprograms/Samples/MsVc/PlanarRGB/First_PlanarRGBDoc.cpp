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


// PlanarRGBDoc.cpp : implementation of the CPlanarRGBDoc class
//

#include "StdAfx.h"
#include "First_PlanarRGB.h"

#include "First_PlanarRGBDoc.h"
#include "First_PlanarRGBView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBDoc

IMPLEMENT_DYNCREATE(CPlanarRGBDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlanarRGBDoc, CDocument)
	//{{AFX_MSG_MAP(CPlanarRGBDoc)
	ON_COMMAND(ID_GRAB, OnGrab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBDoc construction/destruction

CPlanarRGBDoc::CPlanarRGBDoc()
{
	m_ImageSizeX = 1000;
	m_ImageSizeY = 1000;
	m_ImageBufferR = NULL;
	m_ImageBufferG = NULL;
	m_ImageBufferB = NULL;

	m_Channel = NULL;

	char pText [512];

	sprintf_s(pText,512, "This program will use the following settings :\n\n"
				   "Camera File (.CAM) : \"%s\"\n\n"
					 "Color Format : RGB24PL\n\n"
					 "Note that this program will work only for this color format."
					 , EURESYS_SAMPLE_PROGRAM_CAM_FILENAME);

	AfxMessageBox(pText, MB_OK|MB_ICONINFORMATION);
}

CPlanarRGBDoc::~CPlanarRGBDoc()
{
	if (!Terminate())
	{
		AfxMessageBox("Error: Cannot terminate MultiCam application.", MB_OK | MB_ICONERROR);
	}
}

BOOL CPlanarRGBDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	if (!Initialize())
	{
		AfxMessageBox("Error: Cannot initialize MultiCam application.", MB_OK | MB_ICONERROR);
		return FALSE ;
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBDoc serialization

void CPlanarRGBDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBDoc diagnostics

#ifdef _DEBUG
void CPlanarRGBDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlanarRGBDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlanarRGBDoc commands

void CPlanarRGBDoc::OnGrab() 
{
	// TODO: Add your command handler code here

	Grab();
}

void ErrorDisplay(const char *Msg, int Code)
{
	CString Txt;
	Txt.Format("\"%s\" Return value = %d", Msg, Code);
	AfxMessageBox(Txt, MB_ICONERROR | MB_OK);
}

void WINAPI GlobalCallbackFunction (PMCCALLBACKINFO CbInfo)
{
	CPlanarRGBDoc* pDoc = (CPlanarRGBDoc*) CbInfo->Context;
	if (pDoc)
		pDoc->MultiCamCallBackFunction(*CbInfo);
}

BOOL CPlanarRGBDoc::Initialize()
{
	MCSTATUS Status ;
	char pText[128];

	// Establish communication with the driver
	Status = McOpenDriver(NULL);
	if (Status != MC_OK)
	{
		ErrorDisplay("Error: McOpenDriver failed.", Status);
		return FALSE ;
	}

	// Create a channel
	Status = McCreate(MC_CHANNEL, &m_Channel);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McCreate Channel failed.", Status);
		return FALSE;
	}

	// Associate a board with the channel 
	Status = McSetParamInt(m_Channel, MC_DriverIndex, EURESYS_DRIVER_INDEX);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McSetParamInt MC_DriverIndex failed.", Status);
		return FALSE;
	}

	// Associate a connector with the channel 
	Status = McSetParamInt(m_Channel, MC_Connector, EURESYS_CONNECTOR);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McSetParamInt MC_Connector failed.", Status);
		return FALSE;
	}

	// Associate a camera file to the channel
	Status = McSetParamStr(m_Channel, MC_CamFile, EURESYS_SAMPLE_PROGRAM_CAM_FILENAME);
	if (Status != MC_OK) 
	{
		sprintf_s(pText,128, "Error: McSetParamStr MC_CamFile %s failed.", EURESYS_SAMPLE_PROGRAM_CAM_FILENAME);
		ErrorDisplay(pText, Status);
		return FALSE;
	}

	// Set the color format for the channel
	Status = McSetParamInt(m_Channel, MC_ColorFormat, MC_ColorFormat_RGB24PL);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McSetParamStr MC_ColorFormat failed.", Status);
		return FALSE;
	}

	// Register callback
	Status = McRegisterCallback(m_Channel, GlobalCallbackFunction, this);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McRegisterCallback failed.", Status);
		return FALSE;
	}

	// Enable event
	Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_FILLED, MC_SignalEnable_ON);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McSetParamInt MC_SignalEnable failed.", Status);
		return FALSE;
	}

	// Retrieve image parameters
	Status = McGetParamInt(m_Channel, MC_ImageSizeY, &m_ImageSizeY);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McGetParamInt MC_ImageSizeY failed.", Status);
		return FALSE;
	}

	Status = McGetParamInt(m_Channel, MC_ImageSizeX, &m_ImageSizeX);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McGetParamInt MC_ImageSizeX failed.", Status);
		return FALSE;
	}

	// RGB planar
	Status = McGetParamInt(m_Channel, MC_ImagePlaneCount, &m_PlaneCount);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McGetParamInt MC_ImagePlaneCount failed.", Status);
		return FALSE;
	}
	if (m_PlaneCount != 3)
	{
		AfxMessageBox("This sample program was designed to operate the Expert frame-grabber\n"
					  "and perform image acquisitions with 3 planes.\n\n",
					  MB_OK | MB_ICONINFORMATION);
		return FALSE ;
	}

	Status = McGetParamInt(m_Channel, MC_ColorStorage, &m_ColorStorage);
	if (Status == MC_OK && m_ColorStorage != MC_ColorStorage_PLANAR)
	{
		AfxMessageBox("This sample program was designed to operate the Expert frame-grabber\n"
					  "and perform image acquisitions with 3 planes.\n\n",
					  MB_OK | MB_ICONINFORMATION);
		return FALSE ;
	}

	Status = McGetParamInt(m_Channel, MC_BufferPitch, &m_BufferPitch);
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: McGetParamInt MC_BufferPitch failed.", Status);
		return FALSE;
	}

	// Set Channel to Idle state
	Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE); 
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
		return FALSE;
	}

	return TRUE;
}

BOOL CPlanarRGBDoc::Terminate()
{
	MCSTATUS Status ;

	// Delete Channel Instance
	if (m_Channel)
	{
		Status = McDelete(m_Channel);
		if (Status != MC_OK) 
			ErrorDisplay("Error: McDelete m_Channel failed.", Status);
	}

	// Terminate communication with the driver
	Status = McCloseDriver();
	if (Status != MC_OK)
		ErrorDisplay("Error: McCloseDriver failed.", Status);

	return TRUE ;
}

BOOL CPlanarRGBDoc::Grab()
{
	MCSTATUS Status ;

	// Set Channel to Idle state
	Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE); 
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
		return FALSE;
	}

	// Set SeqLength_Pg or SeqLength_Fr to "1" -> one single acquisition
	Status = McGetParamInt(m_Channel, MC_AcquisitionMode, &m_AcquisitionMode); 
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: Cannot get AcquisitionMode.", Status);
		return FALSE;
	}
	if (m_AcquisitionMode == MC_AcquisitionMode_PAGE)
	{
		Status = McSetParamInt(m_Channel, MC_SeqLength_Pg, 1); 
		if (Status != MC_OK) 
		{
			ErrorDisplay("Error: Cannot set SeqLength_Pg.", Status);
			return FALSE;
		}
	}
	else
	{
		Status = McSetParamInt(m_Channel, MC_SeqLength_Fr, 1); 
		if (Status != MC_OK) 
		{
			ErrorDisplay("Error: Cannot set SeqLength_Fr.", Status);
			return FALSE;
		}
	}

	// Set Channel to Active state
	Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE); 
	if (Status != MC_OK) 
	{
		ErrorDisplay("Error: Cannot put channel in ACTIVE state.", Status);
		return FALSE;
	}

	return TRUE ;
}

void CPlanarRGBDoc::MultiCamCallBackFunction(MCCALLBACKINFO& CbInfo)
{
	// Find associated CPlanarRGBView
	POSITION pos = GetFirstViewPosition();
	CPlanarRGBView * pView = (CPlanarRGBView *) GetNextView(pos) ;
	if (pView==NULL)
		return;
	HWND m_hWindow2Update = pView->GetSafeHwnd();
	if ( m_hWindow2Update == NULL )
		return;
	RECT RectImageSize = { 0, 0, 2*(m_ImageSizeX-1), 2*(m_ImageSizeY-1) };

	// Retrieve the image buffer 
	PUINT8 *pData = NULL;
	MCHANDLE hSurface = (MCHANDLE) CbInfo.SignalInfo ;
	McGetParamInt(hSurface, MC_SurfaceContext, (PINT32) &pData);
	McGetParamInt(hSurface, MC_SurfaceAddr+0, (PINT32) &m_ImageBufferR);
	McGetParamInt(hSurface, MC_SurfaceAddr+1, (PINT32) &m_ImageBufferG);
	McGetParamInt(hSurface, MC_SurfaceAddr+2, (PINT32) &m_ImageBufferB);

	// Image Processing 
	if (pData)
	{
		// insert image processing here
	}

	// Post refresh command
	if (!m_bOngoingRefresh)
	{
		m_bOngoingRefresh = TRUE;
		::InvalidateRect(m_hWindow2Update, &RectImageSize, FALSE);
	}
}

