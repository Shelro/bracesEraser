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

#include "Std.h"

// User mode API.
//

#include "Helper.h"
//////////////////////
// DS Helper functions
//////////////////////

//change this macro to fit your style of error handling
#define CHECK_HR(hr, msg) if (hrcheck(hr, msg)) return hr;

BOOL hrcheck(HRESULT hr, TCHAR* errtext)
{
    if (hr >= S_OK)
        return FALSE;

    TCHAR szErr[MAX_ERROR_TEXT_LEN];
    TCHAR Buffer[MAX_PATH];
    DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);
    if (res) {
        StringCchPrintf((TCHAR*)Buffer, sizeof( Buffer )/sizeof(TCHAR), _T("Error %x: %s\n%s\n"), hr, errtext,szErr);
        MessageBox( NULL, Buffer, _T("Picolo DirectShow"), MB_OK );
    }
    else {
        StringCchPrintf((TCHAR*)Buffer, sizeof( Buffer )/sizeof(TCHAR), _T("Error %x: %s\n"), hr, errtext);
        MessageBox( NULL, Buffer, _T("Picolo DirectShow"), MB_OK );
    }
    return TRUE;
}

// Create a filter based on name and category.
//
HRESULT CreateFilterEx(const WCHAR *filterName, IBaseFilter **filter, REFCLSID filterCategory, ULONG instance)
{
    HRESULT hr;    

    // Create the system device enumerator.
    CComPtr<ICreateDevEnum> devenum;
    hr = devenum.CoCreateInstance(CLSID_SystemDeviceEnum);
    if (FAILED(hr))
        return hr;

    // Create an enumerator for this category.
    CComPtr<IEnumMoniker> classenum;
    hr = devenum->CreateClassEnumerator( filterCategory, &classenum, 0 );
    if (hr != S_OK)
        return hr;

   // Enumerate all the devices in that category looking for two matches:
   // 1. Name
   // 2. Instance
   //
    CComVariant name( filterName );
    CComPtr<IMoniker> moniker;
    ULONG FoundCount = 0;
    while (classenum->Next(1, &moniker, 0) == S_OK)
    {
        CComPtr<IPropertyBag> properties;
        hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&properties);
        if (FAILED(hr))
            return hr;

        CComVariant friendlyname;
        hr = properties->Read(L"FriendlyName", &friendlyname, 0);
        if (!FAILED(hr)) {
            if (name == friendlyname)
            {
                // Found the name. Need to check if the instance is correct.
                //
                FoundCount++;
                if ( ( FoundCount - 1 ) == instance ) {
                    // Instance is correct.
                    //
                    hr = moniker->BindToObject( 0, 0, IID_IBaseFilter, (void **)filter );
                    return hr;
                }
            }    
        }

        moniker.Release();
    }    

    return E_FAIL;
}
// Create a filter based on name VfW.
//

CComPtr<IBaseFilter> CreateFilter(WCHAR* displayName)
{
    CComPtr<IBindCtx> pBindCtx;
    HRESULT hr = CreateBindCtx(0, &pBindCtx);
    if (hrcheck(hr, L"Can't create bind context"))
        return NULL;

    ULONG chEaten = 0;
    CComPtr<IMoniker> pMoniker;
    hr = MkParseDisplayName(pBindCtx, displayName, &chEaten, &pMoniker);
    if (hrcheck(hr, L"Can't create parse display name of the filter"))
        return NULL;

    CComPtr<IBaseFilter> pFilter;
    if (SUCCEEDED(hr))
    {
        hr = pMoniker->BindToObject(pBindCtx, NULL, IID_IBaseFilter, (void**)&pFilter);
        if (hrcheck(hr, L"Can't bind moniker to filter object"))
            return NULL;
    }
    return pFilter;
}


// Add a filter to a graph based on name and category ofthe filter in human readable form.
//
HRESULT AddFilter(IFilterGraph *graph, 
                  const WCHAR *name, 
                  IBaseFilter **filter, 
                  REFCLSID filterCategory, 
                  const WCHAR *nameInGraph,
                  ULONG instance)
{
    if ( CreateFilterEx( name, filter, filterCategory, instance ) != S_OK ) {
        return E_FAIL;   
    }

    if (FAILED( graph->AddFilter( *filter, nameInGraph ) ) ) {
        (*filter)->Release();
        *filter = 0;
        return E_FAIL;   
    }   

    return S_OK;
}   



