VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   7920
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   9855
   LinkTopic       =   "Form1"
   ScaleHeight     =   7920
   ScaleWidth      =   9855
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer1 
      Interval        =   1
      Left            =   9360
      Top             =   7440
   End
   Begin VB.PictureBox Picture1 
      Height          =   7215
      Left            =   120
      ScaleHeight     =   7155
      ScaleWidth      =   9555
      TabIndex        =   4
      Top             =   120
      Width           =   9615
   End
   Begin VB.TextBox StatusBar 
      BackColor       =   &H8000000F&
      Height          =   285
      Index           =   1
      Left            =   5520
      Locked          =   -1  'True
      TabIndex        =   3
      Text            =   "ChannelState: IDLE"
      Top             =   7440
      Width           =   2895
   End
   Begin VB.CommandButton Stop 
      Caption         =   "Stop"
      Height          =   375
      Left            =   1320
      TabIndex        =   1
      Top             =   7440
      Width           =   1095
   End
   Begin VB.TextBox StatusBar 
      BackColor       =   &H8000000F&
      Height          =   285
      Index           =   0
      Left            =   2520
      Locked          =   -1  'True
      TabIndex        =   2
      Text            =   "Click on Go"
      Top             =   7440
      Width           =   2895
   End
   Begin VB.CommandButton Go 
      Caption         =   "Go"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   7440
      Width           =   1095
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

' +-------------------------------- DISCLAIMER ---------------------------------+
' |                                                                             |
' | This application program is provided to you free of charge as an example.   |
' | Despite the considerable efforts of Euresys personnel to create a usable    |
' | example, you should not assume that this program is error-free or suitable  |
' | for any purpose whatsoever.                                                 |
' |                                                                             |
' | EURESYS does not give any representation, warranty or undertaking that this |
' | program is free of any defect or error or suitable for any purpose. EURESYS |
' | shall not be liable, in contract, in torts or otherwise, for any damages,   |
' | loss, costs, expenses or other claims for compensation, including those     |
' | asserted by third parties, arising out of or in connection with the use of  |
' | this program.                                                               |
' |                                                                             |
' +-----------------------------------------------------------------------------+

' MultiCam sample program

Dim gChannel As Long
Dim gSizeX As Long
Dim gSizeY As Long
Dim gBufferPitch As Long
Dim gSurfAddr As Long
Dim bChannelActive As Boolean


Private Const BI_RGB = 0
Private Const DIB_RGB_COLORS = 0

Private Type BITMAPINFOHEADER
    biSize As Long
    biWidth As Long
    biHeight As Long
    biPlanes As Integer
    biBitCount As Integer
    biCompression As Long
    biSizeImage As Long
    biXPelsPerMeter As Long
    biYPelsPerMeter As Long
    biClrUsed As Long
    biClrImportant As Long
End Type

Private Type RGBQUAD
    rgbBlue As Byte
    rgbGreen As Byte
    rgbRed As Byte
    rgbReserved As Byte
End Type

Private Type BITMAPINFO
    bmiHeader As BITMAPINFOHEADER
    bmiColors(0 To 255) As RGBQUAD
End Type

Private Declare Function SetDIBitsToDevice Lib "gdi32" ( _
    ByVal hdc As Long, ByVal XDest As Long, ByVal YDest As Long, _
    ByVal dwWidth As Long, ByVal dwHeight As Long, _
    ByVal XSrc As Long, ByVal YSrc As Long, _
    ByVal uStartScan As Long, ByVal CScanLines As Long, _
    ByVal lpvBits As Long, ByRef lpbmi As BITMAPINFO, ByVal fuColorUse As Long) As Long

Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)


Private Function GetMcErrorDescription(mcError As Long) As String
    Dim status As Long
    Dim errorDescription As String
    status = McGetParamNmStr(MC_CONFIGURATION, "ErrorDesc:" & Abs(mcError), _
                             errorDescription)
    If status <> 0 Then
        errorDescription = "Unknown error"
    End If
    GetMcErrorDescription = errorDescription
