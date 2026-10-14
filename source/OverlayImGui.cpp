#include <windows.h>
#include <d3d11.h>
#include <atomic>
#include <cfloat>
#include "OverlayBridge.h"
#include "UpdateFeature.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "MinHook.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

namespace {
using PresentFn=HRESULT (STDMETHODCALLTYPE*)(IDXGISwapChain*,UINT,UINT);
using ResizeFn=HRESULT (STDMETHODCALLTYPE*)(IDXGISwapChain*,UINT,UINT,UINT,DXGI_FORMAT,UINT);
PresentFn originalPresent=nullptr;
ResizeFn originalResize=nullptr;
CRITICAL_SECTION lock;
std::atomic<bool> lockReady{false}, ready{false}, started{false};
IDXGISwapChain* chain=nullptr;
ID3D11Device* device=nullptr;
ID3D11DeviceContext* context=nullptr;
ID3D11RenderTargetView* target=nullptr;
ImGuiContext* gui=nullptr;
void ReleaseTarget(){if(target){target->Release();target=nullptr;}}
void Cleanup(){
    ready=false;
    if(gui){ImGui::SetCurrentContext(gui);ImGui_ImplDX11_Shutdown();ImGui_ImplWin32_Shutdown();ImGui::DestroyContext(gui);gui=nullptr;}
    ReleaseTarget();if(context){context->Release();context=nullptr;}if(device){device->Release();device=nullptr;}
    chain=nullptr;
}
bool CreateTarget(IDXGISwapChain* sc){
    ID3D11Texture2D* buffer=nullptr;
    if(FAILED(sc->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&buffer)))return false;
    HRESULT hr=device->CreateRenderTargetView(buffer,nullptr,&target);buffer->Release();return SUCCEEDED(hr);
}
bool Init(IDXGISwapChain* sc){
    DXGI_SWAP_CHAIN_DESC desc{};
    if(FAILED(sc->GetDesc(&desc)) || desc.OutputWindow!=(HWND)PBUI_Window())return false;
    if(FAILED(sc->GetDevice(__uuidof(ID3D11Device),(void**)&device)))return false;
    device->GetImmediateContext(&context);
    if(!context || !CreateTarget(sc)){Cleanup();return false;}
    gui=ImGui::CreateContext();ImGui::SetCurrentContext(gui);
    auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();auto& s=ImGui::GetStyle();
    s.WindowRounding=7;s.ChildRounding=5;s.FrameRounding=4;s.PopupRounding=5;s.ScrollbarRounding=5;s.GrabRounding=4;
    s.WindowPadding=ImVec2(12,12);s.FramePadding=ImVec2(8,5);s.ItemSpacing=ImVec2(8,7);
    if(!ImGui_ImplWin32_Init(desc.OutputWindow)){ImGui::DestroyContext(gui);gui=nullptr;Cleanup();return false;}
    if(!ImGui_ImplDX11_Init(device,context)){ImGui_ImplWin32_Shutdown();ImGui::DestroyContext(gui);gui=nullptr;Cleanup();return false;}
    chain=sc;ready=true;
    if(PBUI_LegacyWindow())ShowWindowAsync((HWND)PBUI_LegacyWindow(),SW_HIDE);
    PBUI_Log("[UI H98] ImGui DX11 initialized on game swap chain.");return true;
}
void Utf8(const wchar_t* source,char* dest,int cap){if(!WideCharToMultiByte(CP_UTF8,0,source,-1,dest,cap,nullptr,nullptr))dest[0]=0;}
void DrawMenu(){
    ImGui::SetNextWindowSize(ImVec2(990,690),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(80,80),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(650,360),ImVec2(FLT_MAX,FLT_MAX));
    bool open=true;
    if(ImGui::Begin("PostalBorkenMenu",&open,ImGuiWindowFlags_NoCollapse)){
        ImGui::TextUnformatted("POSTAL: BRAIN-DAMAGED  |  ASI Overlay H99 TEST");
        wchar_t menuKeyWide[48];PBUI_MenuKey(menuKeyWide,48);char menuKey[160];Utf8(menuKeyWide,menuKey,sizeof(menuKey));
        ImGui::SameLine();ImGui::TextDisabled("| %s to close",menuKey);ImGui::Separator();
        ImGui::TextDisabled(PBUI_Ready()?"Gameplay commands ready":"Initializing gameplay commands...");
        static const char* tabs[]={"Gameplay","Weapons","Arsenal","Special","Visual"};
        if(ImGui::BeginTabBar("MainTabs")){
            for(int tab=0;tab<5;tab++)if(ImGui::BeginTabItem(tabs[tab])){
                ImGui::Spacing();
                if(ImGui::BeginTable("Commands",6,ImGuiTableFlags_RowBg|ImGuiTableFlags_BordersInnerH|ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_ScrollY,ImVec2(0,ImGui::GetContentRegionAvail().y-28))){
                    ImGui::TableSetupColumn("Action",ImGuiTableColumnFlags_WidthStretch,3.0f);
                    ImGui::TableSetupColumn("Argument",ImGuiTableColumnFlags_WidthStretch,1.5f);
                    ImGui::TableSetupColumn("Run",ImGuiTableColumnFlags_WidthFixed,85);
                    ImGui::TableSetupColumn("Current key",ImGuiTableColumnFlags_WidthFixed,105);
                    ImGui::TableSetupColumn("Change",ImGuiTableColumnFlags_WidthFixed,95);
                    ImGui::TableSetupColumn("Reset",ImGuiTableColumnFlags_WidthFixed,65);
                    ImGui::TableSetupScrollFreeze(0,1);ImGui::TableHeadersRow();
                    // Header column 4 and GodMode command index 4 formerly shared
                    // PushID(4)+"Change". Give body rows their own ID namespace.
                    ImGui::PushID("CommandRows");
                    for(int i=0;i<PBUI_Count();i++){
                        PBUiCommand row{};if(!PBUI_Get(i,&row)||row.tab!=tab)continue;
                        char label[512],arg[384],key[160];Utf8(row.label,label,sizeof(label));Utf8(row.argument,arg,sizeof(arg));Utf8(row.key,key,sizeof(key));
                        ImGui::PushID(i);ImGui::TableNextRow();ImGui::TableSetColumnIndex(0);ImGui::TextWrapped("%s",label);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::BeginDisabled(!PBUI_Ready());
                        if(row.kind==1){
                            ImGui::SetNextItemWidth(-1);
                            if(ImGui::BeginCombo("##Speed",arg)){
                                const char* speeds[]={"0.5","1","2"};
                                for(int k=0;k<3;k++)if(ImGui::Selectable(speeds[k]))PBUI_Request(i,6,k);
                                ImGui::EndCombo();
                            }
                        }else if(row.kind==2){
                            ImGui::SetNextItemWidth(-1);
                            if(ImGui::BeginCombo("##Mode",arg)){
                                const char* modes[]={"OFF","BASE","DLC","ALT"};
                                for(int k=0;k<4;k++)if(ImGui::Selectable(modes[k]))PBUI_Request(i,7,k);
                                ImGui::EndCombo();
                            }
                        }else if(row.kind==3){if(ImGui::Button(arg[0]?arg:"Next slot",ImVec2(-1,0)))PBUI_Request(i,4,0);}
                        else if(arg[0])ImGui::TextUnformatted(arg);
                        ImGui::TableSetColumnIndex(2);
                        if(row.kind==4 || row.kind==5){bool enabled=row.enabled!=0;if(ImGui::Checkbox("##Enabled",&enabled))PBUI_Request(i,0,0);}
                        else if(ImGui::Button("Run",ImVec2(-1,0)))PBUI_Request(i,0,0);
                        ImGui::EndDisabled();
                        ImGui::TableSetColumnIndex(3);
                        if(row.kind==4)ImGui::TextDisabled("Next launch");else ImGui::TextWrapped("%s",key);
                        ImGui::TableSetColumnIndex(4);ImGui::BeginDisabled(row.kind==4);
                        if(ImGui::Button(row.capturing?"Press key...":"Change",ImVec2(-1,0)))PBUI_Request(i,1,0);
                        if(ImGui::IsItemHovered())ImGui::SetTooltip("Press a key. Escape cancels; Backspace clears.");
                        ImGui::TableSetColumnIndex(5);if(ImGui::Button("Default",ImVec2(-1,0)))PBUI_Request(i,3,0);
                        ImGui::EndDisabled();ImGui::PopID();
                    }
                    ImGui::PopID();ImGui::EndTable();
                }
                ImGui::TextDisabled("Drag the title bar to move | Resize from the corner | Keyboard navigation enabled");
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Hotkeys")){
                ImGui::TextUnformatted("Open / close mod menu");ImGui::SameLine();ImGui::TextUnformatted(menuKey);
                if(ImGui::Button(PBUI_MenuCapturing()?"Press key...##MenuBinding":"Change##MenuBinding"))PBUI_Request(-1,8,0);
                ImGui::SameLine();if(ImGui::Button("Default (F1)##MenuBinding"))PBUI_Request(-1,9,0);
                ImGui::TextWrapped("Escape cancels. The menu key must differ from command keys. Command bindings are editable in each tab.");
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Updates")){
                auto update=postal::update::GetStatus();bool beta=PBUI_Beta()!=0;
                ImGui::TextUnformatted("Current build: V2A42-H99-TEST");
                ImGui::BeginDisabled(update.busy||update.queued);
                if(ImGui::Checkbox("Beta updates (experimental)",&beta))PBUI_Request(-1,10,beta?1:0);
                if(ImGui::Button("Check for Updates"))postal::update::Check(beta);
                ImGui::EndDisabled();ImGui::TextWrapped("%s",update.message.c_str());
                ImGui::BeginDisabled(update.busy||update.queued||!update.available||update.beta!=beta);
                if(ImGui::Button("Install after Exit"))ImGui::OpenPopup("Confirm update");
                ImGui::EndDisabled();
                if(ImGui::BeginPopupModal("Confirm update",nullptr,ImGuiWindowFlags_AlwaysAutoResize)){
                    ImGui::Text("Install %s after closing the game?",update.tag.c_str());
                    ImGui::TextWrapped("This may replace an experimental build with an older stable version. Current mod files are backed up; your INI is preserved.");
                    if(ImGui::Button("Confirm install")){postal::update::InstallAfterExit();ImGui::CloseCurrentPopup();}
                    ImGui::SameLine();if(ImGui::Button("Cancel##Update"))ImGui::CloseCurrentPopup();ImGui::EndPopup();
                }
                ImGui::BeginDisabled(update.busy||update.queued);
                if(ImGui::Button("Restore previous version after Exit"))ImGui::OpenPopup("Confirm rollback");
                ImGui::EndDisabled();
                if(ImGui::BeginPopupModal("Confirm rollback",nullptr,ImGuiWindowFlags_AlwaysAutoResize)){
                    ImGui::TextWrapped("Restore the previous mod binaries after exiting? Personal INI remains unchanged.");
                    if(ImGui::Button("Confirm restore")){postal::update::RestoreAfterExit();ImGui::CloseCurrentPopup();}
                    ImGui::SameLine();if(ImGui::Button("Cancel##Rollback"))ImGui::CloseCurrentPopup();ImGui::EndPopup();
                }
                ImGui::TextWrapped("Updates run only after game exit. SHA256, ZIP root and x64 binaries are verified before replacement. Details: PostalBorkenMenu-update-result.txt beside the game.");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();if(!open)PBUI_Request(-1,5,0);
}
HRESULT STDMETHODCALLTYPE Present(IDXGISwapChain* sc,UINT sync,UINT flags){
    if(flags&DXGI_PRESENT_TEST)return originalPresent(sc,sync,flags);
    EnterCriticalSection(&lock);
    if(PBUI_Stopping()){if(gui)Cleanup();LeaveCriticalSection(&lock);return originalPresent(sc,sync,flags);}
    if(!chain)Init(sc);
    if(sc==chain && ready && PBUI_Visible()){
        ImGui::SetCurrentContext(gui);
        ClipCursor(nullptr);
        if(!target)CreateTarget(sc);
        if(target){
            ImGui::GetIO().MouseDrawCursor=true;
            ImGui_ImplDX11_NewFrame();ImGui_ImplWin32_NewFrame();ImGui::NewFrame();DrawMenu();ImGui::Render();
            ID3D11RenderTargetView* oldTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
            ID3D11DepthStencilView* oldDepth=nullptr;
            context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,oldTargets,&oldDepth);
            context->OMSetRenderTargets(1,&target,nullptr);ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,oldTargets,oldDepth);
            for(auto* old:oldTargets)if(old)old->Release();if(oldDepth)oldDepth->Release();
        }
    }
    LeaveCriticalSection(&lock);return originalPresent(sc,sync,flags);
}
HRESULT STDMETHODCALLTYPE Resize(IDXGISwapChain* sc,UINT count,UINT width,UINT height,DXGI_FORMAT format,UINT flags){
    EnterCriticalSection(&lock);if(sc==chain)ReleaseTarget();
    HRESULT hr=originalResize(sc,count,width,height,format,flags);
    LeaveCriticalSection(&lock);return hr;
}
DWORD WINAPI Install(void*){
    InitializeCriticalSection(&lock);lockReady=true;
    HINSTANCE module=GetModuleHandleW(nullptr);
    WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=module;wc.lpszClassName=L"PostalH98Probe";
    RegisterClassW(&wc);HWND window=CreateWindowW(wc.lpszClassName,L"",WS_OVERLAPPED,0,0,8,8,nullptr,nullptr,module,nullptr);
    IDXGISwapChain* dummy=nullptr;ID3D11Device* d=nullptr;ID3D11DeviceContext* c=nullptr;
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferCount=1;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.OutputWindow=window;desc.SampleDesc.Count=1;desc.Windowed=TRUE;
    HRESULT hr=window?D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&desc,&dummy,&d,nullptr,&c):E_FAIL;
    if(SUCCEEDED(hr)){
        void** table=*(void***)dummy;
        if(MH_Initialize()==MH_OK && MH_CreateHook(table[8],(void*)Present,(void**)&originalPresent)==MH_OK && MH_CreateHook(table[13],(void*)Resize,(void**)&originalResize)==MH_OK){
            if(MH_EnableHook(table[13])==MH_OK && MH_EnableHook(table[8])==MH_OK)PBUI_Log("[UI H98] Present/Resize hooks installed; waiting for game DX11 swap chain.");
            else PBUI_Log("[UI H98] hook activation failed; legacy menu remains available.");
        }else PBUI_Log("[UI H98] hook installation failed; legacy menu remains available.");
    }else PBUI_Log("[UI H98] DX11 probe failed; legacy menu remains available.");
    if(c)c->Release();if(d)d->Release();if(dummy)dummy->Release();if(window)DestroyWindow(window);UnregisterClassW(wc.lpszClassName,module);return 0;
}
}
extern "C" void PBUI_StartRenderer(){if(started.exchange(true))return;HANDLE thread=CreateThread(nullptr,0,Install,nullptr,0,nullptr);if(thread)CloseHandle(thread);}
extern "C" int PBUI_RendererReady(){return ready.load()?1:0;}
extern "C" int PBUI_Message(void* window,unsigned int msg,unsigned long long wp,long long lp){
    if(!lockReady || !ready || !PBUI_Visible() || PBUI_Stopping())return 0;
    EnterCriticalSection(&lock);if(!gui){LeaveCriticalSection(&lock);return 0;}ImGui::SetCurrentContext(gui);
    ImGui_ImplWin32_WndProcHandler((HWND)window,msg,(WPARAM)wp,(LPARAM)lp);
    bool mouse=(msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)||msg==WM_INPUT;
    bool keyboard=(msg>=WM_KEYFIRST && msg<=WM_KEYLAST);
    bool swallow=mouse||keyboard;LeaveCriticalSection(&lock);return swallow?1:0;
}
