// PostalBorkenMenu V2A49B
// Empty DX11 overlay on top of the validated V2A49A true DXGI proxy.
// Exact V2A42 ASI remains untouched.
// This build adds only one thing: a blank dark rectangle drawn directly into
// the REAL game swapchain before Present. No input, no cursor, no options.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d11_1.h>

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

static HMODULE g_self = 0;
static HMODULE g_realDxgi = 0;
static volatile LONG g_initState = 0;
static WCHAR g_logPath[1024] = {};

extern "C" FARPROC g_real_DXGID3D10CreateDevice = 0;
extern "C" FARPROC g_real_DXGID3D10CreateLayeredDevice = 0;
extern "C" FARPROC g_real_DXGID3D10GetLayeredDeviceSize = 0;
extern "C" FARPROC g_real_DXGID3D10RegisterLayers = 0;
extern "C" FARPROC g_real_DXGIDeclareAdapterRemovalSupport = 0;
extern "C" FARPROC g_real_DXGIGetDebugInterface1 = 0;
extern "C" FARPROC g_real_DXGIReportAdapterConfiguration = 0;
extern "C" FARPROC g_real_PIXBeginCapture = 0;
extern "C" FARPROC g_real_PIXEndCapture = 0;
extern "C" FARPROC g_real_PIXGetCaptureState = 0;
extern "C" FARPROC g_real_SetAppCompatStringPointer = 0;
extern "C" FARPROC g_real_UpdateHMDEmulationStatus = 0;
extern "C" FARPROC g_real_ApplyCompatResolutionQuirking = 0;
extern "C" FARPROC g_real_CompatString = 0;
extern "C" FARPROC g_real_CompatValue = 0;

static FARPROC g_real_CreateDXGIFactory = 0;
static FARPROC g_real_CreateDXGIFactory1 = 0;
static FARPROC g_real_CreateDXGIFactory2 = 0;

static void CopyW(WCHAR* dst, unsigned cap, const WCHAR* src) {
    if(!dst || !cap) return;
    unsigned i=0;
    if(src) for(; i+1<cap && src[i]; ++i) dst[i]=src[i];
    dst[i]=0;
}
static void AppendW(WCHAR* dst, unsigned cap, const WCHAR* src) {
    if(!dst || !cap || !src) return;
    unsigned i=0; while(i<cap && dst[i]) ++i;
    unsigned j=0; while(i+1<cap && src[j]) dst[i++]=src[j++];
    if(i<cap) dst[i]=0; else dst[cap-1]=0;
}
static unsigned LenA(const char* s) { unsigned n=0; if(s) while(s[n]) ++n; return n; }

static void BuildLogPath() {
    if(g_logPath[0]) return;
    WCHAR path[1024]={};
    DWORD n=GetModuleFileNameW(g_self,path,1024);
    if(!n || n>=1024) return;
    for(DWORD i=n;i>0;--i) {
        if(path[i-1]==L'\\' || path[i-1]==L'/') {
            path[i]=0;
            break;
        }
    }
    CopyW(g_logPath,1024,path);
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
    const char eol[2]={'\r','\n'}; WriteFile(h,eol,2,&wr,0);
    CloseHandle(h);
}

extern "C" void ProxyMissingExport() {
    // Generic fallback target for an export absent from the host OS.
    // The assembly thunk tail-jumps here with the caller's original stack.
    // Returning zero is a conservative failure-ish value for the compatibility
    // and diagnostic exports this probe forwards.
}

static FARPROC Resolve(const char* name) {
    FARPROC p=g_realDxgi ? GetProcAddress(g_realDxgi,name) : 0;
    return p ? p : (FARPROC)&ProxyMissingExport;
}

