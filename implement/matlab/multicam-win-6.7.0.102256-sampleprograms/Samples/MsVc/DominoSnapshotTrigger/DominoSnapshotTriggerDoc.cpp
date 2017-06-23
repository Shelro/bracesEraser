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


// DominoSnapshotTriggerDoc.cpp : implementation of the CDominoSnapshotTriggerDoc class
//
#include "stdafx.h"
#include "DominoSnapshotTrigger.h"

#include "DominoSnapshotTriggerDoc.h"
#include "DominoSnapshotTriggerView.h"


#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDominoSnapshotTriggerDoc

IMPLEMENT_DYNCREATE(CDominoSnapshotTriggerDoc, CDocument)

BEGIN_MESSAGE_MAP(CDominoSnapshotTriggerDoc, CDocument)
    //{{AFX_MSG_MAP(CDominoSnapshotTriggerDoc)
    ON_COMMAND(ID_GO, OnGo)
    ON_COMMAND(ID_STOP, OnStop)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDominoSnapshotTriggerDoc construction/destruction

CDominoSnapshotTriggerDoc::CDominoSnapshotTriggerDoc()
{
    m_Channel = 0;
    m_pCurrent = NULL;
    m_SizeX = 0;
    m_SizeY = 0;
    m_BufferPitch = m_SizeX;
    m_bScreenRefreshCompleted = TRUE;
}

CDominoSnapshotTriggerDoc::~CDominoSnapshotTriggerDoc()
{
    // Set the channel to IDLE before deleting it
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);

    // Delete the channel
    McDelete(m_Channel);
}

BOOL CDominoSnapshotTriggerDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // + DominoSnapshotTrigger Sample Program

    // Show scope of the sample program
    MessageBox(NULL, 
        "This program demonstrates the Snapshot Acquisition Mode on a DOMINO Board.\n"
        "\n"
        "The \"Go!\" menu generates a soft trigger which starts a frame acquisition.\n"
        "By default, this program requires an area-scan camera connected on connector X.",
            "MultiCam sample description", MB_OK);


    // Set the board topology
    // Uncomment the folowing lines and set the board topology corresponding to your setup.
    //McSetParamInt(MC_BOARD + 0, MC_BoardTopology, MC_BoardTopology_1_1);

    // Create a channel and associate it with the first connector on the first board
    McCreate(MC_CHANNEL, &m_Channel);
    McSetParamInt(m_Channel, MC_DriverIndex, 0);
    McSetParamStr(m_Channel, MC_Connector, "X");
    // Choose the video standard
    McSetParamStr(m_Channel, MC_CamFile, "XC-HR50_P60RA");
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
    McSetParamInt(m_Channel, MC_TrigCtl, MC_TrigCtl_TTL);

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

    // - DominoSnapshotTrigger Sample Program
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// GlobalCallback

void WINAPI GlobalCallback(PMCSIGNALINFO SigInfo)
{
    if (SigInfo && SigInfo->Context)
    {
        CDominoSnapshotTriggerDoc* pDoc = (CDominoSnapshotTriggerDoc*) SigInfo->Context;
        pDoc->Callback (SigInfo);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Callback

void CDominoSnapshotTriggerDoc::Callback(PMCSIGNALINFO SigInfo)
{
    // + DominoSnapshotTrigger Sample Program

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
                CDominoSnapshotTriggerView *pView = (CDominoSnapshotTriggerView *)GetNextView(pos);
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
            MessageBox(NULL, "Acquisition Failure !", "DominoSnapshotTrigger", MB_OK);
            break;
        default:
            break;

    }
    // - DominoSnapshotTrigger Sample Program
}



/////////////////////////////////////////////////////////////////////////////
// CDominoSnapshotTriggerDoc commands

void CDominoSnapshotTriggerDoc::OnGo()
{
    // + DominoSnapshotTrigger Sample Program
    
    // Start an acquisition sequence by activating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);

    // Generate a soft trigger event (STRG)
    McSetParamInt(m_Channel, MC_ForceTrig, MC_ForceTrig_TRIG);
    
    // - DominoSnapshotTrigger Sample Program
}

void CDominoSnapshotTriggerDoc::OnStop()
{
    // + DominoSnapshotTrigger Sample Program
    
    // Stop an acquisition sequence by deactivating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    
    // - DominoSnapshotTrigger Sample Program
    
    UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CDominoSnapshotTriggerDoc serialization

void CDominoSnapshotTriggerDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CDominoSnapshotTriggerDoc diagnostics

#ifdef _DEBUG
void CDominoSnapshotTriggerDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDominoSnapshotTriggerDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
