
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

    int listCategory=0, listSlot=0;
    if (DecodeWeaponListCommand(i,&listCategory,&listSlot)) return 2; // Weapon List

    if (StrEqA(n,"GiveAll") ||
        StrEqA(n,"__GiveAllWeapons") ||
        StrEqA(n,"__GiveAllDlcWeapons") ||
        StrEqA(n,"__InitDlcHook") ||
        StrEqA(n,"__WeaponKeysMode") ||
        StrEqA(n,"__BaseWeaponWheel") ||
        StrEqA(n,"__DlcWeaponWheel")) return 1; // Weapons

    if (StrEqA(n,"NoHud") ||
        StrEqA(n,"__NoCrosshair") ||
        StrEqA(n,"NoHudWithCrossHair") ||
        StrEqA(n,"NoModel") ||
        StrEqA(n,"NoMenu")) return 3; // Visual

    return 0; // Gameplay
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
    if(tab<0 || tab>3) return;
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

    // Top-level submenus. They only filter what is displayed; command state,
    // bindings and INI persistence remain shared and unchanged.
    Fill(dc,18,59,158,84,g_menuTab==0?C(70,74,86):C(38,40,47));
    Fill(dc,166,59,306,84,g_menuTab==1?C(70,74,86):C(38,40,47));
    Fill(dc,314,59,486,84,g_menuTab==2?C(70,74,86):C(38,40,47));
    Fill(dc,494,59,634,84,g_menuTab==3?C(70,74,86):C(38,40,47));
    Text(dc,L"GAMEPLAY",18,59,158,84,C(238,238,240),DT_CENTER);
    Text(dc,L"WEAPONS",166,59,306,84,C(238,238,240),DT_CENTER);
    Text(dc,L"WEAPON LIST",314,59,486,84,C(238,238,240),DT_CENTER);
    Text(dc,L"VISUAL",494,59,634,84,C(238,238,240),DT_CENTER);

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
        if (g_bindIndex==i) Fill(dc,598,y+3,738,y+ROW_H-5,C(85,54,34));
        Text(dc,g_cmds[i].displayName,25,y,326,y+ROW_H-2,C(232,233,236),DT_LEFT);
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
    Text(dc,L"Tabs: Gameplay / Weapons / Weapon List / Visual  •  Mouse wheel scroll  •  F1 close",22,fy,738,fy+28,C(145,148,156),DT_LEFT);
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
        if(px>=18 && px<158) SelectMenuTab(0);
        else if(px>=166 && px<306) SelectMenuTab(1);
        else if(px>=314 && px<486) SelectMenuTab(2);
        else if(px>=494 && px<634) SelectMenuTab(3);
        return TRUE;
    }

    if(py>=ROW_Y && py<ROW_Y+VISIBLE_ROWS*ROW_H) {
        int visibleIndex=(py-ROW_Y)/ROW_H;
        int i=TabCommandIndex(g_scrollRow+visibleIndex);
        int ry=(py-ROW_Y)%ROW_H;
        if(i>=0 && i<CMD_COUNT && ry<ROW_H-1) {
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
    for (int k=0;k<600;k++) {
        HWND h=GetForegroundWindow();
        if (h && h!=g_menuHwnd) {
            DWORD wp=0; GetWindowThreadProcessId(h,&wp);
            if (wp==pid) {
                LONG_PTR old=SetWindowLongPtrW(h,GWLP_WNDPROC,(LONG_PTR)GameWndProc);
                if (old) { g_gameHwnd=h; g_oldGameWndProc=(WNDPROC)old; LogA("[OK] Game window subclass installed - F1 overlay ready"); return TRUE; }
            }
        }
        Sleep(100);
    }
    LogA("[ERROR] Could not find/subclass the game window");
    return FALSE;
}

static DWORD __stdcall ModThread(void*) {
    BuildPaths();
    LogA("============================================================");
    LogA("PostalBorkenMenu V2A32 HOOK SLOT99 ADDWEAPON TEST");
    LogA("[OK] PostalBorkenMenu.asi loaded by ASI loader");
    LogA("Target: native IL2CPP developer command bridge");
    LogA("No EXE/GameAssembly patching is performed.");
    LoadConfig();
    if (!InitIl2Cpp()) return 0;
    if (!CreateOverlayWindow()) return 0;
    if (!HookGameWindow()) return 0;
    LogA("[OK] Async keyboard polling enabled; hotkeys no longer depend on Unity WM_KEYDOWN delivery.");
    LogA("[READY] F1 overlay tabs: Gameplay / Weapons / Weapon List / Visual. V2A29 wheels preserved; Hook Slot99 AddWeapon isolation test active.");

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
        HANDLE t=CreateThread(0,0,ModThread,0,0,0);
        if (t) CloseHandle(t);
    }
    return TRUE;
}
