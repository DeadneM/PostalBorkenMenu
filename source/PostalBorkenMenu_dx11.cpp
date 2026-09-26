// PostalBorkenMenu V2A48
// Gori-style in-swapchain overlay for POSTAL: Brain-Damaged.
// POSTAL uses native Direct3D 11, so no D3D11On12 bridge is required.
//
// This translation unit deliberately owns only the rendering hook.
// Game/IL2CPP commands remain in the V2A42 core.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

extern "C" {
int __stdcall PostalOverlayIsVisible();
void __stdcall PostalOverlayPaintToDC(HDC dc);
void __stdcall PostalOverlayLog(const char* text);
}

extern "C" void* memset(void* dst, int v, size_t n) {
    unsigned char* p=(unsigned char*)dst;
    while(n--) *p++=(unsigned char)v;
    return dst;
}
extern "C" void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d=(unsigned char*)dst;
    const unsigned char* s=(const unsigned char*)src;
    while(n--) *d++=*s++;
    return dst;
}

#define SAFE_RELEASE(x) do { if(x) { (x)->Release(); (x)=0; } } while(0)

typedef HRESULT (STDMETHODCALLTYPE *PresentFn)(IDXGISwapChain*,UINT,UINT);

struct PresentHookRec {
    void** slot;
    PresentFn original;
};

static PresentHookRec g_hooks[8] = {};
static int g_hookCount = 0;
static HWND g_gameWindow = 0;
static LONG g_installed = 0;
static LONG g_seenGamePresent = 0;

static IDXGISwapChain* g_swap = 0;
static ID3D11Device* g_device = 0;
static ID3D11DeviceContext* g_context = 0;
static ID3D11Texture2D* g_backBuffer = 0;
static ID3D11RenderTargetView* g_rtv = 0;

static ID3D11Texture2D* g_uiTexture = 0;
static ID3D11ShaderResourceView* g_uiSrv = 0;
static ID3D11VertexShader* g_vs = 0;
static ID3D11PixelShader* g_ps = 0;
static ID3D11SamplerState* g_sampler = 0;
static ID3D11BlendState* g_blend = 0;
static ID3D11RasterizerState* g_raster = 0;
static ID3D11DepthStencilState* g_depthOff = 0;

static HDC g_memDc = 0;
static HBITMAP g_dib = 0;
static HGDIOBJ g_oldBitmap = 0;
static unsigned char* g_dibBits = 0;

static const UINT UI_W = 760;
static const UINT UI_H = 600;
static const float UI_X = 32.0f;
static const float UI_Y = 32.0f;

static void ReleaseRenderResources() {
    SAFE_RELEASE(g_rtv);
    SAFE_RELEASE(g_backBuffer);
    SAFE_RELEASE(g_uiSrv);
    SAFE_RELEASE(g_uiTexture);
    SAFE_RELEASE(g_vs);
    SAFE_RELEASE(g_ps);
    SAFE_RELEASE(g_sampler);
    SAFE_RELEASE(g_blend);
    SAFE_RELEASE(g_raster);
    SAFE_RELEASE(g_depthOff);
    SAFE_RELEASE(g_context);
    SAFE_RELEASE(g_device);
    SAFE_RELEASE(g_swap);

    if(g_memDc) {
        if(g_oldBitmap) SelectObject(g_memDc,g_oldBitmap);
        if(g_dib) DeleteObject(g_dib);
        DeleteDC(g_memDc);
    }
    g_memDc=0;
    g_dib=0;
    g_oldBitmap=0;
    g_dibBits=0;
}

static BOOL CreateGdiSurface() {
    if(g_memDc && g_dib && g_dibBits) return TRUE;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth=(LONG)UI_W;
    bi.bmiHeader.biHeight=-(LONG)UI_H;
    bi.bmiHeader.biPlanes=1;
    bi.bmiHeader.biBitCount=32;
    bi.bmiHeader.biCompression=BI_RGB;

    g_memDc=CreateCompatibleDC(0);
    if(!g_memDc) return FALSE;

    void* bits=0;
    g_dib=CreateDIBSection(g_memDc,&bi,DIB_RGB_COLORS,&bits,0,0);
    if(!g_dib || !bits) return FALSE;

    g_dibBits=(unsigned char*)bits;
    g_oldBitmap=SelectObject(g_memDc,g_dib);
    return TRUE;
}

