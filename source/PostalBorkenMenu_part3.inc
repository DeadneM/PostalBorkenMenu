
// ---------------- configuration and overlay UI ----------------

static void VkName(int vk, WCHAR* out, int cap) {
    if (!out || cap<2) return;
    out[0]=0;
    if (vk==0) { CopyW(out,cap,L"Unbound"); return; }
    if (vk>=VK_F1 && vk<=VK_F24) { out[0]=L'F'; WCHAR n[8]; UIntToW((unsigned int)(vk-VK_F1+1),n,8); out[1]=0; AppendW(out,cap,n); return; }
    if (vk>='A' && vk<='Z') { out[0]=(WCHAR)vk; out[1]=0; return; }
    if (vk>='0' && vk<='9') { out[0]=(WCHAR)vk; out[1]=0; return; }
    switch(vk) {
        case VK_SPACE: CopyW(out,cap,L"Space"); return;
        case VK_TAB: CopyW(out,cap,L"Tab"); return;
        case VK_RETURN: CopyW(out,cap,L"Enter"); return;
        case VK_HOME: CopyW(out,cap,L"Home"); return;
        case VK_END: CopyW(out,cap,L"End"); return;
        case VK_PRIOR: CopyW(out,cap,L"PageUp"); return;
        case VK_NEXT: CopyW(out,cap,L"PageDown"); return;
        case VK_INSERT: CopyW(out,cap,L"Insert"); return;
        case VK_DELETE: CopyW(out,cap,L"Delete"); return;
        case VK_LEFT: CopyW(out,cap,L"Left"); return;
        case VK_RIGHT: CopyW(out,cap,L"Right"); return;
        case VK_UP: CopyW(out,cap,L"Up"); return;
        case VK_DOWN: CopyW(out,cap,L"Down"); return;
    }
    WCHAR h[8]; HexByteW((unsigned int)vk,h); CopyW(out,cap,L"VK "); AppendW(out,cap,h);
}

static void FlushConfig() {
    WritePrivateProfileStringW(0,0,0,g_iniPath);
}
static void SaveBinding(int i) {
    if (i<0 || i>=CMD_COUNT) return;
    WCHAR n[16]; UIntToW((unsigned int)g_cmds[i].vk,n,16);
    WritePrivateProfileStringW(L"Keys",g_cmds[i].iniName,n,g_iniPath);
    FlushConfig();
}
static void SaveArgs(int i) {
    if (i<0 || i>=CMD_COUNT) return;
    WritePrivateProfileStringW(L"Args",g_cmds[i].iniName,g_cmds[i].args,g_iniPath);
    FlushConfig();
}
static void LoadConfig() {
    for (int i=0;i<CMD_COUNT;i++) {
        g_cmds[i].vk=(int)GetPrivateProfileIntW(L"Keys",g_cmds[i].iniName,g_cmds[i].defaultVk,g_iniPath);
        WCHAR def[128]; CopyW(def,128,g_cmds[i].args);
        GetPrivateProfileStringW(L"Args",g_cmds[i].iniName,def,g_cmds[i].args,128,g_iniPath);
    }
    g_noCrosshair=(LONG)GetPrivateProfileIntW(L"Settings",L"NoCrosshair",0,g_iniPath);
    g_skipIntroVideos=(LONG)GetPrivateProfileIntW(L"Settings",L"SkipIntroVideos",1,g_iniPath);
}

// GDI is resolved dynamically so the ASI remains linked only to kernel32/user32.
typedef HBRUSH (__stdcall *CreateSolidBrushFn)(COLORREF);
typedef BOOL (__stdcall *DeleteObjectFn)(HGDIOBJ);
typedef COLORREF (__stdcall *SetTextColorFn)(HDC,COLORREF);
typedef int (__stdcall *SetBkModeFn)(HDC,int);
static CreateSolidBrushFn pCreateSolidBrush=0;
static DeleteObjectFn pDeleteObject=0;
static SetTextColorFn pSetTextColor=0;
static SetBkModeFn pSetBkMode=0;
static COLORREF C(unsigned int r,unsigned int g,unsigned int b){ return (COLORREF)(r | (g<<8) | (b<<16)); }

static BOOL InitGdi() {
    HMODULE gdi=LoadLibraryW(L"gdi32.dll");
    if (!gdi) return FALSE;
    pCreateSolidBrush=(CreateSolidBrushFn)GetProcAddress(gdi,"CreateSolidBrush");
    pDeleteObject=(DeleteObjectFn)GetProcAddress(gdi,"DeleteObject");
    pSetTextColor=(SetTextColorFn)GetProcAddress(gdi,"SetTextColor");
    pSetBkMode=(SetBkModeFn)GetProcAddress(gdi,"SetBkMode");
    return pCreateSolidBrush && pDeleteObject && pSetTextColor && pSetBkMode;
}

static void Text(HDC dc, const WCHAR* s, int x1,int y1,int x2,int y2,COLORREF color,UINT flags) {
    if (!dc || !s) return;
    pSetTextColor(dc,color); pSetBkMode(dc,TRANSPARENT_BK);
    RECT r={x1,y1,x2,y2}; DrawTextW(dc,s,-1,&r,flags|DT_VCENTER|DT_SINGLELINE);
}
static void Fill(HDC dc,int x1,int y1,int x2,int y2,COLORREF color) {
    RECT r={x1,y1,x2,y2}; HBRUSH b=pCreateSolidBrush(color); if(b){ FillRect(dc,&r,b); pDeleteObject((HGDIOBJ)b); }
}

static int CommandMenuTab(int i) {
    if(i<0 || i>=CMD_COUNT) return 0;
    const char* n=g_cmds[i].className;

    const char* exactName=0;
    BOOL special=FALSE;
    if (DecodeNamedWeaponCommand(i,&exactName,&special))
        return special ? 3 : 2; // Arsenal / Special

    if (StrEqA(n,"GiveAll") ||
        StrEqA(n,"__GiveAllWeapons") ||
        StrEqA(n,"__GiveAllDlcWeapons") ||
        StrEqA(n,"__WeaponKeysMode") ||
        StrEqA(n,"__BaseWeaponWheel") ||
        StrEqA(n,"__DlcWeaponWheel")) return 1; // Weapons

    if (StrEqA(n,"NoHud") ||
        StrEqA(n,"__NoCrosshair") ||
        StrEqA(n,"NoHudWithCrossHair") ||
        StrEqA(n,"NoModel") ||
        StrEqA(n,"NoMenu")) return 4; // Visual

    return 0; // Gameplay, including Skip Intro Videos
}

static int TabCommandCount() {
    int count=0;
    for(int i=0;i<CMD_COUNT;i++) if(CommandMenuTab(i)==g_menuTab) ++count;
    return count;
}

