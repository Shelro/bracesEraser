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
// Luts.cpp
//

#include "stdafx.h"
#include "ColorScan.h"

#include "ColorScanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Shading corrector commands

void CColorScanDoc::OnSHC() 
{
    m_bSHCEnabled = !m_bSHCEnabled;

    if (m_bSHCEnabled)
        SHCUse(1);
    else
        SHCUse(0);
}

void CColorScanDoc::OnUpdateSHC(CCmdUI *pCmdUI) 
{
    pCmdUI->SetCheck(m_bSHCEnabled);
    pCmdUI->Enable(m_bSHCCalibrated);
}

void CColorScanDoc::OnSHCCalibration() 
{
    SHCCalibration(1);
}

void CColorScanDoc::OnUpdateSHCCalibration(CCmdUI *pCmdUI) 
{
    pCmdUI->Enable(!m_bActive);
}


/////////////////////////////////////////////////////////////////////////////
// Shading corrector setup

BOOL CColorScanDoc::SHCUse(int Index)
{
    MCSTATUS Status;

    Status = McSetParamInt(m_Channel, MC_SHC_UseIndex, Index);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot set SHC:UseIndex", Status);
        return FALSE;
    }

    return TRUE;
}


BOOL CColorScanDoc::SHCCalibration(int Index)
{
    MCSTATUS Status;
    int ret;
    MCHANDLE hDark, hLight;
    PFLOAT32 darkProfileBuffer, lightProfileBuffer;

    hDark = NULL;
    hLight = NULL;
    darkProfileBuffer = NULL;
    lightProfileBuffer = NULL;

    ret = AfxMessageBox("Click on OK to acquire the dark background, CANCEL to use a uniform background (0).", MB_OKCANCEL);
    if (ret == IDOK)
    {		
        if (!CreateSurfaceForProfile(hDark, darkProfileBuffer))
            goto error;
        if (!BuildProfile(darkProfileBuffer))
            goto error;
    }

    ret = AfxMessageBox("Click on OK to acquire the light background, CANCEL to use a uniform background (1).", MB_OKCANCEL);
    if (ret == IDOK)
    {
        if (!CreateSurfaceForProfile(hLight, lightProfileBuffer))
            goto error;
        if (!BuildProfile(lightProfileBuffer))
            goto error;
    }

    Status = McSetParamInt(m_Channel, MC_SHC_DarkBackground, hDark);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot set SHC:DarkBackground", Status);
        goto error;
    }

    Status = McSetParamInt(m_Channel, MC_SHC_LightBackground, hLight);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot set SHC:LightBackground", Status);
        goto error;
    }

    Status = McSetParamInt(m_Channel, MC_SHC_StoreIndex, Index);
    if (Status == MC_OVERFLOW)
    {
        Status = MC_OK;
        AfxMessageBox("WARNING: overflow", MB_OK);
    }
    else if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot set SHC:StoreIndex", Status);
        goto error;
    }

    m_bSHCCalibrated = TRUE;

error:
    // Delete everything
    if (hDark != NULL)
        McDelete(hDark);
    if (darkProfileBuffer != NULL)
        VirtualFree(darkProfileBuffer, 0, MEM_RELEASE);
    if (hLight != NULL)
        McDelete(hLight);
    if (lightProfileBuffer != NULL)
        VirtualFree(lightProfileBuffer, 0, MEM_RELEASE);

    return TRUE;
}