CComPtr<IPin> GetPin( IBaseFilter *pFilter, LPCOLESTR pinname )
{
    CComPtr<IEnumPins>  pEnum;
    CComPtr<IPin>       pPin;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (hrcheck(hr, _T("Can't enumerate pins.")))
        return NULL;

    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_INFO pinfo;
        pPin->QueryPinInfo(&pinfo);
        BOOL found = !_wcsicmp(pinname, pinfo.achName);
        if (pinfo.pFilter) pFilter->Release();
        if (found)
            return pPin;
        pPin.Release();
    }
    return NULL;  
}

////////////////////
// Picolo Help class
////////////////////

CPicoloDS::CPicoloDS(void)
{
	// Initialize the COM library
	CoInitialize(NULL);
	
	// Initialize variables
	m_bIsInit		= false;
	m_bIsRunning	= false;

}


CPicoloDS::~CPicoloDS(void)
{
	// Stop running graph if needed
	Stop();

	// Close the COM library
	CoUninitialize();

}

// Initialize graph for compressed acquisition and recording
HRESULT CPicoloDS::Initialize(TCHAR * FileName, HWND hWnd, UINT32 VideoInput, UINT32 BoardID, ESF_RESOLUTION Scaler )
{

	// Save name of the file to be recorded
	StringCchCopy(m_wcsFileTitle,sizeof(m_wcsFileTitle)/sizeof(TCHAR),FileName);

	// Save config data
	m_BoardID = BoardID;
	m_VideoInput = VideoInput;
	m_Scaler = Scaler;
	m_hWnd = hWnd;

	// Create graph
	m_pGraph.CoCreateInstance(CLSID_FilterGraph);

    // Build graph
	HRESULT hr = BuildGraph();
    
	// Building succeeds => graph is initialized and ready to run
	if (hr==S_OK) m_bIsInit= true;

   
    return hr;
}


// Start compressed recorder graph
HRESULT CPicoloDS::Start(void)
{
	HRESULT hr;
	// Start only if the graph is initialized and if it is not running
	if ( m_bIsInit && !m_bIsRunning)
	{
		// Get the media control
		CComQIPtr<IMediaControl, &IID_IMediaControl> mediaControl(m_pGraph);
        
		// Run the graph
		hr = mediaControl->Run();
		
		// Graph is running
		if (hr == S_OK) m_bIsRunning = true;

		// Graph will start soon
		if (hr == S_FALSE)
		{
			OAFilterState fs;
			// Check the state of the graph
			hr = mediaControl->GetState(500,&fs);
			// Wait untill we know the state of the graph
			while ( hr == VFW_S_STATE_INTERMEDIATE)
			{
				hr = mediaControl->GetState(500,&fs);
			}
			if (hr == S_OK)
			{
				// Make sure the graph is running
				if ( fs ==  State_Running)
				{
					// Graph has finally started
					m_bIsRunning = true;
				}
				else 
				{
					// Graph is still not running
					hr = E_ABORT;
				}
			}
		}
	}
	else
	{
		// Not allowed to start, graph not initialized
		hr = E_ABORT;
	}
	CHECK_HR(hr,L"Failed to start the graph");
	return hr;

}

// Stop Compressed Recorder graph
HRESULT CPicoloDS::Stop(void)
{
	HRESULT hr;
	// Stop only if the graph is running
	if (m_bIsRunning)
	{
		// Get the media control
		CComQIPtr<IMediaControl, &IID_IMediaControl> mediaControl(m_pGraph);
        
		// Run the graph
		hr = mediaControl->Stop();
		
		// Graph is stopped
		if (hr == S_OK) m_bIsRunning = false;
	}
	else
	{
		// Not allowed to access an uninitialized graph
		hr = E_ABORT;
	}
	CHECK_HR(hr,L"Failed to stop the graph");
	return hr;}




