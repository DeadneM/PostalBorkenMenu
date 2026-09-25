// PostalBorkenMenu V2A8 EARLY SHUTDOWN + EVENT CROSSHAIR TEST
// Native x64 ASI + IL2CPP developer-command bridge.
// UI: borderless in-process overlay shell (no title bar/taskbar entry).
// No EXE/GameAssembly patching. No CRT dependency.

extern "C" {
int _fltused = 0;

typedef void* PVOID;
typedef void* HANDLE;
typedef HANDLE HMODULE;
typedef HANDLE HINSTANCE;
typedef HANDLE HWND;
typedef HANDLE HMENU;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HBRUSH;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef unsigned long long SIZE_T;
typedef long LONG;
typedef long long LONG_PTR;
typedef unsigned long long ULONG_PTR;
typedef ULONG_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef int BOOL;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* LPWSTR;
typedef const char* LPCSTR;
typedef char* LPSTR;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(void*);
typedef LRESULT (__stdcall *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

struct POINT { LONG x; LONG y; };
struct RECT { LONG left; LONG top; LONG right; LONG bottom; };
typedef HANDLE HDC;
typedef HANDLE HGDIOBJ;
typedef unsigned long COLORREF;
struct PAINTSTRUCT { HDC hdc; BOOL fErase; RECT rcPaint; BOOL fRestore; BOOL fIncUpdate; BYTE rgbReserved[32]; };
struct MSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
    DWORD lPrivate;
};
struct WNDCLASSEXW {
    UINT cbSize;
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
    HICON hIconSm;
};
struct WINHTTP_CURRENT_USER_IE_PROXY_CONFIG {
    BOOL fAutoDetect;
    LPWSTR lpszAutoConfigUrl;
    LPWSTR lpszProxy;
    LPWSTR lpszProxyBypass;
};

__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) void* __stdcall GetProcAddress(HMODULE, LPCSTR);
__declspec(dllimport) HMODULE __stdcall LoadLibraryW(LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateThread(void*, SIZE_T, LPTHREAD_START_ROUTINE, void*, DWORD, DWORD*);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) DWORD __stdcall GetCurrentProcessId(void);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
__declspec(dllimport) UINT __stdcall GetSystemDirectoryW(LPWSTR, UINT);
__declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) UINT __stdcall GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
__declspec(dllimport) DWORD __stdcall GetPrivateProfileStringW(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, DWORD, LPCWSTR);
__declspec(dllimport) BOOL __stdcall WritePrivateProfileStringW(LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, const void*, DWORD, DWORD*, void*);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE, LONG, LONG*, DWORD);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);

__declspec(dllimport) HWND __stdcall GetForegroundWindow(void);
__declspec(dllimport) DWORD __stdcall GetWindowThreadProcessId(HWND, DWORD*);
__declspec(dllimport) LONG_PTR __stdcall SetWindowLongPtrW(HWND, int, LONG_PTR);
__declspec(dllimport) LRESULT __stdcall CallWindowProcW(WNDPROC, HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) BOOL __stdcall PostMessageW(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) BOOL __stdcall ShowWindow(HWND, int);
__declspec(dllimport) HWND __stdcall CreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void*);
__declspec(dllimport) LRESULT __stdcall DefWindowProcW(HWND, UINT, WPARAM, LPARAM);
__declspec(dllimport) WORD __stdcall RegisterClassExW(const WNDCLASSEXW*);
__declspec(dllimport) BOOL __stdcall GetMessageW(MSG*, HWND, UINT, UINT);
__declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG*);
__declspec(dllimport) LRESULT __stdcall DispatchMessageW(const MSG*);
__declspec(dllimport) BOOL __stdcall SetWindowTextW(HWND, LPCWSTR);
__declspec(dllimport) int __stdcall GetWindowTextW(HWND, LPWSTR, int);
__declspec(dllimport) HWND __stdcall SetFocus(HWND);
__declspec(dllimport) BOOL __stdcall SetForegroundWindow(HWND);
__declspec(dllimport) BOOL __stdcall IsWindow(HWND);
__declspec(dllimport) BOOL __stdcall GetClientRect(HWND, RECT*);
__declspec(dllimport) BOOL __stdcall ClientToScreen(HWND, POINT*);
__declspec(dllimport) BOOL __stdcall SetWindowPos(HWND, HWND, int, int, int, int, UINT);
__declspec(dllimport) BOOL __stdcall InvalidateRect(HWND, const RECT*, BOOL);
__declspec(dllimport) HDC __stdcall BeginPaint(HWND, PAINTSTRUCT*);
__declspec(dllimport) BOOL __stdcall EndPaint(HWND, const PAINTSTRUCT*);
__declspec(dllimport) int __stdcall FillRect(HDC, const RECT*, HBRUSH);
__declspec(dllimport) int __stdcall DrawTextW(HDC, LPCWSTR, int, RECT*, UINT);
__declspec(dllimport) BOOL __stdcall SetLayeredWindowAttributes(HWND, COLORREF, BYTE, DWORD);
__declspec(dllimport) short __stdcall GetAsyncKeyState(int);
__declspec(dllimport) BOOL __stdcall PeekMessageW(MSG*, HWND, UINT, UINT, UINT);
__declspec(dllimport) BOOL __stdcall DestroyWindow(HWND);
__declspec(dllimport) void __stdcall PostQuitMessage(int);

}

