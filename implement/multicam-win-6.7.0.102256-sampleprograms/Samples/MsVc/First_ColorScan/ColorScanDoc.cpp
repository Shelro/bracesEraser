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
// ColorScanDoc.cpp : implementation of the CColorScanDoc class
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
// CColorScanDoc

IMPLEMENT_DYNCREATE(CColorScanDoc, CDocument)

BEGIN_MESSAGE_MAP(CColorScanDoc, CDocument)
    //{{AFX_MSG_MAP(CColorScanDoc)
    ON_COMMAND(ID_GRAB, OnGrab)
    ON_UPDATE_COMMAND_UI(ID_GRAB, OnUpdateGrab)
    ON_COMMAND(ID_LIVE, OnLive)
    ON_UPDATE_COMMAND_UI(ID_LIVE, OnUpdateLive)
    ON_COMMAND(ID_SHC, OnSHC)
    ON_UPDATE_COMMAND_UI(ID_SHC, OnUpdateSHC)
    ON_COMMAND(ID_SHC_CALIBRATION, OnSHCCalibration)
    ON_UPDATE_COMMAND_UI(ID_SHC_CALIBRATION, OnUpdateSHCCalibration)
    ON_COMMAND(ID_LUT_LUT1, OnLUT1)
    ON_UPDATE_COMMAND_UI(ID_LUT_LUT1, OnUpdateLUT1)
    ON_COMMAND(ID_LUT_LUT2, OnLUT2)
    ON_UPDATE_COMMAND_UI(ID_LUT_LUT2, OnUpdateLUT2)
    ON_COMMAND(ID_LUT_LUT3, OnLUT3)
    ON_UPDATE_COMMAND_UI(ID_LUT_LUT3, OnUpdateLUT3)
    ON_COMMAND(ID_LUT_LUT4, OnLUT4)
    ON_UPDATE_COMMAND_UI(ID_LUT_LUT4, OnUpdateLUT4)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorScanDoc construction/destruction

CColorScanDoc::CColorScanDoc()
{
    m_ImageSizeX_Px = 0;
    m_ImageSizeY_Ln = 0;
    m_ColorFormat = 0;
    m_PlaneCount = 0;
    m_PlaneSize_Bytes = 0;
    m_SurfacePitch_Bytes = 0;

    m_DisplayImageBuffer = NULL;
    m_bOngoingRefresh = FALSE;

    m_bLive = FALSE;
    m_bActive = FALSE;

    m_Channel = NULL;	

    m_bSHCEnabled = FALSE;
    m_bSHCCalibrated = FALSE;

    m_LutIndex = 0;
}

CColorScanDoc::~CColorScanDoc()
{
    if (!Terminate())
    {
        AfxMessageBox("Error: Cannot terminate MultiCam application.", MB_OK | MB_ICONERROR);
    }
}