// Build recording graph
HRESULT CPicoloDS::BuildGraph()
{
	HRESULT hr = S_OK;

    // Create graph builder
    CComPtr<ICaptureGraphBuilder2> pBuilder;
    hr = pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    CHECK_HR(hr, L"Can't create Capture Graph Builder");
    hr = pBuilder->SetFiltergraph(m_pGraph);
    CHECK_HR(hr, L"Can't SetFiltergraph");

    // Add Euresys Source Filter
    hr = m_pESFilter.CoCreateInstance(CLSID_ESFilter);
    CHECK_HR(hr, L"Can't create Euresys Source Filter");
    hr = m_pGraph->AddFilter(m_pESFilter, L"Euresys Source Filter");
    CHECK_HR(hr, L"Can't add Euresys Source Filter to graph");

	////////////////////////////////
	// INIT Euresys Source Filter //
	////////////////////////////////

	// Obtain IESFilterProperties interface
	IESFProperties *pESFProp = NULL;
	hr = m_pESFilter->QueryInterface(IID_IESFProperties, reinterpret_cast<void**>(&pESFProp));
	CHECK_HR(hr, L"Failed to QueryInterface IESFProperties");

	// Set Board 
	hr = pESFProp->SetBoard(m_BoardID);
	CHECK_HR(hr, L"Failed to set Board IESFProperties");

	// Set Video input 
	hr = pESFProp->SetConnector(m_VideoInput);
	CHECK_HR(hr, L"Failed to set VideoInput IESFProperties");

	// Set Video Standard
	hr = pESFProp->SetVideoStandard(m_VidStd, false);
	CHECK_HR(hr, L"Failed to set VideoStandard IESFProperties");

	// Set Video Size
	hr = pESFProp->SetResolution(m_Scaler, 512, 512); // 512*512 in case of custom size is defined
	CHECK_HR(hr, L"Failed to set Resolution IESFProperties");

	pESFProp->Release();

	//////////////
	// RAW path //
	//////////////

    // Add Infinite Pin Tee Filter
    CComPtr<IBaseFilter> pInfinitePinTeeFilter;
    hr = pInfinitePinTeeFilter.CoCreateInstance(CLSID_InfTee);
    CHECK_HR(hr, L"Can't create Infinite Pin Tee Filter");
    hr = m_pGraph->AddFilter(pInfinitePinTeeFilter, L"Infinite Pin Tee Filter");
    CHECK_HR(hr, L"Can't add Infinite Pin Tee Filter to graph");

    // Connect Euresys Source Filter and Infinite Pin Tee Filter
    hr = m_pGraph->Connect(GetPin(m_pESFilter, L"~Capture"), GetPin(pInfinitePinTeeFilter, L"Input"));
    CHECK_HR(hr, L"Can't connect Euresys Source Filter and Infinite Pin Tee Filter");

	// Make windowless VMR
	hr =  InitWindowlessVMR();
	CHECK_HR(hr, L"Failed to initialize Windowless mode");

    // Render Infinite Tee #1 Pin
    hr = m_pGraph->Connect(GetPin(pInfinitePinTeeFilter, L"Output1"), GetPin(m_pVMR, L"VMR Input0"));
	CHECK_HR(hr, L"Can't render Infinite Tee #1 Pin");

	/////////////////////
	// COMPRESSED path //
	/////////////////////

	// Add ffdshow video encoder
    CComPtr<IBaseFilter> pffdshowvideoencoder = CreateFilter(L"@device:sw:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\ffdshow video encoder");
    hr = m_pGraph->AddFilter(pffdshowvideoencoder, L"ffdshow video encoder");
    CHECK_HR(hr, L"Can't add ffdshow video encoder to graph");

	
	// Display ffdshow property page
	ISpecifyPropertyPages *pProp;
	hr = pffdshowvideoencoder->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
    CHECK_HR(hr, L"Can't retrieve ffdshow property page");

	// Get the filter's name and IUnknown pointer.
	FILTER_INFO FilterInfo;
	hr = pffdshowvideoencoder->QueryFilterInfo(&FilterInfo); 
	IUnknown *pFilterUnk;
	pffdshowvideoencoder->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

	// Show the page. 
	CAUUID caGUID;
	pProp->GetPages(&caGUID);
	pProp->Release();
	OleCreatePropertyFrame(
			m_hWnd,                   // Parent window
			0, 0,                   // Reserved
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
	);
	// Clean up.
	pFilterUnk->Release();
	FilterInfo.pGraph->Release(); 
	CoTaskMemFree(caGUID.pElems);

    // Connect Infinite Pin Tee Filter and ffdshow video encoder
    hr = m_pGraph->Connect(GetPin(pInfinitePinTeeFilter, L"Output2"), GetPin(pffdshowvideoencoder, L"In"));
    //hr = m_pGraph->Connect(GetPin(pInfinitePinTeeFilter, L"Output2"), GetPin(pffdshowvideoencoder, L"Input"));
    CHECK_HR(hr, L"Can't connect Infinite Pin Tee Filter and ffdshow video encoder");

	// Add AVI Mux
	hr = m_pAVIMux.CoCreateInstance(CLSID_AviDest);
	CHECK_HR(hr, L"Can't create AVI Mux");
	hr = m_pGraph->AddFilter(m_pAVIMux, L"AVI Mux");
	CHECK_HR(hr, L"Can't add AVI Mux to graph");

    // Connect ffdshow video encoder and AVI Mux
    hr = m_pGraph->Connect(GetPin(pffdshowvideoencoder, L"Out"), GetPin(m_pAVIMux, L"Input 01"));
    //hr = m_pGraph->Connect(GetPin(pffdshowvideoencoder, L"Output"), GetPin(m_pAVIMux, L"Input 01"));
    CHECK_HR(hr, L"Can't connect ffdshow video encoder and AVI Mux");

	// Add File writer
	CComPtr<IBaseFilter> pFilewriter;
	hr = pFilewriter.CoCreateInstance(CLSID_FileWriter);
	CHECK_HR(hr, L"Can't create File writer");
	hr = m_pGraph->AddFilter(pFilewriter, L"File writer");
	CHECK_HR(hr, L"Can't add File writer to graph");
	
	// Set destination filename
	CComQIPtr<IFileSinkFilter, &IID_IFileSinkFilter> pFilewriter_sink(pFilewriter);
	if (!pFilewriter_sink)
		CHECK_HR(E_NOINTERFACE, L"Can't get IFileSinkFilter");
	hr = pFilewriter_sink->SetFileName(m_wcsFileTitle, NULL);
	CHECK_HR(hr, L"Can't set filename");

	// Connect AVI Mux and File writer
	hr = m_pGraph->Connect(GetPin(m_pAVIMux, L"AVI Out"), GetPin(pFilewriter, L"in"));
	CHECK_HR(hr, L"Can't connect AVI Mux and File writer");

	return S_OK;
}