extern "C" void __stdcall EnsureRealDxgi() {
    LONG state=InterlockedCompareExchange(&g_initState,1,0);
    if(state==2 || state==-1) return;
    if(state==1) {
        while((state=InterlockedCompareExchange(&g_initState,1,1))==1) Sleep(0);
        return;
    }

    WCHAR sys[512]={};
    UINT n=GetSystemDirectoryW(sys,512);
    if(!n || n>=500) {
        InterlockedExchange(&g_initState,-1);
        return;
    }
    AppendW(sys,512,L"\\dxgi.dll");
    g_realDxgi=LoadLibraryW(sys);
    if(!g_realDxgi) {
        Log("[DXGI49B] ERROR: failed to load system32\\dxgi.dll");
        InterlockedExchange(&g_initState,-1);
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

    Log("[DXGI49B] System DXGI loaded lazily; proxy forwarding active.");
    InterlockedExchange(&g_initState,2);
}

typedef HRESULT (WINAPI *PFN_CreateFactory)(REFIID,void**);
typedef HRESULT (WINAPI *PFN_CreateFactory2)(UINT,REFIID,void**);

typedef HRESULT (STDMETHODCALLTYPE *PFN_FactoryCreateSwapChain)(
    IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**);
typedef HRESULT (STDMETHODCALLTYPE *PFN_FactoryCreateSwapChainForHwnd)(
    IDXGIFactory2*, IUnknown*, HWND, const DXGI_SWAP_CHAIN_DESC1*,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**);
typedef HRESULT (STDMETHODCALLTYPE *PFN_FactoryCreateSwapChainForCoreWindow)(
    IDXGIFactory2*, IUnknown*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*,
    IDXGIOutput*, IDXGISwapChain1**);
typedef HRESULT (STDMETHODCALLTYPE *PFN_FactoryCreateSwapChainForComposition)(
    IDXGIFactory2*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**);

typedef HRESULT (STDMETHODCALLTYPE *PFN_SwapPresent)(IDXGISwapChain*,UINT,UINT);

struct FactoryRec {
    void* object;
    void** originalVt;
    void** privateVt;
    unsigned count;
    PFN_FactoryCreateSwapChain createSwapChain;
    PFN_FactoryCreateSwapChainForHwnd createHwnd;
    PFN_FactoryCreateSwapChainForCoreWindow createCore;
    PFN_FactoryCreateSwapChainForComposition createComposition;
};
struct SwapRec {
    void* object;
    void** originalVt;
    void** privateVt;
    unsigned count;
    PFN_SwapPresent present;
    volatile LONG logged;

    ID3D11Device* device;
    ID3D11DeviceContext1* context1;
    ID3D11Texture2D* backBuffer;
    ID3D11RenderTargetView* rtv;
    volatile LONG overlayReadyLogged;
    volatile LONG overlayErrorLogged;
};

static FactoryRec g_factories[16] = {};
static SwapRec g_swaps[32] = {};
static volatile LONG g_factoryLock=0;
static volatile LONG g_swapLock=0;

static void Lock(volatile LONG* p) {
    while(InterlockedCompareExchange(p,1,0)!=0) Sleep(0);
}
static void Unlock(volatile LONG* p) { InterlockedExchange(p,0); }

static void CopyPtrs(void** dst, void** src, unsigned count) {
    for(unsigned i=0;i<count;i++) dst[i]=src[i];
}

static unsigned FactoryVtableCount(IUnknown* u) {
    if(!u) return 12;
    void* p=0;
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory7),&p)) && p) { ((IUnknown*)p)->Release(); return 32; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory6),&p)) && p) { ((IUnknown*)p)->Release(); return 30; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory5),&p)) && p) { ((IUnknown*)p)->Release(); return 29; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory4),&p)) && p) { ((IUnknown*)p)->Release(); return 28; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory3),&p)) && p) { ((IUnknown*)p)->Release(); return 26; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory2),&p)) && p) { ((IUnknown*)p)->Release(); return 25; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGIFactory1),&p)) && p) { ((IUnknown*)p)->Release(); return 14; }
    return 12;
}
static unsigned SwapVtableCount(IUnknown* u) {
    if(!u) return 18;
    void* p=0;
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGISwapChain4),&p)) && p) { ((IUnknown*)p)->Release(); return 41; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGISwapChain3),&p)) && p) { ((IUnknown*)p)->Release(); return 40; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGISwapChain2),&p)) && p) { ((IUnknown*)p)->Release(); return 36; }
    if(SUCCEEDED(u->QueryInterface(__uuidof(IDXGISwapChain1),&p)) && p) { ((IUnknown*)p)->Release(); return 29; }
    return 18;
}