#define TRUE 1
#define FALSE 0
#define NULLPTR ((void*)0)
#define DLL_PROCESS_ATTACH 1
#define MAX_PATH_W 512
#define GENERIC_WRITE 0x40000000UL
#define FILE_SHARE_READ 0x00000001UL
#define FILE_SHARE_WRITE 0x00000002UL
#define OPEN_ALWAYS 4UL
#define FILE_ATTRIBUTE_NORMAL 0x00000080UL
#define FILE_END 2UL
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

#define WS_OVERLAPPED 0x00000000L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_VISIBLE 0x10000000L
#define WS_CHILD 0x40000000L
#define WS_TABSTOP 0x00010000L
#define WS_BORDER 0x00800000L
#define BS_PUSHBUTTON 0x00000000L
#define ES_AUTOHSCROLL 0x0080L
#define WS_EX_TOOLWINDOW 0x00000080L
#define WS_EX_TOPMOST 0x00000008L
#define COLOR_WINDOW 5
#define CW_USEDEFAULT ((int)0x80000000)
#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOW 5
#define GWLP_WNDPROC (-4)

#define WM_DESTROY 0x0002
#define WM_NCDESTROY 0x0082
#define WM_CLOSE 0x0010
#define WM_COMMAND 0x0111
#define WM_KEYDOWN 0x0100
#define WM_SYSKEYDOWN 0x0104
#define WM_APP 0x8000
#define WM_APP_EXEC (WM_APP + 0x31)
#define WM_APP_TOGGLE (WM_APP + 0x32)
#define WM_APP_CROSSHAIR (WM_APP + 0x33)
#define WM_APP_SHUTDOWN (WM_APP + 0x34)
#define WM_APP_WEAPONKEY (WM_APP + 0x35)
#define WM_APP_WHEELBANK (WM_APP + 0x36)

#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F24 0x87

#define EN_CHANGE 0x0300
#define WM_PAINT 0x000F
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_LBUTTONDOWN 0x0201
#define WM_MOUSEWHEEL 0x020A
#define WM_MOUSEHWHEEL 0x020E
#define WS_POPUP 0x80000000L
#define WS_EX_LAYERED 0x00080000L
#define WS_EX_TRANSPARENT 0x00000020L
#define WS_EX_NOACTIVATE 0x08000000L
#define LWA_ALPHA 0x00000002UL
#define SW_SHOWNOACTIVATE 4
#define SWP_NOSIZE 0x0001
#define SWP_NOZORDER 0x0004
#define SWP_NOACTIVATE 0x0010
#define DT_LEFT 0x0000
#define DT_CENTER 0x0001
#define DT_VCENTER 0x0004
#define DT_SINGLELINE 0x0020
#define TRANSPARENT_BK 1
#define PM_REMOVE 0x0001

