// PostalBorkenMenu V2A50
// Minimal x64 DXGI proxy + ASI loader.
// Purpose: forward the real System32 DXGI exports and load PostalBorkenMenu.asi.
// No Present hook, no renderer, no vtable patching, no overlay implementation.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winternl.h>
#include <dxgi.h>

#pragma function(memset)
#pragma function(memcpy)
extern "C" void* memset(void* dst, int v, size_t n) {
    unsigned char* d=(unsigned char*)dst;
    while(n--) *d++=(unsigned char)v;
    return dst;
}
extern "C" void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d=(unsigned char*)dst;
    const unsigned char* s=(const unsigned char*)src;
    while(n--) *d++=*s++;
    return dst;
}

static HMODULE g_self=0;
static HMODULE g_realDxgi=0;
static HMODULE g_asi=0;
static volatile LONG g_dxgiState=0;
static volatile LONG g_asiState=0;
static WCHAR g_logPath[1024]={};
static WCHAR g_augmentedCmdLine[16384]={};
static volatile LONG g_skipIntroCmdlineState=0; // 1 injected, 2 already present, -1 failed/disabled

extern "C" FARPROC g_real_DXGID3D10CreateDevice=0;
extern "C" FARPROC g_real_DXGID3D10CreateLayeredDevice=0;
extern "C" FARPROC g_real_DXGID3D10GetLayeredDeviceSize=0;
extern "C" FARPROC g_real_DXGID3D10RegisterLayers=0;
extern "C" FARPROC g_real_DXGIDeclareAdapterRemovalSupport=0;
extern "C" FARPROC g_real_DXGIGetDebugInterface1=0;
extern "C" FARPROC g_real_DXGIReportAdapterConfiguration=0;
extern "C" FARPROC g_real_PIXBeginCapture=0;
extern "C" FARPROC g_real_PIXEndCapture=0;
extern "C" FARPROC g_real_PIXGetCaptureState=0;
extern "C" FARPROC g_real_SetAppCompatStringPointer=0;
extern "C" FARPROC g_real_UpdateHMDEmulationStatus=0;
extern "C" FARPROC g_real_ApplyCompatResolutionQuirking=0;
extern "C" FARPROC g_real_CompatString=0;
extern "C" FARPROC g_real_CompatValue=0;

static FARPROC g_real_CreateDXGIFactory=0;
static FARPROC g_real_CreateDXGIFactory1=0;
static FARPROC g_real_CreateDXGIFactory2=0;

static unsigned LenA(const char* s) {
    unsigned n=0;
    if(s) while(s[n]) ++n;
    return n;
}
static void CopyW(WCHAR* dst, unsigned cap, const WCHAR* src) {
    if(!dst || !cap) return;
    unsigned i=0;
    if(src) for(;i+1<cap && src[i];++i) dst[i]=src[i];
    dst[i]=0;
}
static void AppendW(WCHAR* dst, unsigned cap, const WCHAR* src) {
    if(!dst || !cap || !src) return;
    unsigned i=0; while(i<cap && dst[i]) ++i;
    unsigned j=0; while(i+1<cap && src[j]) dst[i++]=src[j++];
    if(i<cap) dst[i]=0; else dst[cap-1]=0;
}
static BOOL ModuleDirectory(WCHAR* out, unsigned cap) {
    if(!out || cap<4) return FALSE;
    out[0]=0;
    DWORD n=GetModuleFileNameW(g_self,out,cap);
    if(!n || n>=cap) { out[0]=0; return FALSE; }
    for(DWORD i=n;i>0;--i) {
        if(out[i-1]==L'\\' || out[i-1]==L'/') {
            out[i]=0;
            return TRUE;
        }
    }
    out[0]=0;
    return FALSE;
}
static WCHAR LowerW(WCHAR c) {
    return (c>=L'A' && c<=L'Z') ? (WCHAR)(c-L'A'+L'a') : c;
}
static BOOL ContainsNoCaseW(const WCHAR* haystack, const WCHAR* needle) {
    if(!haystack || !needle || !needle[0]) return FALSE;
    for(unsigned i=0;haystack[i];++i) {
        unsigned j=0;
        while(needle[j] && haystack[i+j] &&
              LowerW(haystack[i+j])==LowerW(needle[j])) ++j;
        if(!needle[j]) return TRUE;
    }
    return FALSE;
}
static BOOL ReadSkipIntroSettingEarly() {
    WCHAR ini[1024]={};
    if(!ModuleDirectory(ini,1024)) return TRUE; // preserve H4 default=enabled
    AppendW(ini,1024,L"PostalBorkenMenu.ini");
    return GetPrivateProfileIntW(L"Settings",L"SkipIntroVideos",1,ini)!=0;
}
static BOOL InjectSkipIntroCommandLineEarly() {
    if(!ReadSkipIntroSettingEarly()) {
        g_skipIntroCmdlineState=-1;
        return FALSE;
    }

    const WCHAR* current=GetCommandLineW();
    if(!current || !current[0]) {
        g_skipIntroCmdlineState=-1;
        return FALSE;
    }

    if(ContainsNoCaseW(current,L"-skipintro")) {
        g_skipIntroCmdlineState=2;
        return TRUE;
    }

    unsigned n=0;
    while(current[n] && n+16<16384) {
        g_augmentedCmdLine[n]=current[n];
        ++n;
    }
    if(current[n]) {
        g_skipIntroCmdlineState=-1;
        return FALSE;
    }

    if(n && g_augmentedCmdLine[n-1]!=L' ' && g_augmentedCmdLine[n-1]!=L'\t')
        g_augmentedCmdLine[n++]=L' ';
    const WCHAR arg[]=L"-skipintro";
    for(unsigned j=0;arg[j] && n+1<16384;++j)
        g_augmentedCmdLine[n++]=arg[j];
    g_augmentedCmdLine[n]=0;

    PTEB teb=NtCurrentTeb();
    PPEB peb=teb ? teb->ProcessEnvironmentBlock : 0;
    PRTL_USER_PROCESS_PARAMETERS pp=peb ? peb->ProcessParameters : 0;
    if(!pp) {
        g_skipIntroCmdlineState=-1;
        return FALSE;
    }

    // dxgi.dll is loaded by the process loader before normal game startup.
    // Point the process command-line UNICODE_STRING at our process-lifetime
    // static buffer so Unity sees the same "-skipintro" launch argument the
    // user would normally place in Steam launch options.
    pp->CommandLine.Buffer=g_augmentedCmdLine;
    pp->CommandLine.Length=(USHORT)(n*sizeof(WCHAR));
    pp->CommandLine.MaximumLength=(USHORT)((n+1)*sizeof(WCHAR));
    g_skipIntroCmdlineState=1;
    return TRUE;
}

