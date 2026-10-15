#pragma once
struct PBUiCommand {
    wchar_t label[160], argument[128], key[48];
    int tab, kind, enabled, capturing;
};
extern "C" {
int PBUI_Count();
int PBUI_Get(int index,PBUiCommand* command);
int PBUI_Visible();
int PBUI_Ready();
int PBUI_Stopping();
int PBUI_MenuKey(wchar_t* key,int capacity);
int PBUI_MenuCapturing();
int PBUI_KeyName(int vk,wchar_t* text,int capacity);
int PBUI_Beta();
void* PBUI_Window();
void* PBUI_LegacyWindow();
void PBUI_Request(int index,int action,int value);
void PBUI_Log(const char* text);
void PBUI_StartRenderer();
int PBUI_RendererReady();
int PBUI_Message(void* window,unsigned int message,unsigned long long wp,long long lp);
}
