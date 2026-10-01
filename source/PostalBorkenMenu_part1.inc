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
__declspec(dllimport) BOOL __stdcall VirtualProtect(void*, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(const void*, void*, SIZE_T);

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
#define PAGE_READWRITE 0x00000004UL
#define PAGE_NOACCESS 0x00000001UL
#define PAGE_READONLY 0x00000002UL
#define PAGE_WRITECOPY 0x00000008UL
#define PAGE_EXECUTE 0x00000010UL
#define PAGE_EXECUTE_READ 0x00000020UL
#define PAGE_EXECUTE_READWRITE 0x00000040UL
#define PAGE_EXECUTE_WRITECOPY 0x00000080UL
#define PAGE_GUARD 0x00000100UL
#define MEM_COMMIT 0x00001000UL
#define IMAGE_DOS_SIGNATURE 0x5A4D
#define IMAGE_NT_SIGNATURE 0x00004550UL
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20B
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1

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
#define WM_APP_BASEWHEEL (WM_APP + 0x36)
#define WM_APP_DLCWHEEL (WM_APP + 0x37)

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
static volatile LONG g_skipIntroVideos = 1;
static volatile LONG g_skipStartupLogos = 1;
static volatile LONG g_startupHookInstalled = 0;
static volatile LONG g_startupRedirectDone = 0;
static volatile LONG g_sceneMapPatched = 0;
static volatile LONG g_introSkipPulseSent = 0;
static DWORD g_lastCrosshairTick = 0;
static volatile LONG g_crosshairReapplyCountdown = 0;
static int g_scrollRow = 0;
static BYTE g_altSlotFlip[10] = {};
static int g_menuTab = 0; // 0 Gameplay, 1 Weapons, 2 Arsenal, 3 Special, 4 Visual
static volatile LONG g_baseWheelActive = 0;
static int g_baseWheelVk = 0;
static volatile LONG g_dlcWheelActive = 0;
static int g_dlcWheelVk = 0;
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

/*
 H30 ASI-only startup-logo bypass.

 H29 proved that a raw level0 -> level1 file redirect is not sufficient:
 the game can reach a black screen because Unity still owns the original
 build-scene mapping. H30 therefore fails closed.

 Before redirecting level0, H30 searches committed readable process memory
 for the exact serialized adjacent scene entries:
   Assets/Scenes/PlatformIntro.unity
   Assets/Scenes/Intro.unity

 The exact 72-byte serialized block is swapped in memory only. If that exact
 block is not found and patched, H30 does NOT redirect level0.

 No game file is written, renamed or replaced on disk.
 No SceneManager.LoadScene or IL2CPP scene transition is used.
*/
struct PBM_IMAGE_DOS_HEADER {
    WORD e_magic;
    BYTE pad[58];
    LONG e_lfanew;
};
struct PBM_IMAGE_FILE_HEADER {
    WORD Machine;
    WORD NumberOfSections;
    DWORD TimeDateStamp;
    DWORD PointerToSymbolTable;
    DWORD NumberOfSymbols;
    WORD SizeOfOptionalHeader;
    WORD Characteristics;
};
struct PBM_IMAGE_DATA_DIRECTORY { DWORD VirtualAddress; DWORD Size; };
struct PBM_IMAGE_OPTIONAL_HEADER64 {
    WORD Magic;
    BYTE MajorLinkerVersion;
    BYTE MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    ULONGLONG ImageBase;
    DWORD SectionAlignment;
    DWORD FileAlignment;
    WORD MajorOperatingSystemVersion;
    WORD MinorOperatingSystemVersion;
    WORD MajorImageVersion;
    WORD MinorImageVersion;
    WORD MajorSubsystemVersion;
    WORD MinorSubsystemVersion;
    DWORD Win32VersionValue;
    DWORD SizeOfImage;
    DWORD SizeOfHeaders;
    DWORD CheckSum;
    WORD Subsystem;
    WORD DllCharacteristics;
    ULONGLONG SizeOfStackReserve;
    ULONGLONG SizeOfStackCommit;
    ULONGLONG SizeOfHeapReserve;
    ULONGLONG SizeOfHeapCommit;
    DWORD LoaderFlags;
    DWORD NumberOfRvaAndSizes;
    PBM_IMAGE_DATA_DIRECTORY DataDirectory[16];
};
struct PBM_IMAGE_NT_HEADERS64 {
    DWORD Signature;
    PBM_IMAGE_FILE_HEADER FileHeader;
    PBM_IMAGE_OPTIONAL_HEADER64 OptionalHeader;
};
struct PBM_IMAGE_IMPORT_DESCRIPTOR {
    DWORD OriginalFirstThunk;
    DWORD TimeDateStamp;
    DWORD ForwarderChain;
    DWORD Name;
    DWORD FirstThunk;
};
struct PBM_MEMORY_BASIC_INFORMATION {
    void* BaseAddress;
    void* AllocationBase;
    DWORD AllocationProtect;
    WORD PartitionId;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};

typedef HANDLE (__stdcall *PBM_CreateFileWFn)(LPCWSTR, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
static PBM_CreateFileWFn g_unityOriginalCreateFileW=0;
static ULONG_PTR* g_unityCreateFileWIatSlot=0;

static const BYTE g_sceneMapOriginal[72] = {
    0x21,0x00,0x00,0x00,
    'A','s','s','e','t','s','/','S','c','e','n','e','s','/','P','l','a','t','f','o','r','m','I','n','t','r','o','.','u','n','i','t','y',0x00,0x00,0x00,
    0x19,0x00,0x00,0x00,
    'A','s','s','e','t','s','/','S','c','e','n','e','s','/','I','n','t','r','o','.','u','n','i','t','y',0x00,0x00,0x00
};
static const BYTE g_sceneMapSwapped[72] = {
    0x19,0x00,0x00,0x00,
    'A','s','s','e','t','s','/','S','c','e','n','e','s','/','I','n','t','r','o','.','u','n','i','t','y',0x00,0x00,0x00,
    0x21,0x00,0x00,0x00,
    'A','s','s','e','t','s','/','S','c','e','n','e','s','/','P','l','a','t','f','o','r','m','I','n','t','r','o','.','u','n','i','t','y',0x00,0x00,0x00
};

static BOOL IsReadableProtect(DWORD p) {
    if (p & PAGE_GUARD) return FALSE;
    DWORD b=p & 0xFF;
    return b==PAGE_READONLY || b==PAGE_READWRITE || b==PAGE_WRITECOPY ||
           b==PAGE_EXECUTE_READ || b==PAGE_EXECUTE_READWRITE || b==PAGE_EXECUTE_WRITECOPY;
}

static BOOL BytesEqual72(const BYTE* p, const BYTE* q) {
    for (int i=0;i<72;i++) if (p[i]!=q[i]) return FALSE;
    return TRUE;
}

static BOOL PatchSceneMapBlockInMemory() {
    if (g_sceneMapPatched) return TRUE;

    ULONG_PTR addr=0x10000ULL;
    const ULONG_PTR maxAddr=0x00007FFFFFFEFFFFULL;
    int patchedCount=0;

    while (addr<maxAddr) {
        PBM_MEMORY_BASIC_INFORMATION mbi={};
        SIZE_T got=VirtualQuery((const void*)addr,&mbi,sizeof(mbi));
        if (!got || !mbi.RegionSize) break;

        ULONG_PTR base=(ULONG_PTR)mbi.BaseAddress;
        SIZE_T region=mbi.RegionSize;

        if (mbi.State==MEM_COMMIT && IsReadableProtect(mbi.Protect) && region>=72) {
            BYTE* p=(BYTE*)base;
            SIZE_T limit=region-72;
            for (SIZE_T i=0;i<=limit;i++) {
                if (p[i]!=0x21 || p[i+4]!='A') continue;
                if (!BytesEqual72(p+i,g_sceneMapOriginal)) continue;

                DWORD oldProtect=0;
                if (!VirtualProtect(p+i,72,PAGE_READWRITE,&oldProtect)) continue;
                volatile BYTE* dst=(volatile BYTE*)(p+i);
                for (int j=0;j<72;j++) dst[j]=g_sceneMapSwapped[j];
                DWORD ignored=0;
                VirtualProtect(p+i,72,oldProtect,&ignored);
                ++patchedCount;
            }
        }

        ULONG_PTR next=base+(ULONG_PTR)region;
        if (next<=addr) break;
        addr=next;
    }

    if (patchedCount>0) {
        g_sceneMapPatched=1;
        return TRUE;
    }
    return FALSE;
}

static BOOL IsLevel0Path(const WCHAR* path) {
    if (!path) return FALSE;
    int n=StrLenW(path);
    if (n<6) return FALSE;
    const WCHAR* t=path+n-6;
    if (t[0]!=L'l' || t[1]!=L'e' || t[2]!=L'v' || t[3]!=L'e' || t[4]!=L'l' || t[5]!=L'0') return FALSE;
    if (n==6) return TRUE;
    WCHAR p=path[n-7];
    return (p==L'\\' || p==L'/') ? TRUE : FALSE;
}

static void RestoreStartupCreateFileHook() {
    if (!g_unityCreateFileWIatSlot || !g_unityOriginalCreateFileW) {
        g_startupHookInstalled=0;
        return;
    }
    DWORD oldProtect=0;
    if (VirtualProtect(g_unityCreateFileWIatSlot,sizeof(ULONG_PTR),PAGE_READWRITE,&oldProtect)) {
        *g_unityCreateFileWIatSlot=(ULONG_PTR)g_unityOriginalCreateFileW;
        DWORD ignored=0;
        VirtualProtect(g_unityCreateFileWIatSlot,sizeof(ULONG_PTR),oldProtect,&ignored);
    }
    g_unityCreateFileWIatSlot=0;
    g_startupHookInstalled=0;
}

static HANDLE __stdcall StartupCreateFileWHook(
    LPCWSTR fileName, DWORD desiredAccess, DWORD shareMode, void* securityAttributes,
    DWORD creationDisposition, DWORD flagsAndAttributes, HANDLE templateFile)
{
    PBM_CreateFileWFn realFn=g_unityOriginalCreateFileW;
    if (!realFn) return INVALID_HANDLE_VALUE;

    if (g_skipStartupLogos && !g_startupRedirectDone && IsLevel0Path(fileName)) {
        // Fail closed: redirect only if the exact scene-map block was found
        // and swapped in process memory first.
        if (PatchSceneMapBlockInMemory()) {
            int n=StrLenW(fileName);
            if (n>0 && n<MAX_PATH_W) {
                WCHAR redirected[MAX_PATH_W];
                volatile WCHAR* dst=redirected;
                for (int i=0;i<=n;i++) dst[i]=fileName[i];
                redirected[n-1]=L'1';

                HANDLE h=realFn(redirected,desiredAccess,shareMode,securityAttributes,
                                creationDisposition,flagsAndAttributes,templateFile);
                if (h!=INVALID_HANDLE_VALUE) {
                    g_startupRedirectDone=1;
                    RestoreStartupCreateFileHook();
                    return h;
                }
            }
        }
        // Exact map not found or redirect failed: preserve vanilla startup.
        RestoreStartupCreateFileHook();
    }

    return realFn(fileName,desiredAccess,shareMode,securityAttributes,
                  creationDisposition,flagsAndAttributes,templateFile);
}

static BOOL InstallStartupCreateFileHook() {
    if (!g_skipStartupLogos) return TRUE;
    HMODULE unity=GetModuleHandleW(L"UnityPlayer.dll");
    HMODULE k32=GetModuleHandleW(L"kernel32.dll");
    if (!unity || !k32) return FALSE;

    void* targetAddr=GetProcAddress(k32,"CreateFileW");
    if (!targetAddr) return FALSE;

    BYTE* base=(BYTE*)unity;
    PBM_IMAGE_DOS_HEADER* dos=(PBM_IMAGE_DOS_HEADER*)base;
    if (dos->e_magic!=IMAGE_DOS_SIGNATURE || dos->e_lfanew<=0) return FALSE;

    PBM_IMAGE_NT_HEADERS64* nt=(PBM_IMAGE_NT_HEADERS64*)(base+dos->e_lfanew);
    if (nt->Signature!=IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR64_MAGIC) return FALSE;
    DWORD importRva=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!importRva) return FALSE;

    PBM_IMAGE_IMPORT_DESCRIPTOR* desc=(PBM_IMAGE_IMPORT_DESCRIPTOR*)(base+importRva);
    ULONG_PTR target=(ULONG_PTR)targetAddr;

    for (; desc->FirstThunk; ++desc) {
        ULONG_PTR* slot=(ULONG_PTR*)(base+desc->FirstThunk);
        for (; *slot; ++slot) {
            if (*slot!=target) continue;
            DWORD oldProtect=0;
            if (!VirtualProtect(slot,sizeof(ULONG_PTR),PAGE_READWRITE,&oldProtect)) return FALSE;
            g_unityOriginalCreateFileW=(PBM_CreateFileWFn)(*slot);
            g_unityCreateFileWIatSlot=slot;
            *slot=(ULONG_PTR)&StartupCreateFileWHook;
            DWORD ignored=0;
            VirtualProtect(slot,sizeof(ULONG_PTR),oldProtect,&ignored);
            g_startupHookInstalled=1;
            return TRUE;
        }
    }
    return FALSE;
}
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
    const WCHAR* (*string_chars)(Il2CppString*);
    int (*string_length)(Il2CppString*);
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
    {"__SkipIntroVideos", L"Skip Intro Videos", L"SkipIntroVideos", 0,0,0,0,0,0,0,{0}},
    {"__GiveAllWeapons", L"Give All Weapons", L"GiveAllWeapons", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_Shovel", L"Shovel", L"Arsenal_Shovel", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_Pistol", L"Pistol", L"Arsenal_Pistol", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_Shotgun", L"Shotgun", L"Arsenal_Shotgun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_MachineGun", L"Machine Gun", L"Arsenal_MachineGun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_RocketLauncher", L"Rocket Launcher", L"Arsenal_RocketLauncher", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_LightningGun", L"Lightning Gun", L"Arsenal_LightningGun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_GatlingGun", L"Gatling Gun", L"Arsenal_GatlingGun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_DildoBow", L"Dildo Bow", L"Arsenal_DildoBow", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_CatCanon", L"Cat Canon", L"Arsenal_CatCanon", 0,0,0,0,0,0,0,{0}},

    {"__Arsenal_WEAPON_UmDrill", L"Um Drill [DLC]", L"Arsenal_UmDrill", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_PissGun", L"Piss Gun [DLC]", L"Arsenal_PissGun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_MeatShotgun", L"Meat Shotgun [DLC]", L"Arsenal_MeatShotgun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_BubbleGumMachineGun", L"Bubble Gum MG [DLC]", L"Arsenal_BubbleGumMachineGun", 0,0,0,0,0,0,0,{0}},
    {"__Arsenal_WEAPON_NuclearSyringe", L"Nuclear Syringe [DLC]", L"Arsenal_NuclearSyringe", 0,0,0,0,0,0,0,{0}},

    {"__Special_WEAPON_Hook", L"Grappling Hook", L"Special_Hook", 0,0,0,0,0,0,0,{0}},
    {"__Special_WEAPON_CutsceneWeapon_DLC", L"Cutscene Weapon DLC", L"Special_CutsceneWeapon_DLC", 0,0,0,0,0,0,0,{0}},
    {"__Special_WEAPON_NoWeapon", L"No Weapon", L"Special_NoWeapon", 0,0,0,0,0,0,0,{0}},
    {"__Special_WEAPON_NoWeapon_DLC", L"No Weapon DLC", L"Special_NoWeapon_DLC", 0,0,0,0,0,0,0,{0}},
    {"__GiveAllDlcWeapons", L"Give All DLC Weapons", L"GiveAllDlcWeapons", 0,0,0,0,0,0,0,{0}},
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

    // V2A33 catalog-only helpers are optional so startup compatibility is
    // unchanged if a future IL2CPP build does not export these convenience APIs.
    g_il2cpp.string_chars=(decltype(g_il2cpp.string_chars))Proc("il2cpp_string_chars");
    g_il2cpp.string_length=(decltype(g_il2cpp.string_length))Proc("il2cpp_string_length");
    return TRUE;
}
