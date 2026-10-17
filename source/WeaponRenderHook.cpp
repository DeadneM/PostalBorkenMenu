#include <windows.h>
#include <atomic>
#include <cstring>
#include <cstdint>
#include "MinHook.h"
#include "OverlayBridge.h"
namespace {
std::atomic<int> hookStatus{0};
void* originalUpdate=nullptr;
// Exact normal body of the audited PlayerWeaponCamera.Update (current game build).
// There is no other Update behavior: Input.GetKeyDown(F3), camera URP data, flag flip.
const unsigned char expected[]={0x40,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xf9,0x33,0xd2,0xb9,0x1c,0x01,0x00,0x00,0xe8,0x3b,0xa6,0x4b,0x05,0x84,0xc0,0x74,0x37,0x48,0x8b,0x4f,0x50,0x33,0xd2,0x48,0x89,0x5c,0x24,0x30,0xe8,0xf7,0x00,0x2a,0x05,0x48,0x8b,0x4f,0x50,0x33,0xd2,0x48,0x8b,0xd8,0xe8,0xe9,0x00,0x2a,0x05,0x48,0x85,0xc0,0x74,0x1a,0x48,0x85,0xdb,0x74,0x1b,0x80,0x78,0x44,0x00,0x0f,0x94,0xc1,0x88,0x4b,0x44,0x48,0x8b,0x5c,0x24,0x30,0x48,0x83,0xc4,0x20,0x5f,0xc3};
void IgnoreNativeF3(void*,const void*){}
bool InstallVerified(void* target){
    MEMORY_BASIC_INFORMATION page{};
    if(!VirtualQuery(target,&page,sizeof(page)) || page.State!=MEM_COMMIT ||
       (page.Protect&(PAGE_GUARD|PAGE_NOACCESS)) ||
       (uintptr_t)target+sizeof(expected)>(uintptr_t)page.BaseAddress+page.RegionSize ||
       std::memcmp(target,expected,sizeof(expected))!=0)return false;
    auto initialized=MH_Initialize();
    if(initialized!=MH_OK && initialized!=MH_ERROR_ALREADY_INITIALIZED)return false;
    if(MH_CreateHook(target,(void*)IgnoreNativeF3,&originalUpdate)!=MH_OK)return false;
    if(MH_EnableHook(target)==MH_OK)return true;
    MH_RemoveHook(target);return false;
}
}
extern "C" int PBUI_WeaponF3Status(){return hookStatus.load();}
extern "C" int PBUI_InstallWeaponF3Hook(){
    if(hookStatus.load())return hookStatus.load();
    HMODULE game=GetModuleHandleW(L"GameAssembly.dll");
    if(!game)return 0;
    auto dos=(IMAGE_DOS_HEADER*)game;
    auto nt=(IMAGE_NT_HEADERS64*)((char*)game+dos->e_lfanew);
    constexpr size_t offset=0x80e440;
    if(dos->e_magic!=IMAGE_DOS_SIGNATURE || nt->Signature!=IMAGE_NT_SIGNATURE ||
       nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64 || nt->OptionalHeader.SizeOfImage<offset+sizeof(expected) ||
       !InstallVerified((char*)game+offset)){
        hookStatus=-1;PBUI_Log("[RENDER H101] Native F3 hook NOT installed: signature/build or hook mismatch. Native F3 remains active.");
    }else{hookStatus=1;PBUI_Log("[RENDER H101] Exact PlayerWeaponCamera.Update F3-only hook installed. Native F3 toggle blocked.");}
    return hookStatus.load();
}
#ifdef PB_HOOK_TEST
extern "C" bool PBTestInstall(void* address){return InstallVerified(address);}
#endif