BOOL CColorScanDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    if (!Initialize())
    {
        AfxMessageBox("Error: Cannot initialize document.", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CColorScanDoc serialization

void CColorScanDoc::Serialize(CArchive& ar)
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
// CColorScanDoc diagnostics

#ifdef _DEBUG
void CColorScanDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CColorScanDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Error display

void ErrorDisplay(const char *Msg, int Code)
{
    CString Txt;
    Txt.Format("\"%s\" Return value = %d", Msg, Code);
    AfxMessageBox(Txt, MB_ICONERROR | MB_OK);
}

/////////////////////////////////////////////////////////////////////////////
// Global callback function

void WINAPI GlobalCallbackFunction(PMCCALLBACKINFO CbInfo)
{
    CColorScanDoc *pDoc = (CColorScanDoc *)CbInfo->Context;
    if (pDoc != NULL)
        pDoc->CallBackFunction(*CbInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CColorScanDoc commands

void CColorScanDoc::OnGrab() 
{
    Grab();
}

void CColorScanDoc::OnUpdateGrab(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(!m_bActive);
}

void CColorScanDoc::OnLive() 
{
    m_bLive = !m_bLive;

    if (m_bLive)
        Live();
    else
        StopLive();
}

void CColorScanDoc::OnUpdateLive(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(m_bLive || !m_bActive);
    pCmdUI->SetCheck(m_bLive);
}


/////////////////////////////////////////////////////////////////////////////
// CColorScanDoc operations

/////////////////////////////////////////////////////////////////////////////
// Initialization
BOOL CColorScanDoc::Initialize()
{
    MCSTATUS Status;
    char pText[128];

    // Create the channel
    Status = McCreate(MC_CHANNEL, &m_Channel);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McCreate failed.", Status);
        return FALSE;
    }

    // Associate a board to the channel 
    Status = McSetParamInt(m_Channel, MC_DriverIndex, EURESYS_DRIVER_INDEX);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McSetParamInt MC_DriverIndex failed.", Status);
        return FALSE;
    }

    // Load a camera file
    Status = McSetParamStr(m_Channel, MC_CamFile, EURESYS_CAM_FILENAME);
    if (Status != MC_OK) 
    {
        sprintf_s(pText, 128,"Error: McSetParamStr MC_CamFile %s failed.", EURESYS_CAM_FILENAME);
        ErrorDisplay(pText, Status);
        return FALSE;
    }

    // Register the callback function
    Status = McRegisterCallback(m_Channel, GlobalCallbackFunction, this);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McRegisterCallback failed.", Status);
        return FALSE;
    }

    // Enable signals
    Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McSetParamInt MC_SignalEnable failed.", Status);
        return FALSE;
    }
    Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McSetParamInt MC_SignalEnable failed.", Status);
        return FALSE;
    }
    Status = McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_END_CHANNEL_ACTIVITY, MC_SignalEnable_ON);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McSetParamInt MC_SignalEnable failed.", Status);
        return FALSE;
    }

    // Retrieve image parameters
    Status = McGetParamInt(m_Channel, MC_ImageSizeX, &m_ImageSizeX_Px);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_ImageSizeX failed.", Status);
        return FALSE;
    }
    Status = McGetParamInt(m_Channel, MC_ImageSizeY, &m_ImageSizeY_Ln);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_ImageSizeY failed.", Status);
        return FALSE;
    }
    Status = McGetParamInt(m_Channel, MC_ColorFormat, &m_ColorFormat);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_ColorFormat failed.", Status);
        return FALSE;
    }

    // Plane information
    Status = McGetParamInt(m_Channel, MC_ImagePlaneCount, &m_PlaneCount);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_ImagePlaneCount failed.", Status);
        return FALSE;
    }
    Status = McGetParamInt(m_Channel, MC_BufferSize, &m_PlaneSize_Bytes);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_BufferSize failed.", Status);
        return FALSE;
    }
    Status = McGetParamInt(m_Channel, MC_BufferPitch, &m_SurfacePitch_Bytes);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McGetParamInt MC_BufferPitch failed.", Status);
        return FALSE;
    }

    // Initialize the LUTs
    if (!InitializeLUTs())
    {
        return FALSE;
    }

    // Set channel to IDLE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE); 
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
        return FALSE;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Termination

BOOL CColorScanDoc::Terminate()
{
    MCSTATUS Status;

    // Delete channel instance
    if (m_Channel != NULL)
    {
        Status = McDelete(m_Channel);
        if (Status != MC_OK) 
            ErrorDisplay("Error: McDelete m_Channel failed.", Status);
        m_Channel = NULL;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Acquisition control

BOOL CColorScanDoc::Grab()
{
    MCSTATUS Status;

    // Set Channel to IDLE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
        return FALSE;
    }

    // Set SeqLength_Pg to "1" or SeqLength_Ln to "500" -> one single acquisition
    Status = McGetParamInt(m_Channel, MC_AcquisitionMode, &m_AcquisitionMode); 
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot get AcquisitionMode.", Status);
        return FALSE;
    }
    if (m_AcquisitionMode == MC_AcquisitionMode_WEB)
    {
        Status = McSetParamInt(m_Channel, MC_SeqLength_Ln, 500); 
        if (Status != MC_OK) 
        {
            ErrorDisplay("Error: Cannot set SeqLength_Ln.", Status);
            return FALSE;
        }
    }
    else // MC_AcquisitionMode_PAGE or MC_AcquisitionMode_LONGPAGE
    {
        Status = McSetParamInt(m_Channel, MC_SeqLength_Pg, 1); 
        if (Status != MC_OK) 
        {
            ErrorDisplay("Error: Cannot set SeqLength_Pg.", Status);
            return FALSE;
        }
    }

    // Set Channel to ACTIVE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in ACTIVE state.", Status);
        return FALSE;
    }

    m_bActive = TRUE;

    return TRUE;
}