End Function

Private Sub CheckMcStatus(mcError As Long)
If mcError <> 0 Then
    errorDescription = GetMcErrorDescription(mcError)
    MsgBox "MultiCam Error: " + errorDescription, vbOKOnly + vbExclamation
End If
End Sub

Private Sub Form_Initialize()
    CheckMcStatus McOpenDriver(vbNullString)
End Sub

Private Sub CreateMcChannel()
    ' Create a channel and associate it with the first connector on the first board
    CheckMcStatus McCreateNm("CHANNEL", gChannel)
    CheckMcStatus McSetParamNmInt(gChannel, "DriverIndex", 0)
    CheckMcStatus McSetParamNmStr(gChannel, "Connector", "VID1")
    ' Choose the video standard
    CheckMcStatus McSetParamNmStr(gChannel, "CamFile", "PAL")
    ' Choose the pixel color format
    CheckMcStatus McSetParamNmStr(gChannel, "ColorFormat", "RGB24")

    ' Choose the acquisition mode
    CheckMcStatus McSetParamNmStr(gChannel, "AcquisitionMode", "VIDEO")
    ' Choose the way the first acquisition is triggered
    CheckMcStatus McSetParamNmStr(gChannel, "TrigMode", "IMMEDIATE")
    ' Choose the triggering mode for subsequent acquisitions
    CheckMcStatus McSetParamNmStr(gChannel, "NextTrigMode", "REPEAT")
    ' Choose the number of images to acquire
    CheckMcStatus McSetParamNmInt(gChannel, "SeqLength_Fr", MC_INDETERMINATE)

    ' Retrieve image dimensions
    CheckMcStatus McGetParamNmInt(gChannel, "ImageSizeX", gSizeX)
    CheckMcStatus McGetParamNmInt(gChannel, "ImageSizeY", gSizeY)
    CheckMcStatus McGetParamNmInt(gChannel, "BufferPitch", gBufferPitch)

    ' The memory allocation for the images is automatically done by Multicam when activating the channel.
    ' We only set the number of surfaces to be created by MultiCam.
    CheckMcStatus McSetParamNmInt(gChannel, "SurfaceCount", 3)

    ' Enable SurfaceFilled and AcquisitionFailure signals:
    CheckMcStatus McSetParamNmStr(gChannel, "SignalEnable:" & MC_SIG_SURFACE_FILLED, "ON")
    CheckMcStatus McSetParamNmStr(gChannel, "SignalEnable:" & MC_SIG_ACQUISITION_FAILURE, "ON")
    CheckMcStatus McSetParamNmStr(gChannel, "SignalEnable:" & MC_SIG_END_CHANNEL_ACTIVITY, "ON")
    
    CheckMcStatus McSetParamNmStr(gChannel, "ChannelState", "READY")
End Sub

Private Sub Form_Load()
    ' This program demonstrates the Video Acquisition Mode on a Picolo Board.
    '
    ' The "Go" button starts an acquisition sequence by activating the channel.
    ' By default, this program requires an PAL camera connected on VID1.

    gChannel = 0
    gSizeX = 0
    gSizeY = 0
    gBufferPitch = 0
    gSurfAddr = 0
    bChannelActive = False
    
    Call CreateMcChannel
End Sub