static FactoryRec* FindFactory(void* self) {
    for(unsigned i=0;i<16;i++)
        if(g_factories[i].object==self || (g_factories[i].privateVt && *(void***)self==g_factories[i].privateVt))
            return &g_factories[i];
    return 0;
}
static SwapRec* FindSwap(void* self) {
    for(unsigned i=0;i<32;i++)
        if(g_swaps[i].object==self || (g_swaps[i].privateVt && *(void***)self==g_swaps[i].privateVt))
            return &g_swaps[i];
    return 0;
}

static void ReleaseOverlayTarget(SwapRec* r) {
    if(!r) return;
    if(r->rtv) { r->rtv->Release(); r->rtv=0; }
    if(r->backBuffer) { r->backBuffer->Release(); r->backBuffer=0; }
}

static BOOL EnsureOverlayRenderer(IDXGISwapChain* self, SwapRec* r) {
    if(!self || !r) return FALSE;

    if(!r->device || !r->context1) {
        ID3D11Device* dev=0;
        HRESULT hr=self->GetDevice(__uuidof(ID3D11Device),(void**)&dev);
        if(FAILED(hr) || !dev) {
            if(InterlockedCompareExchange(&r->overlayErrorLogged,1,0)==0)
                Log("[DXGI49B] ERROR: real swapchain did not expose an ID3D11Device.");
            return FALSE;
        }

        ID3D11DeviceContext* baseCtx=0;
        dev->GetImmediateContext(&baseCtx);
        if(!baseCtx) {
            dev->Release();
            if(InterlockedCompareExchange(&r->overlayErrorLogged,1,0)==0)
                Log("[DXGI49B] ERROR: failed to get D3D11 immediate context.");
            return FALSE;
        }

        ID3D11DeviceContext1* ctx1=0;
        hr=baseCtx->QueryInterface(__uuidof(ID3D11DeviceContext1),(void**)&ctx1);
        baseCtx->Release();
        if(FAILED(hr) || !ctx1) {
            dev->Release();
            if(InterlockedCompareExchange(&r->overlayErrorLogged,1,0)==0)
                Log("[DXGI49B] ERROR: ID3D11DeviceContext1 unavailable; empty overlay disabled.");
            return FALSE;
        }

        r->device=dev;
        r->context1=ctx1;
    }

    ID3D11Texture2D* current=0;
    HRESULT hr=self->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&current);
    if(FAILED(hr) || !current) return FALSE;

    if(current==r->backBuffer && r->rtv) {
        current->Release();
        return TRUE;
    }

    ReleaseOverlayTarget(r);
    r->backBuffer=current;

    hr=r->device->CreateRenderTargetView(r->backBuffer,0,&r->rtv);
    if(FAILED(hr) || !r->rtv) {
        ReleaseOverlayTarget(r);
        if(InterlockedCompareExchange(&r->overlayErrorLogged,1,0)==0)
            Log("[DXGI49B] ERROR: failed to create backbuffer RTV for empty overlay.");
        return FALSE;
    }

    if(InterlockedCompareExchange(&r->overlayReadyLogged,1,0)==0)
        Log("[DXGI49B] Empty DX11 overlay renderer attached to the real game backbuffer.");
    return TRUE;
}

static void DrawEmptyOverlay(IDXGISwapChain* self, SwapRec* r) {
    if(!EnsureOverlayRenderer(self,r)) return;

    D3D11_TEXTURE2D_DESC td={};
    r->backBuffer->GetDesc(&td);

    LONG left=32;
    LONG top=32;
    LONG right=(LONG)(32+760);
    LONG bottom=(LONG)(32+600);

    if(right>(LONG)td.Width-16) right=(LONG)td.Width-16;
    if(bottom>(LONG)td.Height-16) bottom=(LONG)td.Height-16;
    if(right<=left || bottom<=top) return;

    D3D11_RECT rect={left,top,right,bottom};
    const FLOAT panel[4]={0.035f,0.038f,0.045f,1.0f};

    // ClearView draws only this rectangle and does not require us to replace
    // the game's pipeline state with our own shaders/blend/rasterizer state.
    r->context1->ClearView(r->rtv,panel,&rect,1);
}

static HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* self, UINT sync, UINT flags) {
    SwapRec* r=FindSwap(self);
    if(!r || !r->present) return DXGI_ERROR_INVALID_CALL;

    if(InterlockedCompareExchange(&r->logged,1,0)==0)
        Log("[DXGI49B] Real game swapchain Present reached through private per-object vtable.");

    DrawEmptyOverlay(self,r);
    return r->present(self,sync,flags);
}

static BOOL PatchSwapchain(IUnknown* unk) {
    if(!unk) return FALSE;

    Lock(&g_swapLock);
    for(unsigned i=0;i<32;i++) {
        if(g_swaps[i].object==unk) { Unlock(&g_swapLock); return TRUE; }
    }

    unsigned slot=32;
    for(unsigned i=0;i<32;i++) if(!g_swaps[i].object) { slot=i; break; }
    if(slot==32) { Unlock(&g_swapLock); Log("[DXGI49B] WARN: swapchain record table full."); return FALSE; }

    unsigned count=SwapVtableCount(unk);
    void** original=*(void***)unk;
    SIZE_T bytes=(SIZE_T)count*sizeof(void*);
    void** clone=(void**)VirtualAlloc(0,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!clone) { Unlock(&g_swapLock); return FALSE; }
    CopyPtrs(clone,original,count);

    SwapRec& r=g_swaps[slot];
    r.object=unk;
    r.originalVt=original;
    r.privateVt=clone;
    r.count=count;
    r.present=(PFN_SwapPresent)original[8];
    r.logged=0;
    r.device=0;
    r.context1=0;
    r.backBuffer=0;
    r.rtv=0;
    r.overlayReadyLogged=0;
    r.overlayErrorLogged=0;

    clone[8]=(void*)&HookPresent;
    *(void***)unk=clone;

    Unlock(&g_swapLock);
    Log("[DXGI49B] Real swapchain patched with a private vtable clone; no global DXGI vtable was modified.");
    return TRUE;
}

static HRESULT STDMETHODCALLTYPE HookCreateSwapChain(
    IDXGIFactory* self, IUnknown* device, DXGI_SWAP_CHAIN_DESC* desc, IDXGISwapChain** out) {
    FactoryRec* r=FindFactory(self);
    if(!r || !r->createSwapChain) return DXGI_ERROR_INVALID_CALL;
    HRESULT hr=r->createSwapChain(self,device,desc,out);
    if(SUCCEEDED(hr) && out && *out) PatchSwapchain(*out);
    return hr;
}
static HRESULT STDMETHODCALLTYPE HookCreateSwapChainForHwnd(
    IDXGIFactory2* self, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fs, IDXGIOutput* restrictOut, IDXGISwapChain1** out) {
    FactoryRec* r=FindFactory(self);
    if(!r || !r->createHwnd) return DXGI_ERROR_INVALID_CALL;
    HRESULT hr=r->createHwnd(self,device,hwnd,desc,fs,restrictOut,out);
    if(SUCCEEDED(hr) && out && *out) PatchSwapchain(*out);
    return hr;
}
static HRESULT STDMETHODCALLTYPE HookCreateSwapChainForCoreWindow(
    IDXGIFactory2* self, IUnknown* device, IUnknown* window, const DXGI_SWAP_CHAIN_DESC1* desc,
    IDXGIOutput* restrictOut, IDXGISwapChain1** out) {
    FactoryRec* r=FindFactory(self);
    if(!r || !r->createCore) return DXGI_ERROR_INVALID_CALL;
    HRESULT hr=r->createCore(self,device,window,desc,restrictOut,out);
    if(SUCCEEDED(hr) && out && *out) PatchSwapchain(*out);
    return hr;
}
static HRESULT STDMETHODCALLTYPE HookCreateSwapChainForComposition(
    IDXGIFactory2* self, IUnknown* device, const DXGI_SWAP_CHAIN_DESC1* desc,
    IDXGIOutput* restrictOut, IDXGISwapChain1** out) {
    FactoryRec* r=FindFactory(self);
    if(!r || !r->createComposition) return DXGI_ERROR_INVALID_CALL;
    HRESULT hr=r->createComposition(self,device,desc,restrictOut,out);
    if(SUCCEEDED(hr) && out && *out) PatchSwapchain(*out);
    return hr;
}