BOOL CColorScanDoc::Live()
{
    MCSTATUS Status;

    // Set Channel to IDLE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
        return FALSE;
    }

    // Set SeqLength_Pg to "MC_INDETERMINATE" or SeqLength_Ln to "MC_INDETERMINATE" -> live acquisitions
    Status = McGetParamInt(m_Channel, MC_AcquisitionMode, &m_AcquisitionMode); 
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot get AcquisitionMode.", Status);
        return FALSE;
    }
    if (m_AcquisitionMode == MC_AcquisitionMode_WEB)
    {
        Status = McSetParamInt(m_Channel, MC_SeqLength_Ln, MC_INDETERMINATE); 
        if (Status != MC_OK) 
        {
            ErrorDisplay("Error: Cannot set SeqLength_Ln.", Status);
            return FALSE;
        }
    }
    else // MC_AcquisitionMode_PAGE or MC_AcquisitionMode_LONGPAGE
    {
        Status = McSetParamInt(m_Channel, MC_SeqLength_Pg, MC_INDETERMINATE); 
        if (Status != MC_OK) 
        {
            ErrorDisplay("Error: Cannot set SeqLength_Pg.", Status);
            return FALSE;
        }
    }

    // Set Channel to ACTIVE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in ACTIVE state.", Status);
        return FALSE;
    }

    m_bActive = TRUE;

    return TRUE;
}

BOOL CColorScanDoc::StopLive()
{
    MCSTATUS Status;

    // Set Channel to IDLE state
    Status = McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot put channel in IDLE state.", Status);
        return FALSE;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Callback

void CColorScanDoc::CallBackFunction(MCCALLBACKINFO &CbInfo)
{
    if (CbInfo.Signal == MC_SIG_ACQUISITION_FAILURE)
    {
        AfxMessageBox("Acquisition failure.", MB_ICONERROR | MB_OK);
    }
    else if (CbInfo.Signal == MC_SIG_END_CHANNEL_ACTIVITY)
    {
        m_bActive = FALSE;
        m_bLive = FALSE;
    }
    else if (CbInfo.Signal == MC_SIG_SURFACE_PROCESSING)
    {
        // Find associated CCustomGrabFirstView
        POSITION pos = GetFirstViewPosition();
        CColorScanView *pView = (CColorScanView *)GetNextView(pos);
        if (pView == NULL)
            return;
        HWND m_hWindow2Update = pView->GetSafeHwnd();
        if (m_hWindow2Update == NULL)
            return;
        RECT RectImageSize = { 0 , 0 , m_ImageSizeX_Px - 1, m_PlaneCount * m_ImageSizeY_Ln - 1 };

        PUINT8 pData;
        MCSTATUS Status = McGetParamInt(CbInfo.SignalInfo, MC_SurfaceAddr, (PINT32)&pData);
        if (Status != MC_OK)
        {
            ErrorDisplay("Error: McGetParamInt MC_SurfaceAddr failed.", Status);
            return;
        }

        if (pData != NULL)
        {
            // image processing
        }

        m_DisplayImageBuffer = pData;

        if (!m_bOngoingRefresh) // Post refresh command
        {
            m_bOngoingRefresh = TRUE;
            ::InvalidateRect(m_hWindow2Update, &RectImageSize, FALSE);
        }
    }
}