BOOL CColorScanDoc::CreateSurfaceForProfile(MCHANDLE &Handle, PFLOAT32 &ProfileBuffer)
{
    MCSTATUS Status;
    int i;

    // Create a surface for the profile
    Status = McCreate(MC_DEFAULT_SURFACE_HANDLE, &Handle);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McCreate Surface failed.", Status);
        return FALSE;
    }

    // Allocate a memory buffer
    ProfileBuffer = (PFLOAT32)VirtualAlloc(NULL, 3 * sizeof(FLOAT32) * m_ImageSizeX_Px, MEM_COMMIT, PAGE_READWRITE);
    if (ProfileBuffer == NULL) 
    {
        ErrorDisplay("Error: Cannot allocate buffer.", 0);
        return FALSE;
    }

    // Set surface parameters
    for (i = 0 ; i < 3 ; i++)
    {
        Status = McSetParamInt(Handle, MC_SurfaceAddr + i, (INT32)(ProfileBuffer + i * m_ImageSizeX_Px));
        if (Status != MC_OK) 
        {
            ErrorDisplay("Error: McSetParamInt SurfaceAddress failed.", Status);
            return FALSE;
        }
    }
    Status = McSetParamInt(Handle, MC_SurfaceSize, sizeof(FLOAT32) * m_ImageSizeX_Px);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McSetParamInt SurfaceSize failed.", Status);
        return FALSE;
    }

    return TRUE;
}

BOOL CColorScanDoc::BuildProfile(PFLOAT32 ProfileBuffer)
{
    MCSTATUS Status;
    MCCALLBACKINFO cbInfo;
    MCHANDLE hSurface;
    PUINT8 pData;
    int i, j, c;
    ULONG mean;
    bool saturated = false;

    // Disable callbacks
    Status = McRegisterCallback(m_Channel, NULL, this);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McRegisterCallback failed.", Status);
        return FALSE;
    }

    // Grab one page
    if (!Grab())
    {
        ErrorDisplay("Error: Cannot grab", 0);
        return FALSE;
    }
    Status = McWaitSignal(m_Channel, MC_SIG_SURFACE_PROCESSING, 10000, &cbInfo);
    if (Status == MC_TIMEOUT)
    {
        ErrorDisplay("Error: Timeout", Status);
        return FALSE;
    }
    else if (Status != MC_OK)
    {
        ErrorDisplay("Error: Grab failed", Status);
        return FALSE;
    }

    hSurface = (MCHANDLE)cbInfo.SignalInfo;
    Status = McGetParamInt(hSurface, MC_SurfaceAddr, (PINT32)&pData);
    if (Status != MC_OK)
    {
        ErrorDisplay("Error: McGetParamInt MC_SurfaceAddr failed", Status);
        return FALSE;
    }

    // Build the profile
    switch (m_ColorFormat)
    {
    case MC_ColorFormat_RGB24PL:
        for (c = 0 ; c < 3 ; c++)
        {
            for (i = 0 ; i < m_ImageSizeX_Px ; i++)
            {
                for (j = 0, mean = 0 ; j < m_ImageSizeY_Ln ; j++)
                {
                    if (pData[(c*m_PlaneSize_Bytes)+(j * m_SurfacePitch_Bytes + i)] == 255)
                        saturated = true;

                    mean += pData[(c*m_PlaneSize_Bytes)+(j * m_SurfacePitch_Bytes + i)];
                }

                ProfileBuffer[c * m_ImageSizeX_Px + i] = static_cast<FLOAT32>((1.0 * mean / m_ImageSizeY_Ln) / 256);
            }
        }
        break;

    case MC_ColorFormat_RGB24:
        for (i = 0 ; i < m_ImageSizeX_Px ; i++)
        {
            for (c = 0 ; c < 3 ; c++)
            {
                for (j = 0, mean = 0 ; j < m_ImageSizeY_Ln ; j++)
                {
                    if (pData[j * m_SurfacePitch_Bytes + i * 3 + 2 - c] == 255)
                        saturated = true;

                    mean += pData[j * m_SurfacePitch_Bytes + i * 3 + 2 - c];
                }
                ProfileBuffer[c * m_ImageSizeX_Px + i] = static_cast<FLOAT32>((1.0 * mean / m_ImageSizeY_Ln) / 256);
            }
        }
        break;

    default:
        return FALSE;
    }

    McSetParamInt(hSurface, MC_SurfaceState, MC_SurfaceState_FREE);

    if (saturated)
    {
        AfxMessageBox("WARNING: saturation", MB_OK);
    }

    // Enable callbacks
    Status = McRegisterCallback(m_Channel, GlobalCallbackFunction, this);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: McRegisterCallback failed.", Status);
        return FALSE;
    }

    return TRUE;
}
