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
// LUT commands

void CColorScanDoc::OnLUT1() 
{
    if (m_LutIndex == 1)
        m_LutIndex = 0;
    else
        m_LutIndex = 1;

    LUTUse(m_LutIndex);
}

void CColorScanDoc::OnUpdateLUT1(CCmdUI *pCmdUI) 
{
    pCmdUI->SetCheck(m_LutIndex == 1);
}

void CColorScanDoc::OnLUT2() 
{
    if (m_LutIndex == 2)
        m_LutIndex = 0;
    else
        m_LutIndex = 2;

    LUTUse(m_LutIndex);
}

void CColorScanDoc::OnUpdateLUT2(CCmdUI *pCmdUI) 
{
    pCmdUI->SetCheck(m_LutIndex == 2);
}

void CColorScanDoc::OnLUT3() 
{
    if (m_LutIndex == 3)
        m_LutIndex = 0;
    else
        m_LutIndex = 3;

    LUTUse(m_LutIndex);
}

void CColorScanDoc::OnUpdateLUT3(CCmdUI *pCmdUI) 
{
    pCmdUI->SetCheck(m_LutIndex == 3);
}

void CColorScanDoc::OnLUT4() 
{
    if (m_LutIndex == 4)
        m_LutIndex = 0;
    else
        m_LutIndex = 4;

    LUTUse(m_LutIndex);
}

void CColorScanDoc::OnUpdateLUT4(CCmdUI *pCmdUI) 
{
    pCmdUI->SetCheck(m_LutIndex == 4);
}


/////////////////////////////////////////////////////////////////////////////
// LUT setup

BOOL CColorScanDoc::InitializeLUTs()
{
    return BuildLUT(1) && BuildLUT(2) && BuildLUT(3) && BuildLUT(4);
}

BOOL CColorScanDoc::BuildLUT(int Index)
{
    MCSTATUS status;
    int i;

    // Reset LUT parameters (Table, Contrast, Brightness, Visibility, Negative)
    status = McSetParamInt(m_Channel, MC_LUT_StoreIndex, 0);
    if (status != MC_OK)
    {
        ErrorDisplay("Error: McSetParamInt LUT:StoreIndex failed.", status);
        return FALSE;
    }

    // Set LUT parameters
    switch (Index)
    {
    case 1: // negative
        for (i = 0 ; i < 3 ; i++)
        {
            status = McSetParamInt(m_Channel, MC_LUT_Negative + i, MC_LUT_Negative_TRUE);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamInt LUT:Negative failed.", status);
                return FALSE;
            }
        }
        break;

    case 2: // a particular S-curve
        for (i = 0 ; i < 3 ; i++)
        {
            status = McSetParamFloat(m_Channel, MC_LUT_Contrast + i, 1.1);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Contrast failed.", status);
                return FALSE;
            }
            status = McSetParamFloat(m_Channel, MC_LUT_Visibility + i, 0.5);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Visibility failed.", status);
                return FALSE;
            }
        }
        break;

    case 3: // Gamma compensation (PAL)
        for (i = 0 ; i < 3 ; i++)
        {
            status = McSetParamFloat(m_Channel, MC_LUT_Contrast + i, 0.87);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Contrast failed.", status);
                return FALSE;
            }
            status = McSetParamFloat(m_Channel, MC_LUT_Brightness + i, 0.35);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Brightness failed.", status);
                return FALSE;
            }
            status = McSetParamFloat(m_Channel, MC_LUT_Visibility + i, 0.73);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Visibility failed.", status);
                return FALSE;
            }
        }
        break;

    case 4: // Gamma removal (PAL)
        for (i = 0 ; i < 3 ; i++)
        {
            status = McSetParamFloat(m_Channel, MC_LUT_Contrast + i, 1.13);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Contrast failed.", status);
                return FALSE;
            }
            status = McSetParamFloat(m_Channel, MC_LUT_Brightness + i, -0.35);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Brightness failed.", status);
                return FALSE;
            }
            status = McSetParamFloat(m_Channel, MC_LUT_Visibility + i, 0.76);
            if (status != MC_OK)
            {
                ErrorDisplay("Error: McSetParamFloat LUT:Visibility failed.", status);
                return FALSE;
            }
        }
        break;

    default:
        ErrorDisplay("Error: Invalid index.", Index);
        return FALSE;
    }

    // Build the LUT
    status = McSetParamInt(m_Channel, MC_LUT_StoreIndex, Index);
    if (status != MC_OK)
    {
        ErrorDisplay("Error: McSetParamInt LUT:StoreIndex failed.", status);
        return FALSE;
    }

    return TRUE;
}

BOOL CColorScanDoc::LUTUse(int Index)
{
    MCSTATUS Status;

    Status = McSetParamInt(m_Channel, MC_LUT_UseIndex, Index);
    if (Status != MC_OK) 
    {
        ErrorDisplay("Error: Cannot set LUT:UseIndex", Status);
        return FALSE;
    }

    return TRUE;
}