static HMODULE g_self = 0;
static HWND g_menuHwnd = 0;
static HWND g_gameHwnd = 0;
static WNDPROC g_oldGameWndProc = 0;
static WCHAR g_iniPath[MAX_PATH_W];
static WCHAR g_logPath[MAX_PATH_W];
static volatile LONG g_ready = 0;
static volatile LONG g_menuVisible = 0;
static volatile LONG g_shuttingDown = 0;
static volatile LONG g_noCrosshair = 0;
static DWORD g_lastCrosshairTick = 0;
static volatile LONG g_crosshairReapplyCountdown = 0;
static int g_scrollRow = 0;
static BYTE g_altSlotFlip[10] = {};
static int g_menuTab = 0; // 0 Gameplay, 1 Weapons, 2 Weapon List, 3 Visual
static volatile LONG g_activeWheelBank = 0;
static int g_activeWheelVk = 0;
static BYTE g_keyPrev[256] = {};
static int g_bindIndex = -1;
static int g_overlayX = 32;
static int g_overlayY = 32;
static float g_lastAppliedTimeScale = 1.0f;
static const int OVERLAY_W = 760;
static const int OVERLAY_H = 600;
static const int ROW_Y = 112;
static const int ROW_H = 29;
static const int VISIBLE_ROWS = (OVERLAY_H - ROW_Y - 48) / ROW_H;