static void BuildLogPath() {
    if(g_logPath[0]) return;
    WCHAR dir[1024]={};
    if(!ModuleDirectory(dir,1024)) return;
    CopyW(g_logPath,1024,dir);
    AppendW(g_logPath,1024,L"PostalBorkenMenu.log");
}
static void Log(const char* s) {
    if(!s) return;
    BuildLogPath();
    if(!g_logPath[0]) return;
    HANDLE h=CreateFileW(g_logPath,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) return;
    SetFilePointer(h,0,0,FILE_END);
    DWORD wr=0; unsigned n=LenA(s);
    if(n) WriteFile(h,s,n,&wr,0);
    const char eol[2]={'\r','\n'};
    WriteFile(h,eol,2,&wr,0);
    CloseHandle(h);
}

extern "C" void ProxyMissingExport() {
    // Conservative fallback for optional compatibility/diagnostic exports
    // absent from a particular Windows DXGI build.
}

static FARPROC Resolve(const char* name) {
    FARPROC p=g_realDxgi ? GetProcAddress(g_realDxgi,name) : 0;
    return p ? p : (FARPROC)&ProxyMissingExport;
}

static void SetForwardersToFallback() {
    FARPROC f=(FARPROC)&ProxyMissingExport;
    g_real_DXGID3D10CreateDevice=f;
    g_real_DXGID3D10CreateLayeredDevice=f;
    g_real_DXGID3D10GetLayeredDeviceSize=f;
    g_real_DXGID3D10RegisterLayers=f;
    g_real_DXGIDeclareAdapterRemovalSupport=f;
    g_real_DXGIGetDebugInterface1=f;
    g_real_DXGIReportAdapterConfiguration=f;
    g_real_PIXBeginCapture=f;
    g_real_PIXEndCapture=f;
    g_real_PIXGetCaptureState=f;
    g_real_SetAppCompatStringPointer=f;
    g_real_UpdateHMDEmulationStatus=f;
    g_real_ApplyCompatResolutionQuirking=f;
    g_real_CompatString=f;
    g_real_CompatValue=f;
}

static void EnsureAsiLoaded() {
    LONG s=InterlockedCompareExchange(&g_asiState,1,0);
    if(s==2 || s==-1) return;
    if(s==1) {
        while((s=InterlockedCompareExchange(&g_asiState,1,1))==1) Sleep(0);
        return;
    }

    WCHAR path[1024]={};
    if(!ModuleDirectory(path,1024)) {
        Log("[DXGI LOADER] ERROR: could not resolve game directory.");
        InterlockedExchange(&g_asiState,-1);
        return;
    }
    AppendW(path,1024,L"PostalBorkenMenu.asi");

    g_asi=LoadLibraryW(path);
    if(!g_asi) {
        Log("[DXGI LOADER] ERROR: PostalBorkenMenu.asi failed to load.");
        InterlockedExchange(&g_asiState,-1);
        return;
    }

    Log("[DXGI LOADER] OK: PostalBorkenMenu.asi loaded.");
    InterlockedExchange(&g_asiState,2);
}