static const char g_vsSource[] =
"struct O{float4 p:SV_Position;float2 u:TEXCOORD0;};"
"O main(uint id:SV_VertexID){"
"float2 q=float2((id<<1)&2,id&2);"
"O o;o.u=q;o.p=float4(q.x*2.0-1.0,1.0-q.y*2.0,0.0,1.0);return o;}";

static const char g_psSource[] =
"Texture2D T:register(t0);SamplerState S:register(s0);"
"float4 main(float4 p:SV_Position,float2 u:TEXCOORD0):SV_Target{"
"float4 c=T.Sample(S,u);return float4(c.rgb,0.94);}";

static BOOL CreateShadersAndStates() {
    ID3DBlob* vsBlob=0;
    ID3DBlob* psBlob=0;
    ID3DBlob* err=0;

    HRESULT hr=D3DCompile(g_vsSource,sizeof(g_vsSource)-1,0,0,0,"main","vs_4_0",
                          D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&vsBlob,&err);
    if(FAILED(hr) || !vsBlob) {
        if(err) err->Release();
        PostalOverlayLog("[DX11] Vertex shader compile failed.");
        return FALSE;
    }
    if(err) { err->Release(); err=0; }

    hr=D3DCompile(g_psSource,sizeof(g_psSource)-1,0,0,0,"main","ps_4_0",
                  D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&psBlob,&err);
    if(FAILED(hr) || !psBlob) {
        if(err) err->Release();
        vsBlob->Release();
        PostalOverlayLog("[DX11] Pixel shader compile failed.");
        return FALSE;
    }
    if(err) { err->Release(); err=0; }

    hr=g_device->CreateVertexShader(vsBlob->GetBufferPointer(),vsBlob->GetBufferSize(),0,&g_vs);
    if(SUCCEEDED(hr))
        hr=g_device->CreatePixelShader(psBlob->GetBufferPointer(),psBlob->GetBufferSize(),0,&g_ps);
    vsBlob->Release();
    psBlob->Release();
    if(FAILED(hr) || !g_vs || !g_ps) {
        PostalOverlayLog("[DX11] Shader creation failed.");
        return FALSE;
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.Width=UI_W;
    td.Height=UI_H;
    td.MipLevels=1;
    td.ArraySize=1;
    td.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count=1;
    td.Usage=D3D11_USAGE_DYNAMIC;
    td.BindFlags=D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    hr=g_device->CreateTexture2D(&td,0,&g_uiTexture);
    if(FAILED(hr) || !g_uiTexture) {
        PostalOverlayLog("[DX11] UI texture creation failed.");
        return FALSE;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC sv = {};
    sv.Format=td.Format;
    sv.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
    sv.Texture2D.MipLevels=1;
    hr=g_device->CreateShaderResourceView(g_uiTexture,&sv,&g_uiSrv);
    if(FAILED(hr) || !g_uiSrv) return FALSE;

    D3D11_SAMPLER_DESC sd = {};
    sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU=D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV=D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MinLOD=0.0f;
    sd.MaxLOD=D3D11_FLOAT32_MAX;
    hr=g_device->CreateSamplerState(&sd,&g_sampler);
    if(FAILED(hr) || !g_sampler) return FALSE;

    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].BlendEnable=TRUE;
    bd.RenderTarget[0].SrcBlend=D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
    hr=g_device->CreateBlendState(&bd,&g_blend);
    if(FAILED(hr) || !g_blend) return FALSE;

    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode=D3D11_FILL_SOLID;
    rd.CullMode=D3D11_CULL_NONE;
    rd.DepthClipEnable=TRUE;
    rd.ScissorEnable=FALSE;
    rd.MultisampleEnable=FALSE;
    hr=g_device->CreateRasterizerState(&rd,&g_raster);
    if(FAILED(hr) || !g_raster) return FALSE;

    D3D11_DEPTH_STENCIL_DESC dd = {};
    dd.DepthEnable=FALSE;
    dd.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ZERO;
    dd.DepthFunc=D3D11_COMPARISON_ALWAYS;
    dd.StencilEnable=FALSE;
    hr=g_device->CreateDepthStencilState(&dd,&g_depthOff);
    if(FAILED(hr) || !g_depthOff) return FALSE;

    return CreateGdiSurface();
}

static BOOL EnsureBackBuffer(IDXGISwapChain* swap) {
    ID3D11Texture2D* bb=0;
    HRESULT hr=swap->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&bb);
    if(FAILED(hr) || !bb) return FALSE;

    if(bb==g_backBuffer && g_rtv) {
        bb->Release();
        return TRUE;
    }

    SAFE_RELEASE(g_rtv);
    SAFE_RELEASE(g_backBuffer);
    g_backBuffer=bb;

    hr=g_device->CreateRenderTargetView(g_backBuffer,0,&g_rtv);
    return SUCCEEDED(hr) && g_rtv;
}

