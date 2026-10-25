#include "UpdateFeature.h"
#include "OverlayBridge.h"
#include <windows.h>
#include <winhttp.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
#include <regex>
#include <vector>
#include <stdexcept>
#include "json.hpp"
namespace postal::update {
namespace {
namespace fs=std::filesystem;
std::mutex mutex;
Status state;
struct Package{std::string tag,url,hash,name;long long size=0;};
Package selected;
constexpr char current[]="V2A42-H104";
std::wstring Wide(const std::string& s){int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s.data(),(int)s.size(),nullptr,0);if(n<=0)throw std::runtime_error("Invalid text");std::wstring out(n,0);MultiByteToWideChar(CP_UTF8,0,s.data(),(int)s.size(),out.data(),n);return out;}
struct Internet{HINTERNET h;~Internet(){if(h)WinHttpCloseHandle(h);}};
std::string Fetch(bool beta){
    Internet session{WinHttpOpen(L"PostalBorkenMenu/H99",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,nullptr,nullptr,0)};
    if(!session.h)throw std::runtime_error("Network session failed");
    WinHttpSetTimeouts(session.h,10000,10000,15000,15000);
    Internet server{WinHttpConnect(session.h,L"api.github.com",443,0)};
    Internet request{server.h?WinHttpOpenRequest(server.h,L"GET",beta?L"/repos/DeadneM/PostalBorkenMenu/releases?per_page=50":L"/repos/DeadneM/PostalBorkenMenu/releases/latest",nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE):nullptr};
    if(!request.h || !WinHttpSendRequest(request.h,L"Accept: application/vnd.github+json\r\n",(DWORD)-1,nullptr,0,0,0)||!WinHttpReceiveResponse(request.h,nullptr))throw std::runtime_error("GitHub request failed");
    DWORD code=0,len=sizeof(code);WinHttpQueryHeaders(request.h,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,nullptr,&code,&len,nullptr);
    if(code!=200)throw std::runtime_error("GitHub HTTP "+std::to_string(code));
    std::string result;DWORD available=0;
    while(WinHttpQueryDataAvailable(request.h,&available)&&available){if(result.size()+available>4*1024*1024)throw std::runtime_error("Response too large");std::vector<char> buffer(available);DWORD got=0;if(!WinHttpReadData(request.h,buffer.data(),available,&got)||!got)throw std::runtime_error("Incomplete response");result.append(buffer.data(),got);}
    return result;
}
void Finish(const std::string& text){std::lock_guard<std::mutex> guard(mutex);state.busy=false;state.message=text;}
void Pin(){HMODULE m=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,(LPCWSTR)&Pin,&m))throw std::runtime_error("Cannot secure updater lifetime");}
fs::path GameDir(){HMODULE m=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)&GameDir,&m))throw std::runtime_error("Module not found");wchar_t path[32768];DWORD n=GetModuleFileNameW(m,path,32768);if(!n||n>=32768)throw std::runtime_error("Invalid module path");auto root=fs::path(path).parent_path();if(!fs::exists(root/L"POSTAL Brain Damaged.exe"))throw std::runtime_error("Game EXE not beside mod");return root;}
void Launch(const Package& p,bool restore){
    HMODULE module=nullptr;GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)&Launch,&module);
    HRSRC resource=FindResourceW(module,MAKEINTRESOURCEW(101),RT_RCDATA);
    HGLOBAL loaded=resource?LoadResource(module,resource):nullptr;DWORD size=resource?SizeofResource(module,resource):0;
    const char* bytes=loaded?(const char*)LockResource(loaded):nullptr;if(!bytes||size<1000)throw std::runtime_error("Embedded updater missing");
    wchar_t local[32768];DWORD count=GetEnvironmentVariableW(L"LOCALAPPDATA",local,32768);if(!count||count>=32768)throw std::runtime_error("LOCALAPPDATA unavailable");
    fs::path dir=fs::path(local)/L"PostalBorkenMenuUpdater";fs::create_directories(dir);
    fs::path script=dir/(L"Updater-"+std::to_wstring(GetCurrentProcessId())+L".ps1");
    {std::ofstream stream(script,std::ios::binary|std::ios::trunc);stream.write(bytes,size);if(!stream.good())throw std::runtime_error("Cannot stage updater");}
    std::wstring args=L"powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \""+script.wstring()+L"\" -GameDir \""+GameDir().wstring()+L"\" -ParentPid "+std::to_wstring(GetCurrentProcessId())+L" -Mode "+(restore?L"Restore":L"Install");
    if(!restore)args+=L" -Tag \""+Wide(p.tag)+L"\" -Url \""+Wide(p.url)+L"\" -Digest "+Wide(p.hash)+L" -ExpectedSize "+std::to_wstring(p.size);
    wchar_t system[32768];GetSystemDirectoryW(system,32768);fs::path exe=fs::path(system)/L"WindowsPowerShell/v1.0/powershell.exe";
    STARTUPINFOW si{};si.cb=sizeof(si);PROCESS_INFORMATION pi{};
    if(!CreateProcessW(exe.c_str(),args.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi))throw std::runtime_error("Cannot launch post-exit updater");CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
    std::lock_guard<std::mutex> guard(mutex);state.busy=false;state.queued=true;state.message=restore?"Rollback queued. Close the game; INI preserved.":"Update queued. Close the game; INI preserved.";
}
}
Status GetStatus(){std::lock_guard<std::mutex> guard(mutex);return state;}
void Check(bool beta){
    {std::lock_guard<std::mutex> guard(mutex);if(state.busy||state.queued)return;state.busy=true;state.available=false;state.message="Checking GitHub...";}
    try{Pin();std::thread([beta]{try{
        auto list=nlohmann::json::parse(Fetch(beta));if(!beta)list=nlohmann::json::array({list});
        Package best;int bestRank=-1;
        const std::regex tags("^V2A42-H([0-9]+)([A-Z]?)(-TEST)?$");
        for(const auto& release:list){if(release.value("draft",true)||(!beta&&release.value("prerelease",true)))continue;std::string tag=release.value("tag_name",std::string{});std::smatch match;if(!std::regex_match(tag,match,tags))continue;int rank=std::stoi(match[1])*100+(match[2].str().empty()?0:match[2].str()[0]-'A'+1);
            for(const auto& asset:release.at("assets")){if(!asset.contains("digest")||!asset["digest"].is_string())continue;std::string name=asset.value("name",std::string{}),url=asset.value("browser_download_url",std::string{}),digest=asset.value("digest",std::string{});long long size=asset.value("size",0LL);
                if(name.rfind("POSTAL_Brain_Damaged_PostalBorkenMenu_V2A42_H",0)!=0 || name.size()<4 || name.substr(name.size()-4)!=".zip" || name.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-")!=std::string::npos)continue;
                if(url!="https://github.com/DeadneM/PostalBorkenMenu/releases/download/"+tag+"/"+name || !std::regex_match(digest,std::regex("sha256:[a-fA-F0-9]{64}")) || size<1000 || size>32*1024*1024)continue;
                if(rank>bestRank){bestRank=rank;best={tag,url,digest.substr(7),name,size};}
            }
        }
        if(bestRank<0)throw std::runtime_error("No valid SHA256 package in channel");
        std::lock_guard<std::mutex> guard(mutex);selected=best;state.busy=false;state.beta=beta;state.tag=best.tag;state.available=best.tag!=current;state.message=best.tag==current?"This is the selected channel's newest build.":"Selected: "+best.tag+". Install requires confirmation; may replace beta with older stable.";
    }catch(const std::exception& e){Finish(std::string("Check failed: ")+e.what());}}).detach();}catch(const std::exception& e){Finish(e.what());}
}
void InstallAfterExit(){Package p;{std::lock_guard<std::mutex> guard(mutex);if(state.busy||state.queued||!state.available)return;state.busy=true;p=selected;}try{Pin();std::thread([p]{try{Launch(p,false);}catch(const std::exception& e){Finish(e.what());}}).detach();}catch(const std::exception& e){Finish(e.what());}}
void RestoreAfterExit(){{std::lock_guard<std::mutex> guard(mutex);if(state.busy||state.queued)return;state.busy=true;}try{Pin();std::thread([]{try{Launch({},true);}catch(const std::exception& e){Finish(e.what());}}).detach();}catch(const std::exception& e){Finish(e.what());}}
}
