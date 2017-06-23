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
// ColorScan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ColorScan.h"

#include "MainFrm.h"
#include "ColorScanDoc.h"
#include "ColorScanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorScanApp

BEGIN_MESSAGE_MAP(CColorScanApp, CWinApp)
    //{{AFX_MSG_MAP(CColorScanApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorScanApp construction

CColorScanApp::CColorScanApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CColorScanApp object

CColorScanApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CColorScanApp initialization

BOOL CColorScanApp::InitInstance()
{
    MCSTATUS Status;

    // Establish communication with the driver
    Status = McOpenDriver(NULL);
    if (Status != MC_OK)
    {
        char Txt[512];
        sprintf_s(Txt,512, "Error: McOpenDriver failed. Return value = %d", Status);
        AfxMessageBox(Txt, MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // Enable the virtual COM port
    Status = McSetParamStr(MC_BOARD + EURESYS_DRIVER_INDEX, MC_SerialControlA, EURESYS_VIRTUAL_COM_PORT);
    if (Status != MC_OK)
    {
        char Txt[512];
        sprintf_s(Txt,512, "Error: McSetParamStr failed. Return value = %d", Status);
        AfxMessageBox(Txt, MB_ICONERROR | MB_OK);
        return FALSE;
    }

    char pText[512];
    sprintf_s(pText, 512, "%s\n"
        "This program will use the following settings:\n\n"
        "Driver index: %i\n"
        "Camera File: \"%s\"\n\n"
        , EURESYS_SAMPLE_PROGRAM_NAME
        , EURESYS_DRIVER_INDEX
        , EURESYS_CAM_FILENAME);
    AfxMessageBox(pText, MB_OK|MB_ICONINFORMATION);

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.
// old MFC
	/*
#ifdef _AFXDLL
    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/
    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CColorScanDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CColorScanView));
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->SetWindowText(EURESYS_SAMPLE_PROGRAM_WINDOW_TITLE);
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CColorScanApp termination

int CColorScanApp::ExitInstance() 
{
    MCSTATUS Status;

    // Terminate communication with the driver
    Status = McCloseDriver();
    if (Status != MC_OK)
    {
        char Txt[512];
        sprintf_s(Txt,512, "Error: McCloseDriver failed. Return value = %d", Status);
        AfxMessageBox(Txt, MB_ICONERROR | MB_OK);
    }

    return CWinApp::ExitInstance();
}



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CColorScanApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CColorScanApp message handlers

