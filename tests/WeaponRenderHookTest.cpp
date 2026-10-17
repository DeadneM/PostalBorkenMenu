#include <windows.h>
#include <cstdio>
#include <cstring>
#include "MinHook.h"
extern "C" bool PBTestInstall(void*);
extern "C" void PBUI_Log(const char* text){std::puts(text);}
int main(){
    // Audited native Update fixture. Relative calls must never run in this isolated process.
    unsigned char code[]={0x40,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xf9,0x33,0xd2,0xb9,0x1c,0x01,0x00,0x00,0xe8,0x3b,0xa6,0x4b,0x05,0x84,0xc0,0x74,0x37,0x48,0x8b,0x4f,0x50,0x33,0xd2,0x48,0x89,0x5c,0x24,0x30,0xe8,0xf7,0x00,0x2a,0x05,0x48,0x8b,0x4f,0x50,0x33,0xd2,0x48,0x8b,0xd8,0xe8,0xe9,0x00,0x2a,0x05,0x48,0x85,0xc0,0x74,0x1a,0x48,0x85,0xdb,0x74,0x1b,0x80,0x78,0x44,0x00,0x0f,0x94,0xc1,0x88,0x4b,0x44,0x48,0x8b,0x5c,0x24,0x30,0x48,0x83,0xc4,0x20,0x5f,0xc3};
    void* target=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!target)return 1;
    std::memcpy(target,code,sizeof(code));
    ((unsigned char*)target)[11]^=1;
    if(PBTestInstall(target)){std::puts("FAIL: mismatched game code accepted");return 2;}
    std::memcpy(target,code,sizeof(code));
    // Simulate renderer MinHook initialization already having happened.
    auto init=MH_Initialize();if(init!=MH_OK && init!=MH_ERROR_ALREADY_INITIALIZED)return 3;
    if(!PBTestInstall(target)){std::puts("FAIL: exact native fixture rejected");return 4;}
    using Update=void(*)(void*,const void*);
    for(int i=0;i<100;i++)((Update)target)(nullptr,nullptr);
    // Successful return proves the unsafe original camera/Input path was suppressed.
    if(MH_DisableHook(target)!=MH_OK || std::memcmp(target,code,sizeof(code))!=0)return 5;
    MH_RemoveHook(target);MH_Uninitialize();VirtualFree(target,0,MEM_RELEASE);
    std::puts("PASS: mismatch rejected, exact native hook invoked 100 times, original bytes restored");return 0;
}
