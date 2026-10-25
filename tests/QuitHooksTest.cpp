#include "OverlayBridge.h"
#include <cstdio>
static volatile int fences=0,calls=0,invalid=0,work=0;
static int tag=123,object=456;
extern "C" void PBUI_BeginQuit(){++fences;}
__declspec(noinline) void StaticExit(const void* method){
    for(int i=0;i<16;i++)++work;
    if(!fences || method!=&tag)++invalid;++calls;
}
__declspec(noinline) void InstanceExit(void* self,const void* method){
    for(int i=0;i<16;i++)++work;
    if(!fences || self!=&object || method!=&tag)++invalid;++calls;
}
__declspec(noinline) void ApplicationQuit(const void* method){
    for(int i=0;i<16;i++)++work;
    if(fences<2 || method!=&tag)++invalid;++calls;
}
int main(int argc,char**){
    bool instance=argc>1;
    void* game=instance?(void*)InstanceExit:(void*)StaticExit;
    if(PBUI_InstallQuitHooks(game,instance,(void*)ApplicationQuit)!=3)return 1;
    if(instance)InstanceExit(&object,&tag);else StaticExit(&tag);
    ApplicationQuit(&tag);
    if(invalid || calls!=2 || fences!=2 || work!=32)return 2;
    std::puts("PASS: fence before native quit, original forwarded once, ABI/arguments preserved");return 0;
}