static int TabCommandIndex(int rank) {
    if(rank<0) return -1;
    int seen=0;
    for(int i=0;i<CMD_COUNT;i++) {
        if(CommandMenuTab(i)!=g_menuTab) continue;
        if(seen==rank) return i;
        ++seen;
    }
    return -1;
}

static int MaxScrollRow() {
    int maxRow=TabCommandCount()-VISIBLE_ROWS;
    return maxRow>0 ? maxRow : 0;
}

static void ClampScrollRow() {
    int maxRow=MaxScrollRow();
    if(g_scrollRow<0) g_scrollRow=0;
    if(g_scrollRow>maxRow) g_scrollRow=maxRow;
}

static void ScrollOverlayRows(int deltaRows) {
    g_scrollRow += deltaRows;
    ClampScrollRow();
    if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
}

static void SelectMenuTab(int tab) {
    if(tab<0 || tab>4) return;
    if(g_menuTab==tab) return;
    g_menuTab=tab;
    g_scrollRow=0;
    g_bindIndex=-1;
    if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
}

static void PaintOverlay(HWND hwnd) {
    PAINTSTRUCT ps={}; HDC dc=BeginPaint(hwnd,&ps); if(!dc) return;
    Fill(dc,0,0,OVERLAY_W,OVERLAY_H,C(16,17,20));
    Fill(dc,0,0,OVERLAY_W,54,C(31,33,39));
    Fill(dc,0,53,OVERLAY_W,55,C(214,82,61));
    Text(dc,L"POSTAL: BRAIN-DAMAGED",22,6,440,30,C(245,245,245),DT_LEFT);
    Text(dc,L"PostalBorkenMenu  •  ASI Overlay",22,27,500,50,C(165,168,176),DT_LEFT);
    Text(dc,g_ready?L"IL2CPP READY":L"IL2CPP INITIALIZING...",560,10,738,32,g_ready?C(115,220,145):C(230,190,90),DT_CENTER);

    Fill(dc,12,59,152,84,g_menuTab==0?C(70,74,86):C(38,40,47));
    Fill(dc,158,59,298,84,g_menuTab==1?C(70,74,86):C(38,40,47));
    Fill(dc,304,59,444,84,g_menuTab==2?C(70,74,86):C(38,40,47));
    Fill(dc,450,59,590,84,g_menuTab==3?C(70,74,86):C(38,40,47));
    Fill(dc,596,59,736,84,g_menuTab==4?C(70,74,86):C(38,40,47));
    Text(dc,L"GAMEPLAY",12,59,152,84,C(238,238,240),DT_CENTER);
    Text(dc,L"WEAPONS",158,59,298,84,C(238,238,240),DT_CENTER);
    Text(dc,L"ARSENAL",304,59,444,84,C(238,238,240),DT_CENTER);
    Text(dc,L"SPECIAL",450,59,590,84,C(238,238,240),DT_CENTER);
    Text(dc,L"VISUAL",596,59,736,84,C(238,238,240),DT_CENTER);

    Text(dc,L"Command",24,87,310,109,C(145,148,156),DT_LEFT);
    Text(dc,L"Argument",340,87,460,109,C(145,148,156),DT_LEFT);
    Text(dc,L"Action",493,87,585,109,C(145,148,156),DT_CENTER);
    Text(dc,L"Key",610,87,735,109,C(145,148,156),DT_CENTER);

    ClampScrollRow();
    int tabCount=TabCommandCount();
    for(int vr=0; vr<VISIBLE_ROWS; ++vr) {
        int i=TabCommandIndex(g_scrollRow+vr);
        if(i<0) break;
        int y=ROW_Y+vr*ROW_H;
        Fill(dc,14,y,746,y+ROW_H-2,(vr&1)?C(23,24,29):C(27,28,34));
        Text(dc,g_cmds[i].displayName,25,y,326,y+ROW_H-2,C(232,233,236),DT_LEFT);

        BOOL isSkipIntro=StrEqA(g_cmds[i].className,"__SkipIntroVideos");
        if (isSkipIntro) {
            Fill(dc,493,y+4,585,y+ROW_H-6,C(43,46,54));
            Fill(dc,501,y+7,517,y+ROW_H-9,C(28,30,35));
            if(g_skipIntroVideos) Text(dc,L"X",501,y+5,517,y+ROW_H-7,C(120,230,150),DT_CENTER);
            Text(dc,g_skipIntroVideos?L"ENABLED":L"DISABLED",522,y+4,585,y+ROW_H-6,
                 g_skipIntroVideos?C(120,230,150):C(200,200,205),DT_CENTER);
            Fill(dc,600,y+4,738,y+ROW_H-6,C(38,41,49));
            Text(dc,L"NEXT LAUNCH",600,y+4,738,y+ROW_H-6,C(198,202,210),DT_CENTER);
            continue;
        }

        if (g_bindIndex==i) Fill(dc,598,y+3,738,y+ROW_H-5,C(85,54,34));
        int listCategory=0, listSlot=0;
        BOOL isWeaponList=DecodeWeaponListCommand(i,&listCategory,&listSlot);
        BOOL clickableArg = StrEqA(g_cmds[i].className,"TimeScale") ||
                            StrEqA(g_cmds[i].className,"__WeaponKeysMode") ||
                            isWeaponList;
        if (clickableArg) Fill(dc,334,y+4,474,y+ROW_H-6,C(38,41,49));
        if (g_cmds[i].args[0]) Text(dc,g_cmds[i].args,342,y,462,y+ROW_H-2,C(198,202,210),DT_LEFT);
        Fill(dc,493,y+4,585,y+ROW_H-6,C(54,58,68));
        Text(dc,L"RUN",493,y+4,585,y+ROW_H-6,C(246,246,246),DT_CENTER);
        WCHAR key[48]; if(g_bindIndex==i) CopyW(key,48,L"PRESS KEY"); else VkName(g_cmds[i].vk,key,48);
        Fill(dc,600,y+4,738,y+ROW_H-6,(g_bindIndex==i)?C(112,69,38):C(43,46,54));
        Text(dc,key,600,y+4,738,y+ROW_H-6,C(238,238,240),DT_CENTER);
    }

    int trackTop=ROW_Y, trackBottom=ROW_Y+VISIBLE_ROWS*ROW_H-2;
    Fill(dc,750,trackTop,754,trackBottom,C(37,40,47));
    if(tabCount>VISIBLE_ROWS) {
        int trackH=trackBottom-trackTop;
        int thumbH=(trackH*VISIBLE_ROWS)/tabCount;
        if(thumbH<28) thumbH=28;
        int maxRow=MaxScrollRow();
        int thumbY=trackTop;
        if(maxRow>0) thumbY += ((trackH-thumbH)*g_scrollRow)/maxRow;
        Fill(dc,750,thumbY,754,thumbY+thumbH,C(116,121,134));
    }

    int fy=ROW_Y+VISIBLE_ROWS*ROW_H+4;
    Text(dc,L"Tabs: Gameplay / Weapons / Arsenal / Special / Visual  •  Mouse wheel scroll  •  F1 close",22,fy,738,fy+28,C(145,148,156),DT_LEFT);
    EndPaint(hwnd,&ps);
}