static int StrLenA(const char* s) { int n=0; if (!s) return 0; while (s[n]) ++n; return n; }
static int StrLenW(const WCHAR* s) { int n=0; if (!s) return 0; while (s[n]) ++n; return n; }
static BOOL StrEqA(const char* a, const char* b) {
    if (!a || !b) return FALSE;
    while (*a && *b) { if (*a != *b) return FALSE; ++a; ++b; }
    return *a == *b;
}
static BOOL StrStartsWithA(const char* s, const char* prefix) {
    if (!s || !prefix) return FALSE;
    while (*prefix) {
        if (*s != *prefix) return FALSE;
        ++s; ++prefix;
    }
    return TRUE;
}
static BOOL StrContainsNoCaseA(const char* s, const char* needle) {
    if (!s || !needle || !*needle) return FALSE;
    for (int i=0; s[i]; ++i) {
        int j=0;
        while (needle[j] && s[i+j]) {
            char a=s[i+j], b=needle[j];
            if (a>='A' && a<='Z') a=(char)(a-'A'+'a');
            if (b>='A' && b<='Z') b=(char)(b-'A'+'a');
            if (a!=b) break;
            ++j;
        }
        if (!needle[j]) return TRUE;
    }
    return FALSE;
}
static void CopyW(WCHAR* dst, int cap, const WCHAR* src) {
    if (!dst || cap <= 0) return;
    int i=0; if (src) for (; i<cap-1 && src[i]; ++i) dst[i]=src[i];
    dst[i]=0;
}
static void AppendW(WCHAR* dst, int cap, const WCHAR* src) {
    int d=StrLenW(dst); if (d>=cap-1) return;
    int i=0; if (src) for (; d+i<cap-1 && src[i]; ++i) dst[d+i]=src[i];
    dst[d+i]=0;
}
static void UIntToW(unsigned int v, WCHAR* out, int cap) {
    if (!out || cap<2) return;
    WCHAR tmp[16]; int n=0;
    do { tmp[n++]=(WCHAR)(L'0'+(v%10)); v/=10; } while (v && n<15);
    int j=0; while (n>0 && j<cap-1) out[j++]=tmp[--n]; out[j]=0;
}
static void HexByteW(unsigned int v, WCHAR* out) {
    static const WCHAR* h=L"0123456789ABCDEF";
    out[0]=L'0'; out[1]=L'x'; out[2]=h[(v>>4)&15]; out[3]=h[v&15]; out[4]=0;
}
static void LogA(const char* s) {
    if (!g_logPath[0] || !s) return;
    HANDLE h=CreateFileW(g_logPath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (h==INVALID_HANDLE_VALUE) return;
    SetFilePointer(h,0,0,FILE_END);
    DWORD wr=0; DWORD len=(DWORD)StrLenA(s); if (len) WriteFile(h,s,len,&wr,0);
    const char eol[2]={'\r','\n'}; WriteFile(h,eol,2,&wr,0); CloseHandle(h);
}
static void BuildPaths() {
    WCHAR p[MAX_PATH_W]; p[0]=0;
    GetModuleFileNameW(g_self,p,MAX_PATH_W);
    int n=StrLenW(p); int slash=-1;
    for (int i=0;i<n;i++) if (p[i]==L'\\' || p[i]==L'/') slash=i;
    if (slash<0) slash=0;
    if (slash>0) p[slash+1]=0; else p[0]=0;
    CopyW(g_iniPath,MAX_PATH_W,p); AppendW(g_iniPath,MAX_PATH_W,L"PostalBorkenMenu.ini");
    CopyW(g_logPath,MAX_PATH_W,p); AppendW(g_logPath,MAX_PATH_W,L"PostalBorkenMenu.log");
}

// ---------------- IL2CPP bridge ----------------

typedef void Il2CppDomain;
typedef void Il2CppAssembly;
typedef void Il2CppImage;
typedef void Il2CppClass;
typedef void MethodInfo;
typedef void Il2CppObject;
typedef void Il2CppException;
typedef void Il2CppArray;
typedef void Il2CppString;
typedef void Il2CppType;
typedef void FieldInfo;

struct Il2CppApi {
    Il2CppDomain* (*domain_get)();
    const Il2CppAssembly* (*domain_assembly_open)(Il2CppDomain*, const char*);
    const Il2CppImage* (*assembly_get_image)(const Il2CppAssembly*);
    SIZE_T (*image_get_class_count)(const Il2CppImage*);
    Il2CppClass* (*image_get_class)(const Il2CppImage*, SIZE_T);
    const char* (*class_get_name)(Il2CppClass*);
    const char* (*class_get_namespace)(Il2CppClass*);
    const MethodInfo* (*class_get_method_from_name)(Il2CppClass*, const char*, int);
    const MethodInfo* (*class_get_methods)(Il2CppClass*, void**);
    const FieldInfo* (*class_get_fields)(Il2CppClass*, void**);
    const char* (*field_get_name)(const FieldInfo*);
    const char* (*method_get_name)(const MethodInfo*);
    const Il2CppType* (*method_get_param)(const MethodInfo*, unsigned int);
    const Il2CppType* (*method_get_return_type)(const MethodInfo*);
    unsigned int (*method_get_param_count)(const MethodInfo*);
    BOOL (*method_is_instance)(const MethodInfo*);
    Il2CppObject* (*object_new)(Il2CppClass*);
    Il2CppObject* (*runtime_invoke)(const MethodInfo*, void*, void**, Il2CppException**);
    Il2CppString* (*string_new)(const char*);
    const Il2CppImage* (*get_corlib)();
    Il2CppClass* (*class_from_name)(const Il2CppImage*, const char*, const char*);
    Il2CppArray* (*array_new)(Il2CppClass*, SIZE_T);
    SIZE_T (*array_length)(Il2CppArray*);
    unsigned int (*array_object_header_size)();
    void (*gc_wbarrier_set_field)(Il2CppObject*, void**, void*);
    unsigned int (*gchandle_new)(Il2CppObject*, BOOL);
    Il2CppObject* (*gchandle_get_target)(unsigned int);
    void (*gchandle_free)(unsigned int);
    void* (*thread_attach)(Il2CppDomain*);
    void* (*thread_current)();
    void (*thread_detach)(void*);
    const FieldInfo* (*class_get_field_from_name)(Il2CppClass*, const char*);
    void (*field_get_value)(Il2CppObject*, const FieldInfo*, void*);
    const Il2CppType* (*class_get_type)(Il2CppClass*);
    Il2CppClass* (*class_from_type)(const Il2CppType*);
    Il2CppObject* (*type_get_object)(const Il2CppType*);
    void* (*object_unbox)(Il2CppObject*);
    void (*format_exception)(Il2CppException*, char*, int);
};
static Il2CppApi g_il2cpp = {};
static HMODULE g_gameAssembly = 0;
static const Il2CppImage* g_assemblyCSharp = 0;
static Il2CppClass* g_stringClass = 0;
static Il2CppDomain* g_domain = 0;
static const Il2CppImage* g_coreImage = 0;
static const Il2CppImage* g_uiImage = 0;
static Il2CppClass* g_systemTypeClass = 0;
static Il2CppClass* g_boolClass = 0;
static Il2CppClass* g_floatClass = 0;

struct CommandRec {
    const char* className;
    const WCHAR* displayName;
    const WCHAR* iniName;
    int defaultVk;
    int vk;
    Il2CppClass* klass;
    const MethodInfo* execute;
    unsigned int objectHandle;
    HWND argEdit;
    HWND bindButton;
    WCHAR args[128];
};

static CommandRec g_cmds[] = {
    {"Help", L"Help", L"Help", 0,0,0,0,0,0,0,{0}},
    {"NextLevel", L"Next Level", L"NextLevel", 0,0,0,0,0,0,0,{0}},
    {"NoNpc", L"No NPC", L"NoNpc", 0,0,0,0,0,0,0,{0}},
    {"ForceAggro", L"Force Aggro", L"ForceAggro", 0,0,0,0,0,0,0,{0}},
    {"GodMode", L"God Mode", L"GodMode", VK_F2,0,0,0,0,0,0,{0}},
    {"Noclip", L"Noclip", L"Noclip", VK_F3,0,0,0,0,0,0,{0}},
    {"NoTarget", L"No Target", L"NoTarget", VK_F4,0,0,0,0,0,0,{0}},
    {"NoHud", L"No HUD", L"NoHud", VK_F5,0,0,0,0,0,0,{0}},
    {"__NoCrosshair", L"No Crosshair", L"NoCrosshair", 0,0,0,0,0,0,0,{0}},
    {"NoHudWithCrossHair", L"No HUD (keep crosshair)", L"NoHudWithCrossHair", 0,0,0,0,0,0,0,{0}},
    {"NoModel", L"No Player Model", L"NoModel", 0,0,0,0,0,0,0,{0}},
    {"TimeScale", L"Time Scale", L"TimeScale", 0,0,0,0,0,0,0,{L'0',L'.',L'5',0}},
    {"LogTime", L"Log Time", L"LogTime", 0,0,0,0,0,0,0,{0}},
    {"GiveAll", L"Give All", L"GiveAll", VK_F6,0,0,0,0,0,0,{0}},
    {"__GiveAllWeapons", L"Give All Weapons", L"GiveAllWeapons", 0,0,0,0,0,0,0,{0}},
    {"__WeaponListBase1", L"1 Base Weapon", L"WeaponListBase1", 0,0,0,0,0,0,0,{L'1',0}},
    {"__WeaponListDlc1", L"1 DLC Weapon", L"WeaponListDlc1", 0,0,0,0,0,0,0,{L'1',0}},
    {"__WeaponListBase2", L"2 Base Weapon", L"WeaponListBase2", 0,0,0,0,0,0,0,{L'2',0}},
    {"__WeaponListDlc2", L"2 DLC Weapon", L"WeaponListDlc2", 0,0,0,0,0,0,0,{L'2',0}},
    {"__WeaponListBase3", L"3 Base Weapon", L"WeaponListBase3", 0,0,0,0,0,0,0,{L'3',0}},
    {"__WeaponListDlc3", L"3 DLC Weapon", L"WeaponListDlc3", 0,0,0,0,0,0,0,{L'3',0}},
    {"__WeaponListBase4", L"4 Base Weapon", L"WeaponListBase4", 0,0,0,0,0,0,0,{L'4',0}},
    {"__WeaponListDlc4", L"4 DLC Weapon", L"WeaponListDlc4", 0,0,0,0,0,0,0,{L'4',0}},
    {"__WeaponListBase5", L"5 Base Weapon", L"WeaponListBase5", 0,0,0,0,0,0,0,{L'5',0}},
    {"__WeaponListDlc5", L"5 DLC Weapon", L"WeaponListDlc5", 0,0,0,0,0,0,0,{L'5',0}},
    {"__WeaponListBase6", L"6 Base Weapon", L"WeaponListBase6", 0,0,0,0,0,0,0,{L'6',0}},
    {"__WeaponListDlc6", L"6 DLC Weapon", L"WeaponListDlc6", 0,0,0,0,0,0,0,{L'6',0}},
    {"__WeaponListBase7", L"7 Base Weapon", L"WeaponListBase7", 0,0,0,0,0,0,0,{L'7',0}},
    {"__WeaponListDlc7", L"7 DLC Weapon", L"WeaponListDlc7", 0,0,0,0,0,0,0,{L'7',0}},
    {"__WeaponListBase8", L"8 Base Weapon", L"WeaponListBase8", 0,0,0,0,0,0,0,{L'8',0}},
    {"__WeaponListDlc8", L"8 DLC Weapon", L"WeaponListDlc8", 0,0,0,0,0,0,0,{L'8',0}},
    {"__WeaponListBase9", L"9 Base Weapon", L"WeaponListBase9", 0,0,0,0,0,0,0,{L'9',0}},
    {"__WeaponListDlc9", L"9 DLC Weapon", L"WeaponListDlc9", 0,0,0,0,0,0,0,{L'9',0}},
    {"__GiveAllDlcWeapons", L"Give All DLC Weapons", L"GiveAllDlcWeapons", 0,0,0,0,0,0,0,{0}},
    {"__InitDlcHook", L"DLC Hook Deep Probe", L"InitDlcHook", 0,0,0,0,0,0,0,{0}},
    {"__WeaponKeysMode", L"Weapon Keys Mode", L"WeaponKeysMode", 0,0,0,0,0,0,0,{L'O',L'F',L'F',0}},
    {"__BaseWeaponWheel", L"Base Weapon Wheel", L"BaseWeaponWheel", 0,0,0,0,0,0,0,{0}},
    {"__DlcWeaponWheel", L"DLC Weapon Wheel", L"DlcWeaponWheel", 0,0,0,0,0,0,0,{0}},
    {"UnlockAll", L"Unlock All", L"UnlockAll", VK_F7,0,0,0,0,0,0,{0}},
    {"NoMenu", L"No Menu", L"NoMenu", 0,0,0,0,0,0,0,{0}},
    {"KillPresident", L"Kill President", L"KillPresident", 0,0,0,0,0,0,0,{0}},
};
static const int CMD_COUNT = sizeof(g_cmds)/sizeof(g_cmds[0]);

static void* Proc(const char* name) { return g_gameAssembly ? GetProcAddress(g_gameAssembly,name) : 0; }
static BOOL ResolveIl2CppApi() {
#define RESOLVE(name) g_il2cpp.name=(decltype(g_il2cpp.name))Proc("il2cpp_" #name); if(!g_il2cpp.name){LogA("[ERROR] Missing il2cpp_" #name);return FALSE;}
    RESOLVE(domain_get)
    RESOLVE(domain_assembly_open)
    RESOLVE(assembly_get_image)
    RESOLVE(image_get_class_count)
    RESOLVE(image_get_class)
    RESOLVE(class_get_name)
    RESOLVE(class_get_namespace)
    RESOLVE(class_get_method_from_name)
    RESOLVE(class_get_methods)
    RESOLVE(class_get_fields)
    RESOLVE(field_get_name)
    RESOLVE(method_get_name)
    RESOLVE(method_get_param)
    RESOLVE(method_get_return_type)
    RESOLVE(method_get_param_count)
    RESOLVE(method_is_instance)
    RESOLVE(object_new)
    RESOLVE(runtime_invoke)
    RESOLVE(string_new)
    RESOLVE(get_corlib)
    RESOLVE(class_from_name)
    RESOLVE(array_new)
    RESOLVE(array_length)
    RESOLVE(array_object_header_size)
    RESOLVE(gc_wbarrier_set_field)
    RESOLVE(gchandle_new)
    RESOLVE(gchandle_get_target)
    RESOLVE(gchandle_free)
    RESOLVE(thread_attach)
    RESOLVE(thread_current)
    RESOLVE(thread_detach)
    RESOLVE(class_get_field_from_name)
    RESOLVE(field_get_value)
    RESOLVE(class_get_type)
    RESOLVE(class_from_type)
    RESOLVE(type_get_object)
    RESOLVE(object_unbox)
    RESOLVE(format_exception)
#undef RESOLVE
    return TRUE;
}
