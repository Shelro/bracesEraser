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


// PicoloVideoDoc.cpp : implementation of the CPicoloVideoDoc class
//
#include "stdafx.h"
#include "PicoloVideo.h"

#include "PicoloVideoDoc.h"
#include "PicoloVideoView.h"


#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicoloVideoDoc

IMPLEMENT_DYNCREATE(CPicoloVideoDoc, CDocument)

BEGIN_MESSAGE_MAP(CPicoloVideoDoc, CDocument)
    //{{AFX_MSG_MAP(CPicoloVideoDoc)
    ON_COMMAND(ID_GO, OnGo)
    ON_COMMAND(ID_STOP, OnStop)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicoloVideoDoc construction/destruction

CPicoloVideoDoc::CPicoloVideoDoc()
{
    m_Channel = 0;
    m_pCurrent = NULL;
    m_SizeX = 0;
    m_SizeY = 0;
    m_BufferPitch = m_SizeX;
    m_bScreenRefreshCompleted = TRUE;
	m_bChannelActive = FALSE;
}

CPicoloVideoDoc::~CPicoloVideoDoc()
{
    // Set the channel to IDLE before deleting it
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);

	// wait until the channel has performed the last acquisition
	while (m_bChannelActive)
		Sleep(10);
    // Delete the channel
    McDelete(m_Channel);
}

BOOL CPicoloVideoDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // + PicoloVideo Sample Program

    // Show scope of the sample program
    MessageBox(NULL, 
        "This program demonstrates the Video Acquisition Mode on a Picolo Board.\n"
        "\n"
        "The \"Go!\" menu starts an acquisition sequence by activating the channel.\n"
        "By default, this program requires a PAL camera connected on VID1.",
            "MultiCam sample description", MB_OK);


    // Create a channel and associate it with the first connector on the first board
    McCreate(MC_CHANNEL, &m_Channel);
    McSetParamInt(m_Channel, MC_DriverIndex, 0);
    McSetParamStr(m_Channel, MC_Connector, "VID1");
    // Choose the video standard
    McSetParamStr(m_Channel, MC_CamFile, "PAL");
    // Choose the pixel color format
    McSetParamInt(m_Channel, MC_ColorFormat, MC_ColorFormat_RGB24);

    // Choose the acquisition mode
    McSetParamInt(m_Channel, MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
    // Choose the way the first acquisition is triggered
    McSetParamInt(m_Channel, MC_TrigMode, MC_TrigMode_IMMEDIATE);
    // Choose the triggering mode for subsequent acquisitions
    McSetParamInt(m_Channel, MC_NextTrigMode, MC_NextTrigMode_REPEAT);
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
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_END_CHANNEL_ACTIVITY, MC_SignalEnable_ON);

    // Register the callback function
    McRegisterCallback(m_Channel, GlobalCallback, this);

    // - PicoloVideo Sample Program
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// GlobalCallback

void WINAPI GlobalCallback(PMCSIGNALINFO SigInfo)
{
    if (SigInfo && SigInfo->Context)
    {
        CPicoloVideoDoc* pDoc = (CPicoloVideoDoc*) SigInfo->Context;
        pDoc->Callback (SigInfo);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Callback

void CPicoloVideoDoc::Callback(PMCSIGNALINFO SigInfo)
{
    // + PicoloVideo Sample Program

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
                CPicoloVideoView *pView = (CPicoloVideoView *)GetNextView(pos);
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
        case MC_SIG_END_CHANNEL_ACTIVITY:
			    m_bChannelActive = FALSE; 
            break;
        case MC_SIG_ACQUISITION_FAILURE:
            MessageBox(NULL, "Acquisition Failure !", "PicoloVideo", MB_OK);
            break;
        default:
            break;

    }
    // - PicoloVideo Sample Program
}



/////////////////////////////////////////////////////////////////////////////
// CPicoloVideoDoc commands

void CPicoloVideoDoc::OnGo()
{
    // + PicoloVideo Sample Program
    

    // Start an acquisition sequence by activating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
    m_bChannelActive = TRUE; 
    // - PicoloVideo Sample Program
}

void CPicoloVideoDoc::OnStop()
{
    // + PicoloVideo Sample Program
    
    // Stop an acquisition sequence by deactivating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    
    // - PicoloVideo Sample Program
    
    UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CPicoloVideoDoc serialization

void CPicoloVideoDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPicoloVideoDoc diagnostics

#ifdef _DEBUG
void CPicoloVideoDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPicoloVideoDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