static BOOL InitForSwap(IDXGISwapChain* swap) {
    if(g_swap==swap && g_device && g_context && g_uiTexture) return TRUE;

    ReleaseRenderResources();

    DXGI_SWAP_CHAIN_DESC desc = {};
    if(FAILED(swap->GetDesc(&desc))) return FALSE;
    if(g_gameWindow && desc.OutputWindow!=g_gameWindow) return FALSE;

    ID3D11Device* dev=0;
    if(FAILED(swap->GetDevice(__uuidof(ID3D11Device),(void**)&dev)) || !dev) return FALSE;

    g_swap=swap;
    g_swap->AddRef();
    g_device=dev;
    g_device->GetImmediateContext(&g_context);
    if(!g_context) return FALSE;

    if(!CreateShadersAndStates()) {
        ReleaseRenderResources();
        return FALSE;
    }

    PostalOverlayLog("[DX11] Postal swapchain captured; in-backbuffer overlay renderer initialized.");
    return TRUE;
}

static void UploadUiTexture() {
    if(!g_context || !g_uiTexture || !g_memDc || !g_dibBits) return;

    PostalOverlayPaintToDC(g_memDc);

    D3D11_MAPPED_SUBRESOURCE m = {};
    if(FAILED(g_context->Map(g_uiTexture,0,D3D11_MAP_WRITE_DISCARD,0,&m))) return;

    const UINT srcPitch=UI_W*4;
    for(UINT y=0;y<UI_H;y++) {
        unsigned char* d=(unsigned char*)m.pData + y*m.RowPitch;
        unsigned char* s=g_dibBits + y*srcPitch;
        for(UINT x=0;x<srcPitch;x++) d[x]=s[x];
    }
    g_context->Unmap(g_uiTexture,0);
}