// Init Windowless Video renderer

HRESULT CPicoloDS::InitWindowlessVMR() 
{ 
	HRESULT hr;
	
	// Create Video Mixing Renderer
    hr = m_pVMR.CoCreateInstance(CLSID_VideoMixingRenderer);
    CHECK_HR(hr, L"Can't create VMR");

	// Add Video Mixing and Renderer to the graph
	hr = m_pGraph->AddFilter(m_pVMR, L"Video Renderer");
    CHECK_HR(hr, L"Can't add VMR to graph");

	// Get VMR Config interface
	CComPtr<IVMRFilterConfig> pConfig; 
	hr = m_pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	CHECK_HR(hr,L"Failed to retrieve VMR Config interface");

	// Set VMR mode to windowless
	hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
	CHECK_HR(hr,L"Failed to set VMR to windoless mode");
	
	// Get the windowles control 
	hr = m_pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&m_pWCtl);
	CHECK_HR(hr,L"Failed to retrieve windowless control");
		
	// Set the VMR clipping window
	hr = m_pWCtl->SetVideoClippingWindow(m_hWnd); 
	CHECK_HR(hr,L"Failed to set the clipping window");

	// Find the native video size.
	long lWidth, lHeight; 
	hr = m_pWCtl->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL); 
	CHECK_HR(hr,L"Failed to retrieve client rectangle\n");

	// Set the source rectangle.
	SetRect(&m_rcSrc, 0, 0, lWidth, lHeight); 

	// Get the window client area.
	GetClientRect(m_hWnd, &m_rcDest); 

	// Set the destination rectangle.
	SetRect(&m_rcDest, 0, 0, m_rcDest.right, m_rcDest.bottom); 

	// Set the video position.
	hr = m_pWCtl->SetVideoPosition(&m_rcSrc, &m_rcDest); 
	CHECK_HR(hr,L"Failed to set video position\n");

	return hr; 
} 

// Redraw the renderer on the target window when needed
HRESULT CPicoloDS::Paint(void)
{
	HRESULT hr = S_OK;

	HDC hdc = GetDC(m_hWnd);

	if (m_bIsInit)
	{
        // Find the region where the application can paint by subtracting 
        // the video destination rectangle from the client area.
        // (Assume that g_rcDest was calculated previously.)
        HRGN rgnClient = CreateRectRgnIndirect(&m_rcDest); 
        HRGN rgnVideo  = CreateRectRgnIndirect(&m_rcSrc);  
        CombineRgn(rgnClient, rgnClient, rgnVideo, RGN_DIFF);  
        
        // Paint on window.
        HBRUSH hbr = GetSysColorBrush(COLOR_BTNFACE); 
        FillRgn(hdc, rgnClient, hbr); 

        // Clean up.
        DeleteObject(hbr); 
        DeleteObject(rgnClient); 
        DeleteObject(rgnVideo); 

        // Request the VMR to paint the video.
        hr = m_pWCtl->RepaintVideo(m_hWnd, hdc);  
		CHECK_HR(hr,L"Failed to display video on target window");

    } 
    else  // There is no video, so paint the whole client area. 
    { 
        FillRect(hdc, &m_rcDest, (HBRUSH)(COLOR_BTNFACE + 1)); 
    }

	return hr;
}


// Set Video Standard to be used
HRESULT CPicoloDS::SetVideoStandard(ESF_VIDEO_STANDARD VideoStd)
{
	if (VideoStd>2)
		return E_NOTIMPL;
	else
		m_VidStd = VideoStd;

	return S_OK;

}

