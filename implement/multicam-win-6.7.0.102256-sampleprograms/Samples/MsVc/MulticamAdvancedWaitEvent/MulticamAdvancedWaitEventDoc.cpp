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


// MulticamAdvancedWaitEventDoc.cpp : implementation of the CMulticamAdvancedWaitEventDoc class
//
#include "stdafx.h"
#include "MulticamAdvancedWaitEvent.h"

#include "MulticamAdvancedWaitEventDoc.h"
#include "MulticamAdvancedWaitEventView.h"


#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMulticamAdvancedWaitEventDoc

IMPLEMENT_DYNCREATE(CMulticamAdvancedWaitEventDoc, CDocument)

BEGIN_MESSAGE_MAP(CMulticamAdvancedWaitEventDoc, CDocument)
    //{{AFX_MSG_MAP(CMulticamAdvancedWaitEventDoc)
    ON_COMMAND(ID_GO, OnGo)
    ON_COMMAND(ID_STOP, OnStop)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMulticamAdvancedWaitEventDoc construction/destruction

CMulticamAdvancedWaitEventDoc::CMulticamAdvancedWaitEventDoc()
{
    m_Channel = 0;
    m_pCurrent = NULL;
    m_SizeX = 0;
    m_SizeY = 0;
    m_BufferPitch = m_SizeX;
    m_bScreenRefreshCompleted = TRUE;
	m_RunLoop = FALSE;
}

CMulticamAdvancedWaitEventDoc::~CMulticamAdvancedWaitEventDoc()
{
    // Set the channel to IDLE before deleting it
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);

    // Delete the channel
    McDelete(m_Channel);
}

BOOL CMulticamAdvancedWaitEventDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // + MulticamAdvancedWaitEvent Sample Program

    // Show scope of the sample program
    MessageBox(NULL, 
        "This program demonstrates Multicam Advanced Acquisition Mode using Windows Events on an Euresys Board.\n"
        "\n"
        "The \"Go!\" menu starts an acquisition sequence by activating the channel.\n"
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

    // Choose the cam file
    McSetParamStr(m_Channel, MC_CamFile, "1000m_P50RG");
    // Choose the camera expose duration
    McSetParamInt(m_Channel, MC_Expose_us, 20000);
    // Choose the pixel color format
    McSetParamInt(m_Channel, MC_ColorFormat, MC_ColorFormat_Y8);

    // Set the acquisition mode to snapShot
    McSetParamInt(m_Channel, MC_AcquisitionMode, MC_AcquisitionMode_SNAPSHOT);
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
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_SURFACE_FILLED, MC_SignalEnable_ON);
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);
    McSetParamInt(m_Channel, MC_SignalEnable + MC_SIG_END_CHANNEL_ACTIVITY, MC_SignalEnable_ON);

	// Enable Multicam Signal Using Windows Event
    McSetParamInt(m_Channel, MC_SignalHandling + MC_SIG_SURFACE_FILLED, MC_SignalHandling_OS_EVENT_SIGNALING);
    McSetParamInt(m_Channel, MC_SignalHandling + MC_SIG_ACQUISITION_FAILURE, MC_SignalHandling_OS_EVENT_SIGNALING);
    McSetParamInt(m_Channel, MC_SignalHandling + MC_SIG_END_CHANNEL_ACTIVITY, MC_SignalHandling_OS_EVENT_SIGNALING);

    // - MulticamAdvancedWaitEvent Sample Program
    
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// GlobalLiveThread
UINT LiveThreadProc( LPVOID Param ) //Sample function for using in AfxBeginThread
{

	CMulticamAdvancedWaitEventDoc* pDoc = (CMulticamAdvancedWaitEventDoc*) Param;
	MCSIGNALINFO sigInfo ;
    
	// Define an array of 3 Handles.
	HANDLE pHandles[3];

	// Assignment of Handles
	McGetParamInt(pDoc->m_Channel, MC_SignalEvent + MC_SIG_SURFACE_FILLED, (int*)&pHandles[0]);
	McGetParamInt(pDoc->m_Channel, MC_SignalEvent + MC_SIG_ACQUISITION_FAILURE, (int*)&pHandles[1]);
	McGetParamInt(pDoc->m_Channel, MC_SignalEvent + MC_SIG_END_CHANNEL_ACTIVITY, (int*)&pHandles[2]);


	while(pDoc->m_RunLoop)
	{
		// Wait for any of the 3 signal	
		DWORD word = WaitForMultipleObjects(3, pHandles, FALSE, 1000);

		// Get the signal information
		switch(word)
		{

		case WAIT_OBJECT_0:
			McGetSignalInfo(pDoc->m_Channel, MC_SIG_SURFACE_FILLED, &sigInfo);
			pDoc->HandleSignal(&sigInfo);
			break;

		case WAIT_OBJECT_0 + 1:
			McGetSignalInfo(pDoc->m_Channel, MC_SIG_ACQUISITION_FAILURE, &sigInfo);
			pDoc->HandleSignal(&sigInfo);
			break;
			
		case WAIT_OBJECT_0 + 2:
			McGetSignalInfo(pDoc->m_Channel, MC_SIG_END_CHANNEL_ACTIVITY, &sigInfo);
			pDoc->HandleSignal(&sigInfo);
			break;

		case WAIT_TIMEOUT:
			break;
		}

	}


   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// HandleSignal

void CMulticamAdvancedWaitEventDoc::HandleSignal(PMCSIGNALINFO SigInfo)
{
    // + MulticamAdvancedWaitSignal Sample Program
	MCHANDLE hSurface = NULL;

    switch(SigInfo->Signal)
    {
	// Handles captured image 
        case MC_SIG_SURFACE_FILLED:
            // Update "current" surface address pointer
            McGetParamInt (SigInfo->SignalInfo, MC_SurfaceAddr, (PINT32) &m_pCurrent); 
			hSurface = (MCHANDLE)SigInfo->SignalInfo;
			McSetParamInt(hSurface,MC_SurfaceState,MC_SurfaceState_FREE);
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
                CMulticamAdvancedWaitEventView *pView = (CMulticamAdvancedWaitEventView *)GetNextView(pos);
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
		
		// Handle Acquisition errors
        case MC_SIG_ACQUISITION_FAILURE:
            MessageBox(NULL, "Acquisition Failure !", "MulticamAdvancedWaitSignal", MB_OK);
            break;

        // Terminate live thread gracefully 
        case MC_SIG_END_CHANNEL_ACTIVITY:
            m_RunLoop = FALSE;
            break;
        default:
            break;

    }
    // - MulticamAdvancedWaitSignal Sample Program
}

/////////////////////////////////////////////////////////////////////////////
// CMulticamAdvancedWaitEventDoc commands

void CMulticamAdvancedWaitEventDoc::OnGo()
{
    // + MulticamAdvancedWaitEvent Sample Program
    
	m_RunLoop = TRUE;
    AfxBeginThread(LiveThreadProc,(LPVOID)this,THREAD_PRIORITY_NORMAL,0,0,NULL);

    // Start an acquisition sequence by activating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_ACTIVE);
    
    // - MulticamAdvancedWaitEvent Sample Program
}

void CMulticamAdvancedWaitEventDoc::OnStop()
{
    // + MulticamAdvancedWaitEvent Sample Program
    
    // Stop an acquisition sequence by deactivating the channel
    McSetParamInt(m_Channel, MC_ChannelState, MC_ChannelState_IDLE);
    
    // - MulticamAdvancedWaitEvent Sample Program
    
    UpdateAllViews(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CMulticamAdvancedWaitEventDoc serialization

void CMulticamAdvancedWaitEventDoc::Serialize(CArchive& ar)
{
}

/////////////////////////////////////////////////////////////////////////////
// CMulticamAdvancedWaitEventDoc diagnostics

#ifdef _DEBUG
void CMulticamAdvancedWaitEventDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CMulticamAdvancedWaitEventDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