extern "C" void __stdcall EnsureRealDxgi() {
    LONG s=InterlockedCompareExchange(&g_dxgiState,1,0);
    if(s==2 || s==-1) {
        if(s==2) EnsureAsiLoaded();
        return;
    }
    if(s==1) {
        while((s=InterlockedCompareExchange(&g_dxgiState,1,1))==1) Sleep(0);
        if(s==2) EnsureAsiLoaded();
        return;
    }

    SetForwardersToFallback();

    WCHAR sys[512]={};
    UINT n=GetSystemDirectoryW(sys,512);
    if(!n || n>=500) {
        Log("[DXGI LOADER] ERROR: GetSystemDirectoryW failed.");
        InterlockedExchange(&g_dxgiState,-1);
        return;
    }
    AppendW(sys,512,L"\\dxgi.dll");

    g_realDxgi=LoadLibraryW(sys);
    if(!g_realDxgi) {
        Log("[DXGI LOADER] ERROR: failed to load System32\\dxgi.dll.");
        InterlockedExchange(&g_dxgiState,-1);
        return;
    }

    g_real_CreateDXGIFactory=Resolve("CreateDXGIFactory");
    g_real_CreateDXGIFactory1=Resolve("CreateDXGIFactory1");
    g_real_CreateDXGIFactory2=Resolve("CreateDXGIFactory2");

    g_real_DXGID3D10CreateDevice=Resolve("DXGID3D10CreateDevice");
    g_real_DXGID3D10CreateLayeredDevice=Resolve("DXGID3D10CreateLayeredDevice");
    g_real_DXGID3D10GetLayeredDeviceSize=Resolve("DXGID3D10GetLayeredDeviceSize");
    g_real_DXGID3D10RegisterLayers=Resolve("DXGID3D10RegisterLayers");
    g_real_DXGIDeclareAdapterRemovalSupport=Resolve("DXGIDeclareAdapterRemovalSupport");
    g_real_DXGIGetDebugInterface1=Resolve("DXGIGetDebugInterface1");
    g_real_DXGIReportAdapterConfiguration=Resolve("DXGIReportAdapterConfiguration");
    g_real_PIXBeginCapture=Resolve("PIXBeginCapture");
    g_real_PIXEndCapture=Resolve("PIXEndCapture");
    g_real_PIXGetCaptureState=Resolve("PIXGetCaptureState");
    g_real_SetAppCompatStringPointer=Resolve("SetAppCompatStringPointer");
    g_real_UpdateHMDEmulationStatus=Resolve("UpdateHMDEmulationStatus");
    g_real_ApplyCompatResolutionQuirking=Resolve("ApplyCompatResolutionQuirking");
    g_real_CompatString=Resolve("CompatString");
    g_real_CompatValue=Resolve("CompatValue");

    Log("[DXGI LOADER] OK: System32 DXGI forwarding initialized.");
    if(g_skipIntroCmdlineState==1)
        Log("[INTRO J1] OK: -skipintro injected into process command line before game startup.");
    else if(g_skipIntroCmdlineState==2)
        Log("[INTRO J1] OK: -skipintro already present in process command line.");
    else
        Log("[INTRO J1] Skip Intro launch argument not injected.");
    InterlockedExchange(&g_dxgiState,2);

    // Load the ASI only after the proxy is fully initialized, outside DllMain.
    EnsureAsiLoaded();
}

typedef HRESULT (WINAPI *PFN_CreateFactory)(REFIID,void**);
typedef HRESULT (WINAPI *PFN_CreateFactory2)(UINT,REFIID,void**);

extern "C" HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory fn=(PFN_CreateFactory)g_real_CreateDXGIFactory;
    if(!fn || fn==(PFN_CreateFactory)&ProxyMissingExport) return E_NOINTERFACE;
    return fn(riid,out);
}
extern "C" HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory fn=(PFN_CreateFactory)g_real_CreateDXGIFactory1;
    if(!fn || fn==(PFN_CreateFactory)&ProxyMissingExport) return E_NOINTERFACE;
    return fn(riid,out);
}
extern "C" HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory2 fn=(PFN_CreateFactory2)g_real_CreateDXGIFactory2;
    if(!fn || fn==(PFN_CreateFactory2)&ProxyMissingExport) return E_NOINTERFACE;
    return fn(flags,riid,out);
}

extern "C" BOOL WINAPI DllMain(HMODULE module, DWORD reason, void*) {
    if(reason==DLL_PROCESS_ATTACH) {
        g_self=module;
        DisableThreadLibraryCalls(module);
        InjectSkipIntroCommandLineEarly();
    }
    return TRUE;
}