static BOOL PatchFactory(IUnknown* unk) {
    if(!unk) return FALSE;

    Lock(&g_factoryLock);
    for(unsigned i=0;i<16;i++) {
        if(g_factories[i].object==unk) { Unlock(&g_factoryLock); return TRUE; }
    }

    unsigned slot=16;
    for(unsigned i=0;i<16;i++) if(!g_factories[i].object) { slot=i; break; }
    if(slot==16) { Unlock(&g_factoryLock); Log("[DXGI49B] WARN: factory record table full."); return FALSE; }

    unsigned count=FactoryVtableCount(unk);
    void** original=*(void***)unk;
    SIZE_T bytes=(SIZE_T)count*sizeof(void*);
    void** clone=(void**)VirtualAlloc(0,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!clone) { Unlock(&g_factoryLock); return FALSE; }
    CopyPtrs(clone,original,count);

    FactoryRec& r=g_factories[slot];
    r.object=unk;
    r.originalVt=original;
    r.privateVt=clone;
    r.count=count;
    r.createSwapChain = count>10 ? (PFN_FactoryCreateSwapChain)original[10] : 0;
    r.createHwnd = count>15 ? (PFN_FactoryCreateSwapChainForHwnd)original[15] : 0;
    r.createCore = count>16 ? (PFN_FactoryCreateSwapChainForCoreWindow)original[16] : 0;
    r.createComposition = count>24 ? (PFN_FactoryCreateSwapChainForComposition)original[24] : 0;

    if(count>10) clone[10]=(void*)&HookCreateSwapChain;
    if(count>15) clone[15]=(void*)&HookCreateSwapChainForHwnd;
    if(count>16) clone[16]=(void*)&HookCreateSwapChainForCoreWindow;
    if(count>24) clone[24]=(void*)&HookCreateSwapChainForComposition;

    *(void***)unk=clone;

    Unlock(&g_factoryLock);
    Log("[DXGI49B] Real DXGI factory patched with a private vtable clone.");
    return TRUE;
}

extern "C" HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory fn=(PFN_CreateFactory)g_real_CreateDXGIFactory;
    if(!fn || fn==(PFN_CreateFactory)&ProxyMissingExport) return E_NOINTERFACE;
    HRESULT hr=fn(riid,out);
    if(SUCCEEDED(hr) && out && *out) PatchFactory((IUnknown*)*out);
    return hr;
}
extern "C" HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory fn=(PFN_CreateFactory)g_real_CreateDXGIFactory1;
    if(!fn || fn==(PFN_CreateFactory)&ProxyMissingExport) return E_NOINTERFACE;
    HRESULT hr=fn(riid,out);
    if(SUCCEEDED(hr) && out && *out) PatchFactory((IUnknown*)*out);
    return hr;
}
extern "C" HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void** out) {
    EnsureRealDxgi();
    PFN_CreateFactory2 fn=(PFN_CreateFactory2)g_real_CreateDXGIFactory2;
    if(!fn || fn==(PFN_CreateFactory2)&ProxyMissingExport) return E_NOINTERFACE;
    HRESULT hr=fn(flags,riid,out);
    if(SUCCEEDED(hr) && out && *out) PatchFactory((IUnknown*)*out);
    return hr;
}

extern "C" BOOL WINAPI DllMain(HMODULE module, DWORD reason, void*) {
    if(reason==DLL_PROCESS_ATTACH) {
        g_self=module;
        DisableThreadLibraryCalls(module);
        // Deliberately no LoadLibrary, DXGI call, thread creation or hook here.
    }
    return TRUE;
}
