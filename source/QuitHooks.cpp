#include "OverlayBridge.h"
#include "MinHook.h"
namespace {
using StaticFn=void(*)(const void*);
using InstanceFn=void(*)(void*,const void*);
void* originalGameExit=nullptr;
StaticFn originalApplicationQuit=nullptr;
void ExitStatic(const void* method){PBUI_BeginQuit();((StaticFn)originalGameExit)(method);}
void ExitInstance(void* self,const void* method){PBUI_BeginQuit();((InstanceFn)originalGameExit)(self,method);}
void ApplicationQuit(const void* method){PBUI_BeginQuit();originalApplicationQuit(method);}
bool Install(void* target,void* callback,void** original){
    if(!target || MH_CreateHook(target,callback,original)!=MH_OK)return false;
    if(MH_EnableHook(target)==MH_OK)return true;
    MH_RemoveHook(target);return false;
}
}
extern "C" int PBUI_InstallQuitHooks(void* gameExit,int instance,void* applicationQuit){
    auto initialized=MH_Initialize();
    if(initialized!=MH_OK && initialized!=MH_ERROR_ALREADY_INITIALIZED)return 0;
    int installed=0;
    if(Install(gameExit,instance?(void*)ExitInstance:(void*)ExitStatic,&originalGameExit))installed|=1;
    if(Install(applicationQuit,(void*)ApplicationQuit,(void**)&originalApplicationQuit))installed|=2;
    return installed;
}