static void DrawOverlay(IDXGISwapChain* swap) {
    if(!PostalOverlayIsVisible()) return;
    if(!InitForSwap(swap)) return;
    if(!EnsureBackBuffer(swap)) return;

    UploadUiTexture();

    D3D11_VIEWPORT oldVp[16] = {};
    UINT oldVpCount=16;
    g_context->RSGetViewports(&oldVpCount,oldVp);

    ID3D11RenderTargetView* oldRtv=0;
    ID3D11DepthStencilView* oldDsv=0;
    g_context->OMGetRenderTargets(1,&oldRtv,&oldDsv);

    ID3D11BlendState* oldBlend=0;
    FLOAT oldBlendFactor[4]={0,0,0,0};
    UINT oldMask=0;
    g_context->OMGetBlendState(&oldBlend,oldBlendFactor,&oldMask);

    ID3D11DepthStencilState* oldDepth=0;
    UINT oldStencilRef=0;
    g_context->OMGetDepthStencilState(&oldDepth,&oldStencilRef);

    ID3D11RasterizerState* oldRaster=0;
    g_context->RSGetState(&oldRaster);

    ID3D11InputLayout* oldLayout=0;
    g_context->IAGetInputLayout(&oldLayout);
    D3D11_PRIMITIVE_TOPOLOGY oldTopo=D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    g_context->IAGetPrimitiveTopology(&oldTopo);

    ID3D11VertexShader* oldVs=0;
    ID3D11PixelShader* oldPs=0;
    ID3D11GeometryShader* oldGs=0;
    ID3D11HullShader* oldHs=0;
    ID3D11DomainShader* oldDs=0;
    g_context->VSGetShader(&oldVs,0,0);
    g_context->PSGetShader(&oldPs,0,0);
    g_context->GSGetShader(&oldGs,0,0);
    g_context->HSGetShader(&oldHs,0,0);
    g_context->DSGetShader(&oldDs,0,0);

    ID3D11ShaderResourceView* oldSrv=0;
    ID3D11SamplerState* oldSampler=0;
    g_context->PSGetShaderResources(0,1,&oldSrv);
    g_context->PSGetSamplers(0,1,&oldSampler);

    DXGI_SWAP_CHAIN_DESC desc = {};
    swap->GetDesc(&desc);
    FLOAT vpW=(FLOAT)UI_W;
    FLOAT vpH=(FLOAT)UI_H;
    if(desc.BufferDesc.Width && UI_X+vpW>(FLOAT)desc.BufferDesc.Width)
        vpW=(FLOAT)desc.BufferDesc.Width-UI_X;
    if(desc.BufferDesc.Height && UI_Y+vpH>(FLOAT)desc.BufferDesc.Height)
        vpH=(FLOAT)desc.BufferDesc.Height-UI_Y;
    if(vpW<1.0f || vpH<1.0f) goto restore_state;

    D3D11_VIEWPORT vp = {};
    vp.TopLeftX=UI_X;
    vp.TopLeftY=UI_Y;
    vp.Width=vpW;
    vp.Height=vpH;
    vp.MinDepth=0.0f;
    vp.MaxDepth=1.0f;

    {
        FLOAT factor[4]={0,0,0,0};
        g_context->OMSetRenderTargets(1,&g_rtv,0);
        g_context->OMSetBlendState(g_blend,factor,0xffffffffu);
        g_context->OMSetDepthStencilState(g_depthOff,0);
        g_context->RSSetState(g_raster);
        g_context->RSSetViewports(1,&vp);

        g_context->IASetInputLayout(0);
        g_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_context->VSSetShader(g_vs,0,0);
        g_context->PSSetShader(g_ps,0,0);
        g_context->GSSetShader(0,0,0);
        g_context->HSSetShader(0,0,0);
        g_context->DSSetShader(0,0,0);
        g_context->PSSetShaderResources(0,1,&g_uiSrv);
        g_context->PSSetSamplers(0,1,&g_sampler);
        g_context->Draw(3,0);
    }

restore_state:
    g_context->PSSetShaderResources(0,1,&oldSrv);
    g_context->PSSetSamplers(0,1,&oldSampler);
    g_context->VSSetShader(oldVs,0,0);
    g_context->PSSetShader(oldPs,0,0);
    g_context->GSSetShader(oldGs,0,0);
    g_context->HSSetShader(oldHs,0,0);
    g_context->DSSetShader(oldDs,0,0);
    g_context->IASetInputLayout(oldLayout);
    g_context->IASetPrimitiveTopology(oldTopo);
    if(oldVpCount) g_context->RSSetViewports(oldVpCount,oldVp);
    g_context->RSSetState(oldRaster);
    g_context->OMSetDepthStencilState(oldDepth,oldStencilRef);
    g_context->OMSetBlendState(oldBlend,oldBlendFactor,oldMask);
    g_context->OMSetRenderTargets(1,&oldRtv,oldDsv);

    SAFE_RELEASE(oldSrv);
    SAFE_RELEASE(oldSampler);
    SAFE_RELEASE(oldVs);
    SAFE_RELEASE(oldPs);
    SAFE_RELEASE(oldGs);
    SAFE_RELEASE(oldHs);
    SAFE_RELEASE(oldDs);
    SAFE_RELEASE(oldLayout);
    SAFE_RELEASE(oldRaster);
    SAFE_RELEASE(oldDepth);
    SAFE_RELEASE(oldBlend);
    SAFE_RELEASE(oldRtv);
    SAFE_RELEASE(oldDsv);
}