static void UpdateOverlayPosition() {
    if (!g_gameHwnd || !g_menuHwnd) return;
    RECT r={}; POINT p={0,0};
    if (!GetClientRect(g_gameHwnd,&r) || !ClientToScreen(g_gameHwnd,&p)) return;
    int maxX=(int)(r.right-r.left)-OVERLAY_W-16; if(maxX<0) maxX=0;
    int maxY=(int)(r.bottom-r.top)-OVERLAY_H-16; if(maxY<0) maxY=0;
    if(g_overlayX>maxX) g_overlayX=maxX; if(g_overlayY>maxY) g_overlayY=maxY;
    SetWindowPos(g_menuHwnd,(HWND)(LONG_PTR)-1,p.x+g_overlayX,p.y+g_overlayY,OVERLAY_W,OVERLAY_H,SWP_NOACTIVATE);
}

static LRESULT __stdcall OverlayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg==WM_PAINT) { PaintOverlay(hwnd); return 0; }
    if (msg==WM_APP_SHUTDOWN) { DestroyWindow(hwnd); return 0; }
    if (msg==WM_DESTROY) { g_menuHwnd=0; PostQuitMessage(0); return 0; }
    if (msg==WM_CLOSE) {
        if (g_shuttingDown) { DestroyWindow(hwnd); return 0; }
        ShowWindow(hwnd,SW_HIDE); g_menuVisible=0; g_bindIndex=-1; return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

static BOOL CreateOverlayWindow() {
    if(!InitGdi()){ LogA("[ERROR] Failed to resolve GDI32 overlay functions"); return FALSE; }
    WNDCLASSEXW wc={}; wc.cbSize=sizeof(wc); wc.lpfnWndProc=OverlayWndProc; wc.hInstance=(HINSTANCE)g_self;
    wc.lpszClassName=L"PostalBorkenMenuOverlayClass";
    RegisterClassExW(&wc);
    g_menuHwnd=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_NOACTIVATE,
        wc.lpszClassName,L"PostalBorkenMenu",WS_POPUP,32,32,OVERLAY_W,OVERLAY_H,0,0,wc.hInstance,0);
    if(!g_menuHwnd){ LogA("[ERROR] Failed to create overlay window"); return FALSE; }
    SetLayeredWindowAttributes(g_menuHwnd,0,242,LWA_ALPHA);
    ShowWindow(g_menuHwnd,SW_HIDE); g_menuVisible=0;
    LogA("[OK] Borderless overlay shell created (no title bar/taskbar entry)");
    return TRUE;
}

static void ToggleOverlay() {
    if(!g_menuHwnd) return;
    if(g_menuVisible){ ShowWindow(g_menuHwnd,SW_HIDE); g_menuVisible=0; g_bindIndex=-1; }
    else { ClampScrollRow(); UpdateOverlayPosition(); ShowWindow(g_menuHwnd,SW_SHOWNOACTIVATE); g_menuVisible=1; InvalidateRect(g_menuHwnd,0,TRUE); }
}

static void CycleTimeScaleArg(int i) {
    if(i<0 || i>=CMD_COUNT || !StrEqA(g_cmds[i].className,"TimeScale")) return;
    if(g_cmds[i].args[0]==L'0' && g_cmds[i].args[1]==L'.' && g_cmds[i].args[2]==L'5') CopyW(g_cmds[i].args,128,L"1");
    else if(g_cmds[i].args[0]==L'1' && g_cmds[i].args[1]==0) CopyW(g_cmds[i].args,128,L"2");
    else if(g_cmds[i].args[0]==L'2' && g_cmds[i].args[1]==0) CopyW(g_cmds[i].args,128,L"0.5");
    else CopyW(g_cmds[i].args,128,L"1");
    SaveArgs(i); if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
}

static void CycleWeaponSlotArg(int i) {
    if(i<0 || i>=CMD_COUNT) return;
    int category=0, slot=0;
    if(!DecodeWeaponListCommand(i,&category,&slot)) return;

    ++slot;
    if(slot>9) slot=1;

    WCHAR n[16];
    UIntToW((unsigned int)slot,n,16);
    CopyW(g_cmds[i].args,128,n);
    SaveArgs(i);
    if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
}

static int GetWeaponKeysMode() {
    for(int i=0;i<CMD_COUNT;i++) {
        if(!StrEqA(g_cmds[i].className,"__WeaponKeysMode")) continue;
        WCHAR* a=g_cmds[i].args;
        if(a[0]==L'B' && a[1]==L'A') return 1;   // BASE
        if(a[0]==L'D' && a[1]==L'L') return 2;   // DLC
        if(a[0]==L'A' && a[1]==L'L') return 3;   // ALT
        return 0;                                 // OFF / unknown
    }
    return 0;
}

static void CycleWeaponKeysMode(int i) {
    if(i<0 || i>=CMD_COUNT || !StrEqA(g_cmds[i].className,"__WeaponKeysMode")) return;
    int mode=GetWeaponKeysMode();
    if(mode==0) CopyW(g_cmds[i].args,128,L"BASE");
    else if(mode==1) CopyW(g_cmds[i].args,128,L"DLC");
    else if(mode==2) CopyW(g_cmds[i].args,128,L"ALT");
    else CopyW(g_cmds[i].args,128,L"OFF");
    for(int s=0;s<10;s++) g_altSlotFlip[s]=0;
    SaveArgs(i);
    LogA(mode==0 ? "[KEYMAP] Weapon Keys Mode -> BASE" :
         mode==1 ? "[KEYMAP] Weapon Keys Mode -> DLC" :
         mode==2 ? "[KEYMAP] Weapon Keys Mode -> ALT" :
                   "[KEYMAP] Weapon Keys Mode -> OFF");
    if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
}

