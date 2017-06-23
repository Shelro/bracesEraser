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


// GrablinkSnapshotTriggerDoc.cpp : implementation of the CGrablinkSnapshotTriggerDoc class
//
#include "stdafx.h"
#include "GrablinkSnapshotTrigger.h"

#include "GrablinkSnapshotTriggerDoc.h"
#include "GrablinkSnapshotTriggerView.h"


#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrablinkSnapshotTriggerDoc

IMPLEMENT_DYNCREATE(CGrablinkSnapshotTriggerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGrablinkSnapshotTriggerDoc, CDocument)
    //{{AFX_MSG_MAP(CGrablinkSnapshotTriggerDoc)
    ON_COMMAND(ID_GO, OnGo)
    ON_COMMAND(ID_STOP, OnStop)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrablinkSnapshotTriggerDoc construction/destruction

CGrablinkSnapshotTriggerDoc::CGrablinkSnapshotTriggerDoc()
{
    m_Channel = 0;
    m_pCurrent = NULL;
    m_SizeX = 0;
    m_SizeY = 0;
    m_BufferPitch = m_SizeX;
    m_bScreenRefreshCompleted = TRUE;
}

CGrablinkSnapshotTriggerDoc::~CGrablinkSnapshotTriggerDoc()
{
    // Set the channel to IDLE before deleting it
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);

    // Delete the channel
    McDelete(m_Channel);
}

BOOL CGrablinkSnapshotTriggerDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // + GrablinkSnapshotTrigger Sample Program

    // Show scope of the sample program
    MessageBox(NULL, 
        "This program demonstrates the Snapshot Acquisition Mode on a GRABLINK Board.\n"
        "\n"
        "The \"Go!\" menu generates a soft trigger which starts a frame acquisition.\n"
        "By default, this program requires an area-scan camera connected on connector A.",
            "MultiCam sample description", MB_OK);


	// In order to support a 10-tap camera on Grablink Full
	// BoardTopology need to be set to MC_BoardTopology_MONO_DECA
	// In order to support 2 cameras on Grablink Expert 2
	// BoardTopology need to be set to MC_BoardTopology_DUO
	// In all other case the default value will work properly 
	// and the parameter don't need to be set

    // Set the board topology to support 2 cameras (only with a Grablink Expert 2)
    // McSetParamInt(MC_BOARD + 0, MC_BoardTopology, MC_BoardTopology_DUO);

	// Set the board topology to support 10 taps mode (only with a Grablink Full)
    // McSetParamInt(MC_BOARD + 0, MC_BoardTopology, MC_BoardTopology_MONO_DECA);

    // Create a channel and associate it with the first connector on the first board
    McCreate(MC_CHANNEL, &m_Channel);
    McSetParamInt(m_Channel, MC_DriverIndex, 0);

	// In order to use single camera on connector A
	// MC_Connector need to be set to A for Grablink Expert 2 and Grablink DualBase
	// For all the other Grablink boards the parameter has to be set to M  
	
	// For all GrabLink boards but Grablink Expert 2 and Dualbase
	McSetParamStr(m_Channel, MC_Connector, "M");
	// For Grablink Expert 2 and Dualbase
	// McSetParamStr(m_Channel, MC_Connector, "A");

	// Choose the video standard
    McSetParamStr(m_Channel, MC_CamFile, "1000m_P50RG");
    // Choose the camera expose duration
    McSetParamInt(m_Channel, MC_Expose_us, 20000);
    // Choose the pixel color format
    McSetParamInt(m_Channel, MC_ColorFormat, MC_ColorFormat_Y8);

    // Configure triggering mode
    McSetParamInt(m_Channel, MC_TrigMode, MC_TrigMode_COMBINED);
    McSetParamInt(m_Channel, MC_NextTrigMode, MC_NextTrigMode_COMBINED);

    // Configure triggering line
    // A rising edge on the triggering line generates a trigger.
    // See the TrigLine Parameter and the board documentation for more details.
    McSetParamInt(m_Channel, MC_TrigLine, MC_TrigLine_NOM);
    McSetParamInt(m_Channel, MC_TrigEdge, MC_TrigEdge_GOHIGH);
    McSetParamInt(m_Channel, MC_TrigFilter, MC_TrigFilter_ON);
	
	// Parameter valid for all Grablink but Full, DualBase, Base
	// McSetParamInt(m_Channel, MC_TrigCtl, MC_TrigCtl_ITTL);
	// Parameter valid only for Grablink Full, DualBase, Base
	McSetParamInt(m_Channel, MC_TrigCtl, MC_TrigCtl_ISO);

    // Choose the number of images to acquire
    McSetParamInt(m_Channel, MC_SeqLength_Fr, MC_INDETERMINATE);


    // Retrieve image dimensions
    McGetParamInt(m_Channel, MC_ImageSizeX, &m_SizeX);
    McGetParamInt(m_Channel, MC_ImageSizeY, &m_SizeY);
    McGetParamInt(m_Channel, MC_BufferPitch, &m_BufferPitch);

    // The memory allocation for the images is automatically done by Multicam when activating the channel.
    // We only set the number of surfaces to be created by MultiCam.
    McSetParamInt(m_Channel, MC_SurfaceCount, EURESYS_SURFACE_COUNT);

    // Enable MultiCam signals
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

    // Register the callback function
    McRegisterCallback(m_Channel, GlobalCallback, this);

    // - GrablinkSnapshotTrigger Sample Program
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// GlobalCallback