static PresentFn FindOriginal(IDXGISwapChain* swap) {
    if(!swap) return 0;
    void** vt=*(void***)swap;
    void** slot=&vt[8];
    for(int i=0;i<g_hookCount;i++)
        if(g_hooks[i].slot==slot) return g_hooks[i].original;
    return g_hookCount ? g_hooks[0].original : 0;
}

static HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* swap,UINT sync,UINT flags) {
    PresentFn original=FindOriginal(swap);

    DXGI_SWAP_CHAIN_DESC desc = {};
    if(SUCCEEDED(swap->GetDesc(&desc)) && (!g_gameWindow || desc.OutputWindow==g_gameWindow)) {
        if(!InterlockedExchange(&g_seenGamePresent,1))
            PostalOverlayLog("[DX11] Game Present intercepted. Gori-style in-swapchain path active.");
        DrawOverlay(swap);
    }

    return original ? original(swap,sync,flags) : S_OK;
}

static BOOL PatchSwapchainVtable(IDXGISwapChain* swap) {
    if(!swap || g_hookCount>=8) return FALSE;

    void** vt=*(void***)swap;
    void** slot=&vt[8];

    for(int i=0;i<g_hookCount;i++)
        if(g_hooks[i].slot==slot) return TRUE;

    DWORD oldProtect=0;
    if(!VirtualProtect(slot,sizeof(void*),PAGE_EXECUTE_READWRITE,&oldProtect)) return FALSE;

    PresentFn original=(PresentFn)(*slot);
    *slot=(void*)&HookPresent;

    DWORD ignored=0;
    VirtualProtect(slot,sizeof(void*),oldProtect,&ignored);

    g_hooks[g_hookCount].slot=slot;
    g_hooks[g_hookCount].original=original;
    ++g_hookCount;
    return TRUE;
}

static BOOL CreateLegacyProbe(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width=16;
    sd.BufferDesc.Height=16;
    sd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count=1;
    sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount=1;
    sd.OutputWindow=hwnd;
    sd.Windowed=TRUE;
    sd.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swap=0;
    ID3D11Device* dev=0;
    ID3D11DeviceContext* ctx=0;
    D3D_FEATURE_LEVEL fl=D3D_FEATURE_LEVEL_11_0;

    HRESULT hr=D3D11CreateDeviceAndSwapChain(
        0,D3D_DRIVER_TYPE_HARDWARE,0,0,
        0,0,D3D11_SDK_VERSION,&sd,&swap,&dev,&fl,&ctx);

    BOOL ok=FALSE;
    if(SUCCEEDED(hr) && swap) {
        ok=PatchSwapchainVtable(swap);

        IDXGISwapChain1* swap1=0;
        if(SUCCEEDED(swap->QueryInterface(__uuidof(IDXGISwapChain1),(void**)&swap1)) && swap1) {
            PatchSwapchainVtable((IDXGISwapChain*)swap1);
            swap1->Release();
        }
    }

    SAFE_RELEASE(ctx);
    SAFE_RELEASE(dev);
    SAFE_RELEASE(swap);
    return ok;
}

