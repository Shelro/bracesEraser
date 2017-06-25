Attribute VB_Name = "Module1"
' Default object instance Constants
Public Const MC_CONFIGURATION = 536870912
Public Const MC_BOARD = -536870912
Public Const MC_CHANNEL = -2147418113
Public Const MC_DEFAULT_SURFACE_HANDLE = 1342177279

' Specific parameter values Constants
Public Const MC_INFINITE = -1&
Public Const MC_INDETERMINATE = -1&
Public Const MC_LOW_PART = 0&
Public Const MC_HIGH_PART = 1&
Public Const MC_DISABLE = 0&

' Signal Constants
Public Const MC_SIG_ANY = 0&
Public Const MC_SIG_SURFACE_PROCESSING = 1&
Public Const MC_SIG_SURFACE_FILLED = 2&
Public Const MC_SIG_UNRECOVERABLE_OVERRUN = 3&
Public Const MC_SIG_FRAMETRIGGER_VIOLATION = 4&
Public Const MC_SIG_START_EXPOSURE = 5&
Public Const MC_SIG_END_EXPOSURE = 6&
Public Const MC_SIG_ACQUISITION_FAILURE = 7&
Public Const MC_SIG_CLUSTER_UNAVAILABLE = 8&
Public Const MC_SIG_RELEASE = 9&
Public Const MC_SIG_END_ACQUISITION_SEQUENCE = 10&
Public Const MC_SIG_START_ACQUISITION_SEQUENCE = 11&
Public Const MC_SIG_END_CHANNEL_ACTIVITY = 12&

Public Const MC_SIG_GOLOW = 4096&
Public Const MC_SIG_GOHIGH = 8192&
Public Const MC_SIG_GOOPEN = 12288&

' Driver connection Functions
Public Declare Function McOpenDriver Lib "MultiCam.dll" Alias _
    "_McOpenDriver@4" (ByVal szString As String) As Long

Public Declare Function McCloseDriver Lib "MultiCam.dll" Alias _
    "_McCloseDriver@0" () As Long

' Object creation/deletion Functions
Public Declare Function McCreate Lib "MultiCam.dll" Alias _
    "_McCreate@8" (ByVal ModelInstance As Long, ByRef Instance As Long) As Long

Public Declare Function McCreateNm Lib "MultiCam.dll" Alias _
    "_McCreateNm@8" (ByVal ModelName As String, ByRef Instance As Long) As Long

Public Declare Function McDelete Lib "MultiCam.dll" Alias _
    "_McDelete@4" (ByVal ModelInstance As Long) As Long

' Parameter 'setter' Functions
Public Declare Function McSetParamNmInt Lib "MultiCam.dll" Alias _
    "_McSetParamNmInt@12" (ByVal Instance As Long, ByVal ParamName As String, _
                           ByVal ValueInt As Long) As Long

Public Declare Function McSetParamNmStr Lib "MultiCam.dll" Alias _
    "_McSetParamNmStr@12" (ByVal Instance As Long, ByVal ParamName As String, _
                           ByVal ValueStr As String) As Long

Public Declare Function McSetParamNmFloat Lib "MultiCam.dll" Alias _
    "_McSetParamNmFloat@16" (ByVal Instance As Long, ByVal ParamName As String, _
                             ByVal ValueFloat As Double) As Long

Public Declare Function McSetParamNmInst Lib "MultiCam.dll" Alias _
    "_McSetParamNmInst@12" (ByVal Instance As Long, ByVal ParamName As String, _
                            ByVal ValueInst As Long) As Long

' Parameter 'getter' Functions
Public Declare Function McGetParamNmInt Lib "MultiCam.dll" Alias _
    "_McGetParamNmInt@12" (ByVal Instance As Long, ByVal ParamName As String, _
                           ByRef ValueInt As Long) As Long

Private Declare Function McGetParamNmStrInternal Lib "MultiCam.dll" Alias _
    "_McGetParamNmStr@16" (ByVal Instance As Long, ByVal ParamName As String, _
                           ByVal ValueStr As String, ByVal MaxLength As Long) As Long

Public Declare Function McGetParamNmFloat Lib "MultiCam.dll" Alias _
    "_McGetParamNmFloat@12" (ByVal Instance As Long, ByVal ParamName As String, _
                             ByRef ValueFloat As Double) As Long

Public Declare Function McGetParamNmInst Lib "MultiCam.dll" Alias _
    "_McGetParamNmInst@12" (ByVal Instance As Long, ByVal ParamName As String, _
                            ByRef ValueInst As Long) As Long

' Signal handling Functions
Public Declare Function McWaitSignal Lib "MultiCam.dll" Alias _
    "_McWaitSignal@16" (ByVal Instance As Long, ByVal Signal As Long, _
                        ByVal Timeout As Long, ByRef SigInfo As MCSIGNALINFO) As Long

Public Declare Function McGetSignalInfo Lib "MultiCam.dll" Alias _
    "_McGetSignalInfo@12" (ByVal Instance As Long, ByVal Signal As Long, _
                           ByRef SigInfo As MCSIGNALINFO) As Long

' Signal handling Type Definitions
Public Type MCSIGNALINFO
    Context As Long
    Instance As Long
    Signal As Long
    SignalInfo As Long
    SignalContext As Long
End Type

Private Function ConvertToVbString(ByVal cString As String) As String
    Dim vbString As String
    Dim nullCharPos As Integer
    nullCharPos = InStr(cString, vbNullChar)
    If nullCharPos > 0 Then
        vbString = Left(cString, nullCharPos - 1)
    Else
        vbString = cString
    End If
    ConvertToVbString = vbString
End Function

Public Function McGetParamNmStr(ByVal Instance As Long, ByVal ParamName As String, _
                                ByRef ValueStr As String) As Long
    Const MAX_VALUE_LENGTH = 1024&
    Dim status As Long
    ValueStr = String(MAX_VALUE_LENGTH, vbNullChar)
    status = McGetParamNmStrInternal(Instance, ParamName, ValueStr, MAX_VALUE_LENGTH)
    ValueStr = ConvertToVbString(ValueStr)
    McGetParamNmStr = status
End Function