void WINAPI GlobalCallback(PMCSIGNALINFO SigInfo)
{
    if (SigInfo && SigInfo->Context)
    {
        CGrablinkSnapshotTriggerDoc* pDoc = (CGrablinkSnapshotTriggerDoc*) SigInfo->Context;
        pDoc->Callback (SigInfo);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Callback

void CGrablinkSnapshotTriggerDoc::Callback(PMCSIGNALINFO SigInfo)
{
    // + GrablinkSnapshotTrigger Sample Program

    switch(SigInfo->Signal)
    {
        case MC_SIG_SURFACE_PROCESSING:
            // Update "current" surface address pointer
            McGetParamInt (SigInfo->SignalInfo, MC_SurfaceAddr, (PINT32) &m_pCurrent); 

            //----------------------------------------
            //
            // Insert the eVision code here.
            //
            //----------------------------------------

            // Post screen refresh message only if previous refresh completed
            if(m_bScreenRefreshCompleted)
            {
                m_bScreenRefreshCompleted = FALSE;
                POSITION pos = GetFirstViewPosition();
                CGrablinkSnapshotTriggerView *pView = (CGrablinkSnapshotTriggerView *)GetNextView(pos);
                if (pView == NULL) return;

                HWND hWindow = pView->GetSafeHwnd();
                if (hWindow == NULL) return;

                RECT recpict ;
                recpict.left =0;
                recpict.top =0;
                recpict.right = m_SizeX - 1;
                recpict.bottom = m_SizeY - 1;
                InvalidateRect(hWindow,&recpict,FALSE);
            }
            break;
        case MC_SIG_ACQUISITION_FAILURE:
            MessageBox(NULL, "Acquisition Failure !", "GrablinkSnapshotTrigger", MB_OK);
            break;
        default:
            break;

    }
    // - GrablinkSnapshotTrigger Sample Program
}



/////////////////////////////////////////////////////////////////////////////
// CGrablinkSnapshotTriggerDoc commands

void CGrablinkSnapshotTriggerDoc::OnGo()
{
    // + GrablinkSnapshotTrigger Sample Program
    
    // Start an acquisition sequence by activating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);

    // Generate a soft trigger event (STRG)
    McSetParamInt(m_Channel, MC_ForceTrig, MC_ForceTrig_TRIG);
    
    // - GrablinkSnapshotTrigger Sample Program
}

void CGrablinkSnapshotTriggerDoc::OnStop()
{
    // + GrablinkSnapshotTrigger Sample Program
    
    // Stop an acquisition sequence by deactivating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    
    // - GrablinkSnapshotTrigger Sample Program
    
    UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CGrablinkSnapshotTriggerDoc serialization

void CGrablinkSnapshotTriggerDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CGrablinkSnapshotTriggerDoc diagnostics

#ifdef _DEBUG
void CGrablinkSnapshotTriggerDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CGrablinkSnapshotTriggerDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