static BOOL CreateFlipProbe(HWND hwnd, HWND hwnd2) {
    ID3D11Device* dev=0;
    ID3D11DeviceContext* ctx=0;
    D3D_FEATURE_LEVEL fl=D3D_FEATURE_LEVEL_11_0;
    HRESULT hr=D3D11CreateDevice(0,D3D_DRIVER_TYPE_HARDWARE,0,0,0,0,D3D11_SDK_VERSION,&dev,&fl,&ctx);
    if(FAILED(hr) || !dev) {
        SAFE_RELEASE(ctx);
        SAFE_RELEASE(dev);
        return FALSE;
    }

    IDXGIDevice* dxgiDev=0;
    IDXGIAdapter* adapter=0;
    IDXGIFactory2* factory=0;
    IDXGISwapChain1* swap1=0;
    BOOL ok=FALSE;

    if(SUCCEEDED(dev->QueryInterface(__uuidof(IDXGIDevice),(void**)&dxgiDev)) && dxgiDev &&
       SUCCEEDED(dxgiDev->GetAdapter(&adapter)) && adapter &&
       SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory2),(void**)&factory)) && factory) {

        DXGI_SWAP_CHAIN_DESC1 sd1 = {};
        sd1.Width=16;
        sd1.Height=16;
        sd1.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
        sd1.SampleDesc.Count=1;
        sd1.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd1.BufferCount=2;
        sd1.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd1.Scaling=DXGI_SCALING_STRETCH;
        sd1.AlphaMode=DXGI_ALPHA_MODE_IGNORE;

        if(SUCCEEDED(factory->CreateSwapChainForHwnd(dev,hwnd2,&sd1,0,0,&swap1)) && swap1) {
            ok=PatchSwapchainVtable((IDXGISwapChain*)swap1);
        }
    }

    SAFE_RELEASE(swap1);
    SAFE_RELEASE(factory);
    SAFE_RELEASE(adapter);
    SAFE_RELEASE(dxgiDev);
    SAFE_RELEASE(ctx);
    SAFE_RELEASE(dev);
    return ok;
}

extern "C" BOOL __stdcall PostalDx11Install(HWND gameWindow) {
    if(InterlockedCompareExchange(&g_installed,1,0)!=0) return TRUE;
    g_gameWindow=gameWindow;

    HINSTANCE inst=GetModuleHandleW(0);
    HWND w1=CreateWindowExW(0,L"STATIC",L"",WS_OVERLAPPED,0,0,16,16,0,0,inst,0);
    HWND w2=CreateWindowExW(0,L"STATIC",L"",WS_OVERLAPPED,20,20,16,16,0,0,inst,0);
    if(!w1 || !w2) {
        if(w1) DestroyWindow(w1);
        if(w2) DestroyWindow(w2);
        PostalOverlayLog("[DX11] Could not create transient probe windows.");
        g_installed=0;
        return FALSE;
    }

    BOOL legacy=CreateLegacyProbe(w1);
    BOOL flip=CreateFlipProbe(w1,w2);

    DestroyWindow(w2);
    DestroyWindow(w1);

    if(!legacy && !flip) {
        PostalOverlayLog("[DX11] Failed to patch any D3D11 swapchain Present vtable.");
        g_installed=0;
        return FALSE;
    }

    PostalOverlayLog("[DX11] Present vtable hook installed using transient D3D11 probes.");
    return TRUE;
}

extern "C" void __stdcall PostalDx11Shutdown() {
    if(!g_installed) return;

    for(int i=0;i<g_hookCount;i++) {
        if(!g_hooks[i].slot || !g_hooks[i].original) continue;
        DWORD oldProtect=0;
        if(VirtualProtect(g_hooks[i].slot,sizeof(void*),PAGE_EXECUTE_READWRITE,&oldProtect)) {
            *g_hooks[i].slot=(void*)g_hooks[i].original;
            DWORD ignored=0;
            VirtualProtect(g_hooks[i].slot,sizeof(void*),oldProtect,&ignored);
        }
    }

    ReleaseRenderResources();
    g_hookCount=0;
    g_seenGamePresent=0;
    g_installed=0;
    PostalOverlayLog("[DX11] Present vtable hooks restored and overlay renderer released.");
}
