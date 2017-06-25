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
#pragma once
#include "std.h"
// From the driver.
//
#include "..\..\Include\ESFilter.h"

// {B87BEB7B-8D29-423F-AE4D-6582C10175AC}
DEFINE_GUID(CLSID_VideoRenderer,
0xB87BEB7B, 0x8D29, 0x423F, 0xAE, 0x4D, 0x65, 0x82, 0xC1, 0x01, 0x75, 0xAC); //quartz.dll



class CPicoloDS
{
// Functions
public:
	CPicoloDS(void);
	~CPicoloDS(void);
	// Initialize graph for compressed acquisition and recording
	HRESULT Initialize(TCHAR * FileName, HWND hWnd, UINT32 VideoInput=0 ,UINT32 BoardID=0, ESF_RESOLUTION Scaler = ESF_RESOLUTION_FRAME );
	// Start compressed recorder graph
	HRESULT Start(void);
	// Stop compressed recorder graph
	HRESULT Stop(void);
	// Set Video Standard to be used
	HRESULT SetVideoStandard(ESF_VIDEO_STANDARD VideoStd);
	// Redraw the renderer on the target window when needed
	HRESULT Paint(void);

	// Internal functions
private:
	// Build graph
	HRESULT BuildGraph();
	// Initialize the renderer windowless mode
	HRESULT InitWindowlessVMR();


// Variables
private:
	// Name of the file to be recorded
	TCHAR m_wcsFileTitle[1024];
	// Handle to the target display window
	HWND m_hWnd;
	// Rectangle defining the source and destination for the windowless display
	RECT m_rcSrc, m_rcDest; 

	// Picolo ID
	UINT32 m_BoardID;
	// Picolo Connector
	UINT32 m_VideoInput;
	// Picolo Scaler
	ESF_RESOLUTION m_Scaler;
	// 	Picolo Video standard
	ESF_VIDEO_STANDARD m_VidStd;


	// DShow graph
	CComPtr<IGraphBuilder> m_pGraph;
	// AVI Mux
    CComPtr<IBaseFilter> m_pAVIMux;
	// Euresys source filter
	CComPtr<IBaseFilter> m_pESFilter;
	// Dshow windowless control 
	CComPtr<IVMRWindowlessControl> m_pWCtl;
	// Dshow renderer filter
	CComPtr<IBaseFilter> m_pVMR; 

	// Graph initialization success
	bool m_bIsInit;
	// Graph Running
	bool m_bIsRunning;


};