static BOOL HandleOverlayClick(int x,int y) {
    int px=x-g_overlayX, py=y-g_overlayY;
    if(px<0 || py<0 || px>=OVERLAY_W || py>=OVERLAY_H) return FALSE;

    if(py>=59 && py<84) {
        if(px>=12 && px<152) SelectMenuTab(0);
        else if(px>=158 && px<298) SelectMenuTab(1);
        else if(px>=304 && px<444) SelectMenuTab(2);
        else if(px>=450 && px<590) SelectMenuTab(3);
        else if(px>=596 && px<736) SelectMenuTab(4);
        return TRUE;
    }

    if(py>=ROW_Y && py<ROW_Y+VISIBLE_ROWS*ROW_H) {
        int visibleIndex=(py-ROW_Y)/ROW_H;
        int i=TabCommandIndex(g_scrollRow+visibleIndex);
        int ry=(py-ROW_Y)%ROW_H;
        if(i>=0 && i<CMD_COUNT && ry<ROW_H-1) {
            if(StrEqA(g_cmds[i].className,"__SkipIntroVideos")) {
                if(px>=493 && px<744) ToggleSkipIntroVideos();
                return TRUE;
            }
            if(px>=493 && px<590){ ExecuteCommandIndex(i); if(g_menuHwnd)InvalidateRect(g_menuHwnd,0,TRUE); return TRUE; }
            if(px>=600 && px<744){ g_bindIndex=i; if(g_menuHwnd)InvalidateRect(g_menuHwnd,0,TRUE); return TRUE; }
            if(px>=330 && px<480 && StrEqA(g_cmds[i].className,"TimeScale")){ CycleTimeScaleArg(i); return TRUE; }
            int listCategory=0, listSlot=0;
            if(px>=330 && px<480 && DecodeWeaponListCommand(i,&listCategory,&listSlot)){ CycleWeaponSlotArg(i); return TRUE; }
            if(px>=330 && px<480 && StrEqA(g_cmds[i].className,"__WeaponKeysMode")){ CycleWeaponKeysMode(i); return TRUE; }
        }
    }
    return TRUE;
}

static WNDPROC BeginEarlyShutdown(HWND hwnd, const char* reason) {
    WNDPROC old=g_oldGameWndProc;
    if (!g_shuttingDown) {
        // Critical V2A8 rule: fence ALL mod/IL2CPP work BEFORE Unity begins teardown.
        g_shuttingDown=1;
        g_ready=0;
        g_crosshairReapplyCountdown=0;
        g_baseWheelActive=0;
        g_baseWheelVk=0;
        g_dlcWheelActive=0;
        g_dlcWheelVk=0;
        g_bindIndex=-1;
        g_menuVisible=0;
        LogA(reason);
    }

    // Restore Unity's original WndProc immediately. No PostalBorkenMenu callback
    // is allowed to survive into the native/IL2CPP destruction phase.
    if (old && hwnd) {
        SetWindowLongPtrW(hwnd,GWLP_WNDPROC,(LONG_PTR)old);
        g_oldGameWndProc=0;
    }
    g_gameHwnd=0;

    // The overlay belongs to ModThread. Ask it to close; ModThread also performs
    // a same-thread DestroyWindow fallback after leaving its loop.
    if (g_menuHwnd) PostMessageW(g_menuHwnd,WM_APP_SHUTDOWN,0,0);
    return old;
}

static LRESULT __stdcall GameWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    // WM_CLOSE is earlier than WM_NCDESTROY and is the important V2A8 fence.
    // The previous exit fix waited until teardown was already too far along.
    if (msg==WM_CLOSE) {
        WNDPROC old=BeginEarlyShutdown(hwnd,"[SHUTDOWN] WM_CLOSE detected early; IL2CPP work fenced and original WndProc restored.");
        return old ? CallWindowProcW(old,hwnd,msg,wp,lp) : DefWindowProcW(hwnd,msg,wp,lp);
    }

    // Fallback for exit paths that skip WM_CLOSE.
    if (msg==WM_DESTROY || msg==WM_NCDESTROY) {
        WNDPROC old=BeginEarlyShutdown(hwnd,"[SHUTDOWN] Window destruction fallback detected; mod work fenced and WndProc restored.");
        return old ? CallWindowProcW(old,hwnd,msg,wp,lp) : DefWindowProcW(hwnd,msg,wp,lp);
    }

    if (g_shuttingDown) {
        return g_oldGameWndProc ? CallWindowProcW(g_oldGameWndProc,hwnd,msg,wp,lp) : DefWindowProcW(hwnd,msg,wp,lp);
    }

    if (msg==WM_MOUSEWHEEL && g_menuVisible) {
        short delta=(short)((wp>>16)&0xFFFF);
        if(delta>0) ScrollOverlayRows(-3);
        else if(delta<0) ScrollOverlayRows(3);
        return 0;
    }

    if (msg==WM_APP_EXEC) { ExecuteCommandIndex((int)wp); return 0; }
    if (msg==WM_APP_WEAPONKEY) {
        int slot=(int)(wp & 0xFF);
        int mode=(int)((wp >> 8) & 0xFF);
        ExecuteWeaponKeyMapping(slot,mode);
        return 0;
    }
    if (msg==WM_APP_BASEWHEEL) {
        ExecuteNativeBaseWheel(lp ? TRUE : FALSE);
        return 0;
    }
    if (msg==WM_APP_DLCWHEEL) {
        ExecuteNativeDlcWheel(lp ? TRUE : FALSE);
        return 0;
    }
    if (msg==WM_APP_CROSSHAIR) {
        if (g_ready && !g_shuttingDown) {
            BOOL attached=FALSE; void* thread=0;
            if (EnsureIl2CppThread(&attached,&thread)) {
                SetCrosshairVisualAlpha(g_noCrosshair ? 0.0f : 1.0f);
                ReleaseIl2CppThread(attached,thread);
            }
        }
        return 0;
    }

    // V2A7 repeatedly entered IL2CPP every ~250 ms while No Crosshair was on.
    // V2A8 is event-driven instead: schedule one delayed visual refresh after
    // keyboard/system-key or mouse-wheel input. This covers normal weapon
    // switching without touching IL2CPP continuously during idle/exit.
    if (msg==WM_KEYDOWN || msg==WM_SYSKEYDOWN || msg==WM_MOUSEWHEEL || msg==WM_MOUSEHWHEEL) {
        if (g_ready) g_crosshairReapplyCountdown=15; // ~150 ms at the 10 ms worker cadence
    }

    if ((msg==WM_MOVE || msg==WM_SIZE) && g_menuVisible) UpdateOverlayPosition();
    if (msg==WM_LBUTTONDOWN && g_menuVisible) {
        int x=(short)(lp & 0xFFFF); int y=(short)((lp>>16)&0xFFFF);
        if(HandleOverlayClick(x,y)) return 0;
    }
    return g_oldGameWndProc ? CallWindowProcW(g_oldGameWndProc,hwnd,msg,wp,lp) : DefWindowProcW(hwnd,msg,wp,lp);
}

