; PostalBorkenMenu V2A49A generic DXGI export thunks.
; Each thunk preserves the Win64 argument registers and XMM0-XMM3, lazily
; resolves the real system DXGI, restores the exact call state, then tail-jumps.

OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

EXTERN EnsureRealDxgi:PROC

EXTERN g_real_DXGID3D10CreateDevice:QWORD
EXTERN g_real_DXGID3D10CreateLayeredDevice:QWORD
EXTERN g_real_DXGID3D10GetLayeredDeviceSize:QWORD
EXTERN g_real_DXGID3D10RegisterLayers:QWORD
EXTERN g_real_DXGIDeclareAdapterRemovalSupport:QWORD
EXTERN g_real_DXGIGetDebugInterface1:QWORD
EXTERN g_real_DXGIReportAdapterConfiguration:QWORD
EXTERN g_real_PIXBeginCapture:QWORD
EXTERN g_real_PIXEndCapture:QWORD
EXTERN g_real_PIXGetCaptureState:QWORD
EXTERN g_real_SetAppCompatStringPointer:QWORD
EXTERN g_real_UpdateHMDEmulationStatus:QWORD
EXTERN g_real_ApplyCompatResolutionQuirking:QWORD
EXTERN g_real_CompatString:QWORD
EXTERN g_real_CompatValue:QWORD

.code

THUNK MACRO name:req, target:req
PUBLIC name
name PROC
    sub rsp, 0A8h
    mov [rsp+060h], rcx
    mov [rsp+068h], rdx
    mov [rsp+070h], r8
    mov [rsp+078h], r9
    movdqu [rsp+020h], xmm0
    movdqu [rsp+030h], xmm1
    movdqu [rsp+040h], xmm2
    movdqu [rsp+050h], xmm3
    call EnsureRealDxgi
    movdqu xmm0, [rsp+020h]
    movdqu xmm1, [rsp+030h]
    movdqu xmm2, [rsp+040h]
    movdqu xmm3, [rsp+050h]
    mov rcx, [rsp+060h]
    mov rdx, [rsp+068h]
    mov r8,  [rsp+070h]
    mov r9,  [rsp+078h]
    add rsp, 0A8h
    jmp qword ptr [target]
name ENDP
ENDM

THUNK Proxy_DXGID3D10CreateDevice, g_real_DXGID3D10CreateDevice
THUNK Proxy_DXGID3D10CreateLayeredDevice, g_real_DXGID3D10CreateLayeredDevice
THUNK Proxy_DXGID3D10GetLayeredDeviceSize, g_real_DXGID3D10GetLayeredDeviceSize
THUNK Proxy_DXGID3D10RegisterLayers, g_real_DXGID3D10RegisterLayers
THUNK Proxy_DXGIDeclareAdapterRemovalSupport, g_real_DXGIDeclareAdapterRemovalSupport
THUNK Proxy_DXGIGetDebugInterface1, g_real_DXGIGetDebugInterface1
THUNK Proxy_DXGIReportAdapterConfiguration, g_real_DXGIReportAdapterConfiguration
THUNK Proxy_PIXBeginCapture, g_real_PIXBeginCapture
THUNK Proxy_PIXEndCapture, g_real_PIXEndCapture
THUNK Proxy_PIXGetCaptureState, g_real_PIXGetCaptureState
THUNK Proxy_SetAppCompatStringPointer, g_real_SetAppCompatStringPointer
THUNK Proxy_UpdateHMDEmulationStatus, g_real_UpdateHMDEmulationStatus
THUNK Proxy_ApplyCompatResolutionQuirking, g_real_ApplyCompatResolutionQuirking
THUNK Proxy_CompatString, g_real_CompatString
THUNK Proxy_CompatValue, g_real_CompatValue

END