Private Sub DrawImage()
    If gSurfAddr <> 0 Then
        Dim bmpInfo As BITMAPINFO
        bmpInfo.bmiHeader.biSize = Len(bmpInfo.bmiHeader)
        bmpInfo.bmiHeader.biPlanes = 1
        bmpInfo.bmiHeader.biBitCount = 24
        bmpInfo.bmiHeader.biCompression = BI_RGB
        bmpInfo.bmiHeader.biSizeImage = 0
        bmpInfo.bmiHeader.biXPelsPerMeter = 0
        bmpInfo.bmiHeader.biYPelsPerMeter = 0
        bmpInfo.bmiHeader.biClrUsed = 0
        bmpInfo.bmiHeader.biClrImportant = 0
        For i = 0 To UBound(bmpInfo.bmiColors)
            bmpInfo.bmiColors(i).rgbBlue = i
            bmpInfo.bmiColors(i).rgbGreen = i
            bmpInfo.bmiColors(i).rgbRed = i
            bmpInfo.bmiColors(i).rgbReserved = 0
        Next i
        ' Width = Pitch(bytes) divided by the number of bytes per pixel
        bmpInfo.bmiHeader.biWidth = gBufferPitch / (bmpInfo.bmiHeader.biBitCount / 8)
        bmpInfo.bmiHeader.biHeight = -gSizeY
        SetDIBitsToDevice Picture1.hdc, 0, 0, gSizeX, gSizeY, 0, 0, 0, gSizeY, gSurfAddr, bmpInfo, DIB_RGB_COLORS
    End If
End Sub

Private Sub Form_Paint()
    Call DrawImage
End Sub

Private Sub ProcessSurfaceFilled(SigInfo As MCSIGNALINFO)
    Dim frameRate As String
    CheckMcStatus McGetParamNmInt(SigInfo.SignalInfo, "SurfaceAddr", gSurfAddr)
    Call DrawImage
    ' Retrieve the frame rate and display it
    CheckMcStatus McGetParamNmStr(SigInfo.Instance, "PerSecond_Fr", frameRate)
    StatusBar(0).Text = "PerSecond_Fr: " + frameRate
End Sub

Private Sub ProcessAcquisitionFailure()
    Dim state As String
    CheckMcStatus McGetParamNmStr(gChannel, "ChannelState", state)
    StatusBar(0).Text = "Acquisition Failure"
    StatusBar(1).Text = "ChannelState: " + state
End Sub

Private Sub Timer1_Timer()
    Dim SigInfo As MCSIGNALINFO
    Dim status As Long
    ' Wait for a MultiCam signal
    status = McWaitSignal(gChannel, MC_SIG_ANY, 0&, SigInfo)
    If (status <> MC_OK) Then
        Exit Sub
    ElseIf (SigInfo.Signal = MC_SIG_SURFACE_FILLED) Then
        Call ProcessSurfaceFilled(SigInfo)
    ElseIf (SigInfo.Signal = MC_SIG_ACQUISITION_FAILURE) Then
        Call ProcessAcquisitionFailure
    ElseIf (SigInfo.Signal = MC_SIG_END_CHANNEL_ACTIVITY) Then
         bChannelActive = False
    End If
End Sub

Private Sub DeleteMcChannel()
    If gChannel <> 0 Then
        CheckMcStatus McDelete(gChannel)
        gChannel = 0
    End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
    CheckMcStatus McSetParamNmStr(gChannel, "ChannelState", "IDLE")
    Do While bChannelActive = True
        DoEvents
        Sleep (1)
    Loop
    
    Call DeleteMcChannel
End Sub

Private Sub Form_Terminate()
    CheckMcStatus McCloseDriver()
End Sub

Private Sub Go_Click()
    Dim state As String
    ' Start an acquisition sequence by activating the channel
    CheckMcStatus McGetParamNmStr(gChannel, "ChannelState", state)
    If state <> "ACTIVE" Then
        CheckMcStatus McSetParamNmStr(gChannel, "ChannelState", "ACTIVE")
        StatusBar(0).Text = ""
        StatusBar(1).Text = "ChannelState: ACTIVE"
        bChannelActive = True
    End If
End Sub

Private Sub Stop_Click()
    ' Stop an acquisition sequence by deactivating the channel
    CheckMcStatus McSetParamNmStr(gChannel, "ChannelState", "IDLE")
    StatusBar(0).Text = "FrameRate_Hz: 0"
    StatusBar(1).Text = "ChannelState: IDLE"
End Sub