static void PollHotkeys() {
    if (g_shuttingDown) return;

    HWND fg=GetForegroundWindow();
    BOOL active=(fg && fg==g_gameHwnd);

    if (!active && g_baseWheelActive && g_gameHwnd) {
        PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,0);
        g_baseWheelVk=0;
    }
    if (!active && g_dlcWheelActive && g_gameHwnd) {
        PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,0);
        g_dlcWheelVk=0;
    }

    for (int vk=8; vk<256; ++vk) {
        if (g_shuttingDown) return;

        BYTE was=g_keyPrev[vk];
        BYTE down=(GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
        BYTE pressed=(down && !was) ? 1 : 0;
        BYTE released=(!down && was) ? 1 : 0;
        g_keyPrev[vk]=down;

        if (!active || (!pressed && !released)) continue;

        if (g_bindIndex>=0) {
            if (!pressed) continue;
            if (vk==VK_ESCAPE) {
                g_bindIndex=-1;
                if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
                return;
            }
            if (vk==VK_F1) {
                g_bindIndex=-1;
                if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
                ToggleOverlay();
                return;
            }
            if (vk==VK_BACK) g_cmds[g_bindIndex].vk=0;
            else g_cmds[g_bindIndex].vk=vk;
            SaveBinding(g_bindIndex);
            g_bindIndex=-1;
            if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
            return;
        }

        // Both weapon wheels are HOLD interactions. The base wheel keeps the
        // validated V2A20 path; the DLC wheel uses a V2A20-style empty-shell path with no GiveAllWeapons fallback.
        if (g_ready && !g_shuttingDown && !g_menuVisible) {
            for(int i=0;i<CMD_COUNT;i++) {
                if(!g_cmds[i].vk || g_cmds[i].vk!=vk) continue;

                if(StrEqA(g_cmds[i].className,"__BaseWeaponWheel")) {
                    if(pressed) {
                        if(g_dlcWheelActive) PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,0);
                        g_baseWheelVk=vk;
                        PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,1);
                        return;
                    }
                    if(released && g_baseWheelVk==vk) {
                        PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,0);
                        g_baseWheelVk=0;
                        return;
                    }
                }

                if(StrEqA(g_cmds[i].className,"__DlcWeaponWheel")) {
                    if(pressed) {
                        if(g_baseWheelActive) PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,0);
                        g_dlcWheelVk=vk;
                        PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,1);
                        return;
                    }
                    if(released && g_dlcWheelVk==vk) {
                        PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,0);
                        g_dlcWheelVk=0;
                        return;
                    }
                }
            }
        }

        if (released && g_ready && !g_shuttingDown && vk>=0x31 && vk<=0x39) {
            int mode=GetWeaponKeysMode();
            if (mode>0) {
                int slot=vk-0x30;
                WPARAM packed=(WPARAM)(slot | (mode<<8));
                PostMessageW(g_gameHwnd,WM_APP_WEAPONKEY,packed,0);
                return;
            }
        }

        if (!pressed) continue;

        if (vk==VK_F1) {
            if(g_baseWheelActive) PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,0);
            if(g_dlcWheelActive) PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,0);
            g_baseWheelVk=0;
            g_dlcWheelVk=0;
            ToggleOverlay();
            return;
        }

        if (g_ready && !g_shuttingDown) {
            for (int i=0;i<CMD_COUNT;i++) {
                if (g_cmds[i].vk && g_cmds[i].vk==vk) {
                    if(StrEqA(g_cmds[i].className,"__BaseWeaponWheel") ||
                       StrEqA(g_cmds[i].className,"__DlcWeaponWheel")) continue;
                    PostMessageW(g_gameHwnd,WM_APP_EXEC,(WPARAM)i,0);
                    return;
                }
            }
        }
    }

    if (g_crosshairReapplyCountdown>0) {
        LONG next=g_crosshairReapplyCountdown-1;
        g_crosshairReapplyCountdown=next;
        if (next==0 && g_ready && !g_shuttingDown && g_gameHwnd && IsWindow(g_gameHwnd))
            PostMessageW(g_gameHwnd,WM_APP_CROSSHAIR,0,0);
    }
}

static BOOL HookGameWindow() {
    DWORD pid=GetCurrentProcessId();
    HWND candidate=0;
    int stableSamples=0;

    LogA("[WINDOW] Waiting for one stable foreground game window before subclassing.");

    for (int k=0;k<600;k++) {
        HWND h=GetForegroundWindow();
        BOOL valid=FALSE;

        if (h && h!=g_menuHwnd && IsWindow(h)) {
            DWORD wp=0;
            GetWindowThreadProcessId(h,&wp);

            RECT r={};
            if (wp==pid && GetClientRect(h,&r)) {
                int w=(int)(r.right-r.left);
                int hh=(int)(r.bottom-r.top);
                if (w>=320 && hh>=200) valid=TRUE;
            }
        }

        if (valid) {
            if (h==candidate) {
                ++stableSamples;
            } else {
                candidate=h;
                stableSamples=1;
                LogA("[WINDOW] New foreground candidate detected; stability timer restarted.");
            }

            if (stableSamples==10) {
                LogA("[WINDOW] Candidate stable for 1 second.");
                if (g_skipIntroVideos)
                    LogA("[H19 EARLY] No simulated input is emitted. Early watcher is observation-only.");
            } else if (stableSamples==20)
                LogA("[WINDOW] Candidate stable for 2 seconds.");

            if (stableSamples>=30) {
                LogA("[WINDOW] Candidate stable for 3 seconds; installing WndProc subclass now.");
                LONG_PTR old=SetWindowLongPtrW(candidate,GWLP_WNDPROC,(LONG_PTR)GameWndProc);
                if (old) {
                    g_gameHwnd=candidate;
                    g_oldGameWndProc=(WNDPROC)old;
                    LogA("[OK] Stable game window subclass installed - F1 overlay ready");
                    return TRUE;
                }

                LogA("[WINDOW] Subclass attempt failed; candidate stability reset.");
                candidate=0;
                stableSamples=0;
            }
        } else {
            if (candidate || stableSamples)
                LogA("[WINDOW] Foreground candidate lost; stability timer reset.");
            candidate=0;
            stableSamples=0;
        }

        Sleep(100);
    }

    LogA("[ERROR] Could not find a stable game window to subclass");
    return FALSE;
}


static void CopyA(char* dst,int cap,const char* src) {
    if(!dst || cap<=0) return;
    int i=0; if(src) for(;i<cap-1 && src[i];++i) dst[i]=src[i];
    dst[i]=0;
}

static void H19ManagedStringToAsciiLocal(Il2CppString* str,
                                         const WCHAR* (__stdcall *stringChars)(Il2CppString*),
                                         int (__stdcall *stringLength)(Il2CppString*),
                                         char* out, int cap) {
    if (!out || cap<=0) return;
    out[0]=0;
    if (!str || !stringChars || !stringLength) return;
    const WCHAR* s=stringChars(str);
    int len=stringLength(str);
    if (!s || len<=0) return;
    int p=0;
    for (int i=0;i<len && p<cap-1;i++) {
        WCHAR w=s[i];
        out[p++]=(w>=32 && w<=126) ? (char)w : '?';
    }
    out[p]=0;
}

static DWORD __stdcall EarlyStartupObjectWatchH19(void*) {
    BuildPaths();
    LogA("[H19 EARLY] ===== isolated early startup-object watch begin =====");
    LogA("[H19 EARLY] Observation only. No field write, no skip call, no synthetic input.");

    HMODULE ga=0;
    int waitMs=0;
    for (;waitMs<5000 && !g_shuttingDown;waitMs+=5) {
        ga=GetModuleHandleW(L"GameAssembly.dll");
        if (ga) break;
        Sleep(5);
    }
    if (!ga || g_shuttingDown) {
        LogA("[H19 EARLY] GameAssembly.dll unavailable in early window.");
        LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
        return 0;
    }
    LogIntA("[H19 EARLY] GameAssembly available after ms: ",waitMs);

    typedef Il2CppDomain* (__stdcall *FnDomainGet)();
    typedef const Il2CppAssembly* (__stdcall *FnDomainAssemblyOpen)(Il2CppDomain*,const char*);
    typedef const Il2CppImage* (__stdcall *FnAssemblyGetImage)(const Il2CppAssembly*);
    typedef SIZE_T (__stdcall *FnImageGetClassCount)(const Il2CppImage*);
    typedef Il2CppClass* (__stdcall *FnImageGetClass)(const Il2CppImage*,SIZE_T);
    typedef const char* (__stdcall *FnClassGetName)(Il2CppClass*);
    typedef const char* (__stdcall *FnClassGetNamespace)(Il2CppClass*);
    typedef const MethodInfo* (__stdcall *FnClassGetMethods)(Il2CppClass*,void**);
    typedef const MethodInfo* (__stdcall *FnClassGetMethodFromName)(Il2CppClass*,const char*,int);
    typedef const FieldInfo* (__stdcall *FnClassGetFieldFromName)(Il2CppClass*,const char*);
    typedef const char* (__stdcall *FnMethodGetName)(const MethodInfo*);
    typedef unsigned int (__stdcall *FnMethodGetParamCount)(const MethodInfo*);
    typedef const Il2CppType* (__stdcall *FnClassGetType)(Il2CppClass*);
    typedef Il2CppObject* (__stdcall *FnTypeGetObject)(const Il2CppType*);
    typedef Il2CppObject* (__stdcall *FnRuntimeInvoke)(const MethodInfo*,void*,void**,Il2CppException**);
    typedef SIZE_T (__stdcall *FnArrayLength)(Il2CppArray*);
    typedef unsigned int (__stdcall *FnArrayObjectHeaderSize)();
    typedef void* (__stdcall *FnThreadAttach)(Il2CppDomain*);
    typedef void* (__stdcall *FnThreadCurrent)();
    typedef void (__stdcall *FnThreadDetach)(void*);
    typedef void (__stdcall *FnFieldGetValue)(Il2CppObject*,const FieldInfo*,void*);
    typedef const WCHAR* (__stdcall *FnStringChars)(Il2CppString*);
    typedef int (__stdcall *FnStringLength)(Il2CppString*);
    typedef Il2CppClass* (__stdcall *FnObjectGetClass)(Il2CppObject*);

#define H19RESOLVE(type,var,name) type var=(type)GetProcAddress(ga,name)
    H19RESOLVE(FnDomainGet,domainGet,"il2cpp_domain_get");
    H19RESOLVE(FnDomainAssemblyOpen,domainAssemblyOpen,"il2cpp_domain_assembly_open");
    H19RESOLVE(FnAssemblyGetImage,assemblyGetImage,"il2cpp_assembly_get_image");
    H19RESOLVE(FnImageGetClassCount,imageGetClassCount,"il2cpp_image_get_class_count");
    H19RESOLVE(FnImageGetClass,imageGetClass,"il2cpp_image_get_class");
    H19RESOLVE(FnClassGetName,classGetName,"il2cpp_class_get_name");
    H19RESOLVE(FnClassGetNamespace,classGetNamespace,"il2cpp_class_get_namespace");
    H19RESOLVE(FnClassGetMethods,classGetMethods,"il2cpp_class_get_methods");
    H19RESOLVE(FnClassGetMethodFromName,classGetMethodFromName,"il2cpp_class_get_method_from_name");
    H19RESOLVE(FnClassGetFieldFromName,classGetFieldFromName,"il2cpp_class_get_field_from_name");
    H19RESOLVE(FnMethodGetName,methodGetName,"il2cpp_method_get_name");
    H19RESOLVE(FnMethodGetParamCount,methodGetParamCount,"il2cpp_method_get_param_count");
    H19RESOLVE(FnClassGetType,classGetType,"il2cpp_class_get_type");
    H19RESOLVE(FnTypeGetObject,typeGetObject,"il2cpp_type_get_object");
    H19RESOLVE(FnRuntimeInvoke,runtimeInvoke,"il2cpp_runtime_invoke");
    H19RESOLVE(FnArrayLength,arrayLength,"il2cpp_array_length");
    H19RESOLVE(FnArrayObjectHeaderSize,arrayObjectHeaderSize,"il2cpp_array_object_header_size");
    H19RESOLVE(FnThreadAttach,threadAttach,"il2cpp_thread_attach");
    H19RESOLVE(FnThreadCurrent,threadCurrent,"il2cpp_thread_current");
    H19RESOLVE(FnThreadDetach,threadDetach,"il2cpp_thread_detach");
    H19RESOLVE(FnFieldGetValue,fieldGetValue,"il2cpp_field_get_value");
    H19RESOLVE(FnStringChars,stringChars,"il2cpp_string_chars");
    H19RESOLVE(FnStringLength,stringLength,"il2cpp_string_length");
    H19RESOLVE(FnObjectGetClass,objectGetClass,"il2cpp_object_get_class");
#undef H19RESOLVE

    if (!domainGet || !domainAssemblyOpen || !assemblyGetImage || !imageGetClassCount ||
        !imageGetClass || !classGetName || !classGetMethods || !classGetMethodFromName ||
        !classGetFieldFromName || !methodGetName || !methodGetParamCount ||
        !classGetType || !typeGetObject || !runtimeInvoke || !arrayLength ||
        !arrayObjectHeaderSize || !threadAttach || !threadCurrent || !threadDetach ||
        !fieldGetValue) {
        LogA("[H19 EARLY] Required isolated IL2CPP exports unavailable.");
        LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
        return 0;
    }

    Il2CppDomain* domain=0;
    int domainWait=0;
    for (;domainWait<5000 && !g_shuttingDown;domainWait+=5) {
        domain=domainGet();
        if (domain) break;
        Sleep(5);
    }
    if (!domain || g_shuttingDown) {
        LogA("[H19 EARLY] IL2CPP domain unavailable.");
        LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
        return 0;
    }
    LogIntA("[H19 EARLY] IL2CPP domain available after additional ms: ",domainWait);

    BOOL attached=FALSE;
    void* attachedThread=0;
    if (!threadCurrent()) {
        attachedThread=threadAttach(domain);
        attached=attachedThread ? TRUE : FALSE;
    }

    const Il2CppAssembly* gameAss=0;
    const Il2CppAssembly* coreAss=0;
    int assWait=0;
    for (;assWait<5000 && !g_shuttingDown;assWait+=5) {
        if (!gameAss) gameAss=domainAssemblyOpen(domain,"Assembly-CSharp");
        if (!coreAss) coreAss=domainAssemblyOpen(domain,"UnityEngine.CoreModule");
        if (gameAss && coreAss) break;
        Sleep(5);
    }
    const Il2CppImage* gameImage=gameAss ? assemblyGetImage(gameAss) : 0;
    const Il2CppImage* coreImage=coreAss ? assemblyGetImage(coreAss) : 0;
    if (!gameImage || !coreImage || g_shuttingDown) {
        LogA("[H19 EARLY] Required metadata images unavailable.");
        if (attached) threadDetach(attachedThread);
        LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
        return 0;
    }
    LogIntA("[H19 EARLY] Assembly-CSharp/CoreModule available after additional ms: ",assWait);

    auto findClassSimple=[&](const Il2CppImage* img,const char* simple)->Il2CppClass* {
        if (!img || !simple) return 0;
        SIZE_T n=imageGetClassCount(img);
        for (SIZE_T i=0;i<n;i++) {
            Il2CppClass* k=imageGetClass(img,i);
            const char* name=k ? classGetName(k) : 0;
            if (name && StrEqA(name,simple)) return k;
        }
        return 0;
    };

    auto findMethod=[&](Il2CppClass* k,const char* name,int argc)->const MethodInfo* {
        if (!k || !name) return 0;
        void* it=0;
        const MethodInfo* m=0;
        while ((m=classGetMethods(k,&it))!=0) {
            const char* mn=methodGetName(m);
            if (mn && StrEqA(mn,name) && (int)methodGetParamCount(m)==argc) return m;
        }
        return 0;
    };

    Il2CppClass* resourcesClass=findClassSimple(coreImage,"Resources");
    Il2CppClass* objectClass=findClassSimple(coreImage,"Object");
    Il2CppClass* componentClass=findClassSimple(coreImage,"Component");

    const MethodInfo* findAll=resourcesClass ? findMethod(resourcesClass,"FindObjectsOfTypeAll",1) : 0;
    const MethodInfo* getName=objectClass ? classGetMethodFromName(objectClass,"get_name",0) : 0;
    const MethodInfo* getGameObject=componentClass ? classGetMethodFromName(componentClass,"get_gameObject",0) : 0;

    if (!findAll || !getName || !getGameObject) {
        LogA("[H19 EARLY] Resources/Object/Component bridge unresolved.");
        if (attached) threadDetach(attachedThread);
        LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
        return 0;
    }

    struct Target {
        const char* name;
        Il2CppClass* klass;
        int lastCount;
        char lastGoName[160];
        char lastComponentName[160];
    };

    Target targets[4];
    targets[0].name="IntroController";
    targets[1].name="RCLogoComponent";
    targets[2].name="SceneManager";
    targets[3].name="SkipIntroController";
    for (int t=0;t<4;t++) {
        targets[t].klass=0;
        targets[t].lastCount=-1;
        targets[t].lastGoName[0]=0;
        targets[t].lastComponentName[0]=0;
    }

    for (int t=0;t<4;t++) {
        targets[t].klass=findClassSimple(gameImage,targets[t].name);
        LogA("[H19 EARLY] target class:");
        LogA(targets[t].name);
        LogA(targets[t].klass ? "[H19 EARLY] metadata resolved." : "[H19 EARLY] metadata unresolved.");
    }

    int totalPolls=0;
    for (int elapsed=0;elapsed<=8000 && !g_shuttingDown;elapsed+=20) {
        ++totalPolls;

        for (int ti=0;ti<4;ti++) {
            Target& T=targets[ti];
            if (!T.klass) continue;

            const Il2CppType* type=classGetType(T.klass);
            Il2CppObject* typeObj=type ? typeGetObject(type) : 0;
            if (!typeObj) continue;

            void* pars[1]; pars[0]=typeObj;
            Il2CppException* ex=0;
            Il2CppObject* arrObj=runtimeInvoke(findAll,0,pars,&ex);
            if (ex || !arrObj) {
                if (ex && T.lastCount!=-2) {
                    LogA("[H19 EARLY] FindObjectsOfTypeAll threw for:");
                    LogA(T.name);
                    T.lastCount=-2;
                }
                continue;
            }

            Il2CppArray* arr=(Il2CppArray*)arrObj;
            SIZE_T count=arrayLength(arr);
            if (count>64) count=64;

            if ((int)count!=T.lastCount) {
                LogIntA("[H19 EARLY] elapsed ms: ",elapsed);
                LogA("[H19 EARLY] object-count transition for:");
                LogA(T.name);
                LogIntA("[H19 EARLY] loaded object count: ",(int)count);
                T.lastCount=(int)count;
            }

            if (!count) continue;

            unsigned int hdr=arrayObjectHeaderSize();
            Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
            for (SIZE_T oi=0;oi<count;oi++) {
                Il2CppObject* obj=elems[oi];
                if (!obj) continue;

                char compName[160]; compName[0]=0;
                ex=0;
                Il2CppObject* compNameObj=runtimeInvoke(getName,obj,0,&ex);
                if (!ex && compNameObj)
                    H19ManagedStringToAsciiLocal((Il2CppString*)compNameObj,stringChars,stringLength,compName,160);

                char goName[160]; goName[0]=0;
                ex=0;
                Il2CppObject* go=runtimeInvoke(getGameObject,obj,0,&ex);
                if (!ex && go) {
                    Il2CppObject* goNameObj=runtimeInvoke(getName,go,0,&ex);
                    if (!ex && goNameObj)
                        H19ManagedStringToAsciiLocal((Il2CppString*)goNameObj,stringChars,stringLength,goName,160);
                }

                BOOL changed=FALSE;
                if (compName[0] && !StrEqA(compName,T.lastComponentName)) changed=TRUE;
                if (goName[0] && !StrEqA(goName,T.lastGoName)) changed=TRUE;

                if (changed) {
                    LogIntA("[H19 EARLY] elapsed ms: ",elapsed);
                    LogA("[H19 EARLY] live target:");
                    LogA(T.name);
                    LogA("[H19 EARLY] component Unity name:");
                    LogA(compName[0] ? compName : "<empty>");
                    LogA("[H19 EARLY] GameObject name:");
                    LogA(goName[0] ? goName : "<empty>");
                    CopyA(T.lastComponentName,160,compName);
                    CopyA(T.lastGoName,160,goName);

                    if (StrEqA(T.name,"IntroController")) {
                        const FieldInfo* videoClip=classGetFieldFromName(T.klass,"_videoClip");
                        const FieldInfo* videoPlayer=classGetFieldFromName(T.klass,"_videoPlayer");
                        const FieldInfo* skipped=classGetFieldFromName(T.klass,"_skipped");
                        Il2CppObject* ref=0;
                        if (videoClip) {
                            fieldGetValue(obj,videoClip,&ref);
                            if (ref) {
                                Il2CppObject* nobj=runtimeInvoke(getName,ref,0,&ex);
                                char n[160]; n[0]=0;
                                if (!ex && nobj) H19ManagedStringToAsciiLocal((Il2CppString*)nobj,stringChars,stringLength,n,160);
                                LogA("[H19 EARLY] IntroController._videoClip name:");
                                LogA(n[0] ? n : "<empty>");
                            }
                        }
                        ref=0;
                        if (videoPlayer) {
                            fieldGetValue(obj,videoPlayer,&ref);
                            if (ref) {
                                Il2CppObject* nobj=runtimeInvoke(getName,ref,0,&ex);
                                char n[160]; n[0]=0;
                                if (!ex && nobj) H19ManagedStringToAsciiLocal((Il2CppString*)nobj,stringChars,stringLength,n,160);
                                LogA("[H19 EARLY] IntroController._videoPlayer name:");
                                LogA(n[0] ? n : "<empty>");
                            }
                        }
                        if (skipped) {
                            BYTE v=0;
                            fieldGetValue(obj,skipped,&v);
                            LogIntA("[H19 EARLY] IntroController._skipped: ",v?1:0);
                        }
                    } else if (StrEqA(T.name,"RCLogoComponent")) {
                        const FieldInfo* cursor=classGetFieldFromName(T.klass,"_cursorSprite");
                        Il2CppObject* ref=0;
                        if (cursor) {
                            fieldGetValue(obj,cursor,&ref);
                            if (ref) {
                                Il2CppObject* nobj=runtimeInvoke(getName,ref,0,&ex);
                                char n[160]; n[0]=0;
                                if (!ex && nobj) H19ManagedStringToAsciiLocal((Il2CppString*)nobj,stringChars,stringLength,n,160);
                                LogA("[H19 EARLY] RCLogoComponent._cursorSprite name:");
                                LogA(n[0] ? n : "<empty>");
                            }
                        }
                    } else if (StrEqA(T.name,"SceneManager")) {
                        const FieldInfo* introCluster=classGetFieldFromName(T.klass,"_introCluster");
                        Il2CppObject* ref=0;
                        if (introCluster) {
                            fieldGetValue(obj,introCluster,&ref);
                            if (ref) {
                                Il2CppObject* nobj=runtimeInvoke(getName,ref,0,&ex);
                                char n[160]; n[0]=0;
                                if (!ex && nobj) H19ManagedStringToAsciiLocal((Il2CppString*)nobj,stringChars,stringLength,n,160);
                                LogA("[H19 EARLY] SceneManager._introCluster name:");
                                LogA(n[0] ? n : "<empty>");
                                if (objectGetClass) {
                                    Il2CppClass* rk=objectGetClass(ref);
                                    if (rk) {
                                        const char* rns=classGetNamespace ? classGetNamespace(rk) : 0;
                                        const char* rn=classGetName(rk);
                                        LogA("[H19 EARLY] _introCluster runtime namespace:");
                                        LogA(rns && *rns ? rns : "<global namespace>");
                                        LogA("[H19 EARLY] _introCluster runtime class:");
                                        LogA(rn ? rn : "<unknown>");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Sleep(20);
    }

    LogIntA("[H19 EARLY] total polls completed: ",totalPolls);
    for (int ti=0;ti<4;ti++) {
        LogA("[H19 EARLY] final/last observed count for:");
        LogA(targets[ti].name);
        LogIntA("[H19 EARLY] count: ",targets[ti].lastCount<0 ? 0 : targets[ti].lastCount);
    }

    if (attached) threadDetach(attachedThread);
    LogA("[H19 EARLY] ===== isolated early startup-object watch end =====");
    return 0;
}

static DWORD __stdcall ModThread(void*) {
    BuildPaths();
    LogA("============================================================");
    LogA("PostalBorkenMenu V2A42-H19 EARLY STARTUP OBJECT WATCH TEST");
    LogA("[OK] PostalBorkenMenu.asi loaded by ASI loader");
    LogA("Target: native IL2CPP developer command bridge");
    LogA("No EXE/GameAssembly patching is performed.");
    LoadConfig();
    if (g_dormantExactNameResolver)
        LogA("[OK] Exact-name WeaponId resolver linked; cleaned Arsenal/Special dispatcher active.");
    else
        LogA("[ERROR] Exact-name WeaponId resolver was not retained.");
    LogA("[H19 EARLY] Skip Intro Videos is not under test; H19 watches startup objects only.");
    if (!InitIl2Cpp()) return 0;
    if (!CreateOverlayWindow()) return 0;
    if (!HookGameWindow()) return 0;
    LogA("[OK] Async keyboard polling enabled; hotkeys no longer depend on Unity WM_KEYDOWN delivery.");
    LogA("[READY] Five tabs active. Arsenal has 14 single weapons, Special has 4 non-Dong entries. Skip Intro checkbox persists in INI.");

    if (g_noCrosshair && g_gameHwnd) PostMessageW(g_gameHwnd,WM_APP_CROSSHAIR,0,0);

    MSG m;
    while (!g_shuttingDown) {
        while (PeekMessageW(&m,0,0,0,PM_REMOVE)) {
            if (m.message==0x0012) { g_shuttingDown=1; break; } // WM_QUIT
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        if (g_shuttingDown) break;
        PollHotkeys();
        Sleep(10);
    }

    // Same-thread overlay destruction. No Unity/IL2CPP calls are made here.
    if (g_menuHwnd && IsWindow(g_menuHwnd)) DestroyWindow(g_menuHwnd);
    g_menuHwnd=0;
    LogA("[SHUTDOWN] V2A8 worker ended cleanly; no further IL2CPP access is possible.");
    return 0;
}

extern "C" BOOL __stdcall DllMain(HMODULE module, DWORD reason, void*) {
    if (reason==DLL_PROCESS_ATTACH) {
        g_self=module;
        DisableThreadLibraryCalls(module);
        HANDLE watch=CreateThread(0,0,EarlyStartupObjectWatchH19,0,0,0);
        if (watch) CloseHandle(watch);
        HANDLE t=CreateThread(0,0,ModThread,0,0,0);
        if (t) CloseHandle(t);
    }
    return TRUE;
}
