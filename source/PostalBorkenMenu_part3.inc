
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
    g_skipIntroVideos=0; // game already owns its own Skip Intro Videos option; mod leaves it alone
    g_skipStartupLogos=(LONG)GetPrivateProfileIntW(L"Settings",L"SkipStartupLogos",1,g_iniPath);
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
    if(i<0 || i>=CMD_COUNT) return -1;
    const char* n=g_cmds[i].className;

    // H43 safe cleanup: preserve command records/indices, hide only UI rows.
    if (StrEqA(n,"Help") || StrEqA(n,"LogTime") || StrEqA(n,"__SkipIntroVideos"))
        return -1;

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

static void UpdateOverlayPosition();

static const int g_h71WheelPointX[9]={0,64,98,87,34,-34,-87,-98,-64};
static const int g_h71WheelPointY[9]={-100,-77,-17,50,94,94,50,-17,-77};

static const WCHAR* VirtualWheelLabelH71(int category,int slot) {
    static const WCHAR* base[10]={
        L"",
        L"1  SHOVEL",
        L"2  PISTOL",
        L"3  SHOTGUN",
        L"4  MACHINE GUN",
        L"5  COMMON SLOT",
        L"6  COMMON SLOT",
        L"7  COMMON SLOT",
        L"8  DILDO BOW",
        L"9  COMMON SLOT"
    };
    static const WCHAR* dlc[10]={
        L"",
        L"1  UMDRILL",
        L"2  PISS GUN",
        L"3  MEAT SHOTGUN",
        L"4  BUBBLE GUN",
        L"5  COMMON SLOT",
        L"6  COMMON SLOT",
        L"7  COMMON SLOT",
        L"8  NUCLEAR SYRINGE",
        L"9  COMMON SLOT"
    };
    if(slot<1 || slot>9) return L"";
    return category==1 ? dlc[slot] : base[slot];
}

static void VirtualWheelPointH71(int slot,int w,int h,int* outX,int* outY) {
    if(!outX || !outY || slot<1 || slot>9) return;
    int minDim=w<h ? w : h;
    int radius=minDim/3;
    if(radius<120) radius=120;
    if(radius>260) radius=260;
    *outX=w/2 + (g_h71WheelPointX[slot-1]*radius)/100;
    *outY=h/2 + (g_h71WheelPointY[slot-1]*radius)/100;
}

static void PaintVirtualWheelH71(HWND hwnd) {
    PAINTSTRUCT ps={};
    HDC dc=BeginPaint(hwnd,&ps);
    if(!dc) return;

    RECT cr={};
    GetClientRect(hwnd,&cr);
    int w=cr.right-cr.left;
    int h=cr.bottom-cr.top;

    Fill(dc,0,0,w,h,C(8,9,12));
    Text(dc,
         g_h71VirtualWheelCategory==1 ? L"VIRTUAL DLC WEAPON WHEEL" : L"VIRTUAL NORMAL WEAPON WHEEL",
         0,18,w,50,C(245,245,248),DT_CENTER);
    Text(dc,L"Move cursor or press 1-9  •  Release wheel key to equip",
         0,50,w,78,C(175,180,190),DT_CENTER);

    for(int slot=1;slot<=9;slot++) {
        int x=0,y=0;
        VirtualWheelPointH71(slot,w,h,&x,&y);
        int bw=150,bh=46;
        BOOL selected=(slot==g_h71VirtualWheelSelectedSlot);
        Fill(dc,x-bw/2,y-bh/2,x+bw/2,y+bh/2,
             selected ? C(120,66,148) : C(38,41,49));
        Text(dc,VirtualWheelLabelH71(g_h71VirtualWheelCategory,slot),
             x-bw/2+4,y-bh/2,x+bw/2-4,y+bh/2,
             selected ? C(255,255,255) : C(215,218,224),DT_CENTER);
    }

    Text(dc,L"H71 minimal functional test  •  no native cross-mode wheel calls",
         0,h-44,w,h-16,C(145,150,160),DT_CENTER);

    EndPaint(hwnd,&ps);
}

static void ShowVirtualWheelH71(int category,int vk) {
    if(!g_menuHwnd || !g_gameHwnd || g_h71VirtualWheelActive) return;

    RECT r={}; POINT p={0,0};
    if(!GetClientRect(g_gameHwnd,&r) || !ClientToScreen(g_gameHwnd,&p)) return;

    int w=r.right-r.left;
    int h=r.bottom-r.top;
    if(w<320 || h<200) return;

    g_h71VirtualWheelCategory=(category==1)?1:0;
    g_h71VirtualWheelVk=vk;
    g_h71VirtualWheelSelectedSlot=1;
    g_h71VirtualWheelActive=1;

    SetLayeredWindowAttributes(g_menuHwnd,0,190,LWA_ALPHA);
    SetWindowPos(g_menuHwnd,(HWND)(LONG_PTR)-1,p.x,p.y,w,h,SWP_NOACTIVATE);
    ShowWindow(g_menuHwnd,SW_SHOWNOACTIVATE);
    InvalidateRect(g_menuHwnd,0,TRUE);

    LogA(category==1 ? "[WHEEL H71] virtual DLC wheel opened." :
                       "[WHEEL H71] virtual BASE wheel opened.");
}

static void UpdateVirtualWheelSelectionH71() {
    if(!g_h71VirtualWheelActive || !g_gameHwnd) return;

    POINT cur={0,0};
    POINT origin={0,0};
    RECT r={};
    if(!GetCursorPos(&cur) || !GetClientRect(g_gameHwnd,&r) || !ClientToScreen(g_gameHwnd,&origin))
        return;

    int w=r.right-r.left;
    int h=r.bottom-r.top;
    int cx=origin.x+w/2;
    int cy=origin.y+h/2;
    LONG dx=cur.x-cx;
    LONG dy=cur.y-cy;

    // Many FPS games keep the OS cursor centered while using raw mouse input.
    // In that case keep the previous selection; number keys remain available.
    if((dx*dx + dy*dy) < (70*70))
        return;

    int bestSlot=g_h71VirtualWheelSelectedSlot;
    unsigned long bestDist=0xFFFFFFFFUL;

    for(int slot=1;slot<=9;slot++) {
        int px=0,py=0;
        VirtualWheelPointH71(slot,w,h,&px,&py);
        LONG sx=(LONG)(origin.x+px)-cur.x;
        LONG sy=(LONG)(origin.y+py)-cur.y;
        unsigned long dist=(unsigned long)(sx*sx + sy*sy);
        if(dist<bestDist) {
            bestDist=dist;
            bestSlot=slot;
        }
    }

    if(bestSlot!=g_h71VirtualWheelSelectedSlot) {
        g_h71VirtualWheelSelectedSlot=bestSlot;
        InvalidateRect(g_menuHwnd,0,TRUE);
    }
}

static void HideVirtualWheelH71(BOOL commitSelection) {
    if(!g_h71VirtualWheelActive) return;

    int category=g_h71VirtualWheelCategory;
    int slot=g_h71VirtualWheelSelectedSlot;

    g_h71VirtualWheelActive=0;
    g_h71VirtualWheelVk=0;
    ShowWindow(g_menuHwnd,SW_HIDE);
    SetLayeredWindowAttributes(g_menuHwnd,0,242,LWA_ALPHA);
    UpdateOverlayPosition();

    if(commitSelection) {
        LogIntA("[WHEEL H71] virtual wheel release slot: ",slot);
        EquipVirtualWheelSlotH71(category,slot);
    } else {
        LogA("[WHEEL H71] virtual wheel cancelled.");
    }
}

static void PaintOverlay(HWND hwnd) {
    if(g_h71VirtualWheelActive) {
        PaintVirtualWheelH71(hwnd);
        return;
    }
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
        BOOL isSkipStartup=StrEqA(g_cmds[i].className,"__SkipStartupLogos");
        if (isSkipIntro || isSkipStartup) {
            BOOL enabled=isSkipIntro ? (g_skipIntroVideos?TRUE:FALSE) : (g_skipStartupLogos?TRUE:FALSE);
            Fill(dc,493,y+4,585,y+ROW_H-6,C(43,46,54));
            Fill(dc,501,y+7,517,y+ROW_H-9,C(28,30,35));
            if(enabled) Text(dc,L"X",501,y+5,517,y+ROW_H-7,C(120,230,150),DT_CENTER);
            Text(dc,enabled?L"ENABLED":L"DISABLED",522,y+4,585,y+ROW_H-6,
                 enabled?C(120,230,150):C(200,200,205),DT_CENTER);
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
            if(StrEqA(g_cmds[i].className,"__SkipStartupLogos")) {
                if(px>=493 && px<744) ToggleSkipStartupLogos();
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
        g_h71VirtualWheelActive=0;
        g_h71VirtualWheelVk=0;
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
    if (msg==WM_APP_BASEWHEELFILTER) {
        ExecuteDelayedBaseFilterH51();
        return 0;
    }
    if (msg==WM_APP_DLCWHEELSWAP) {
        ExecuteDelayedDlcSwapH54();
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
    if (!active && g_h71VirtualWheelActive) {
        HideVirtualWheelH71(FALSE);
    }

    // H51 base-wheel behavior is intentionally preserved byte-for-byte.
    static int h51FilterDivider=0;
    if(active && g_h51BaseFilterPending && g_gameHwnd) {
        if(++h51FilterDivider>=3) {
            h51FilterDivider=0;
            PostMessageW(g_gameHwnd,WM_APP_BASEWHEELFILTER,0,0);
        }
    } else {
        h51FilterDivider=0;
    }

    // H54 affects only DLC Weapon Wheel in normal campaign.
    static int h54DlcSwapDivider=0;
    if(active && g_h54DlcSwapPending && g_gameHwnd) {
        if(++h54DlcSwapDivider>=3) {
            h54DlcSwapDivider=0;
            PostMessageW(g_gameHwnd,WM_APP_DLCWHEELSWAP,0,0);
        }
    } else {
        h54DlcSwapDivider=0;
    }

    // H71 performs all expensive discovery away from the hotkey path.
    static int h71BackgroundDivider=0;
    if(active && !g_h71VirtualWheelActive && ++h71BackgroundDivider>=200) {
        h71BackgroundDivider=0;
        RefreshVirtualWheelRuntimeH71();
    }

    if(g_h71VirtualWheelActive)
        UpdateVirtualWheelSelectionH71();

    for (int vk=8; vk<256; ++vk) {
        if (g_shuttingDown) return;

        BYTE was=g_keyPrev[vk];
        BYTE down=(GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;
        BYTE pressed=(down && !was) ? 1 : 0;
        BYTE released=(!down && was) ? 1 : 0;
        g_keyPrev[vk]=down;

        if (!active || (!pressed && !released)) continue;

        if(g_h71VirtualWheelActive) {
            if(released && vk==g_h71VirtualWheelVk) {
                HideVirtualWheelH71(TRUE);
                return;
            }
            if(pressed && vk>=0x31 && vk<=0x39) {
                g_h71VirtualWheelSelectedSlot=vk-0x30;
                if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
                return;
            }
            if(pressed && vk==VK_ESCAPE) {
                HideVirtualWheelH71(FALSE);
                return;
            }
        }

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
                        // In DLC scenes, BASE is cross-mode: use H71 virtual wheel.
                        if(g_h71SceneMode==2) {
                            if(g_dlcWheelActive) PostMessageW(g_gameHwnd,WM_APP_DLCWHEEL,0,0);
                            ShowVirtualWheelH71(0,vk);
                            return;
                        }

                        // Normal scene keeps the native base wheel untouched.
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
                        // In normal scenes, DLC is cross-mode: use H71 virtual wheel.
                        if(g_h71SceneMode==1) {
                            if(g_baseWheelActive) PostMessageW(g_gameHwnd,WM_APP_BASEWHEEL,0,0);
                            ShowVirtualWheelH71(1,vk);
                            return;
                        }

                        // Native DLC scene keeps the native DLC wheel untouched.
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
            if(g_h71VirtualWheelActive) HideVirtualWheelH71(FALSE);
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

struct H63WindowSearch {
    DWORD pid;
    HWND best;
    LONG bestArea;
};

static BOOL __stdcall EnumGameWindowsH63(HWND hwnd, LPARAM lp) {
    H63WindowSearch* s=(H63WindowSearch*)lp;
    if(!s || !hwnd || hwnd==g_menuHwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return TRUE;

    DWORD wp=0;
    GetWindowThreadProcessId(hwnd,&wp);
    if(wp!=s->pid) return TRUE;

    RECT r={};
    if(!GetClientRect(hwnd,&r)) return TRUE;

    LONG w=r.right-r.left;
    LONG h=r.bottom-r.top;
    if(w<320 || h<200) return TRUE;

    LONG area=w*h;
    if(area>s->bestArea) {
        s->best=hwnd;
        s->bestArea=area;
    }
    return TRUE;
}

static HWND FindOwnGameWindowH63() {
    H63WindowSearch s={};
    s.pid=GetCurrentProcessId();
    s.best=0;
    s.bestArea=0;
    EnumWindows(EnumGameWindowsH63,(LPARAM)&s);
    return s.best;
}

static BOOL HookGameWindow() {
    HWND candidate=0;
    int stableSamples=0;
    int quietTicks=0;

    LogA("[WINDOW H63] Waiting for the game's own top-level window; foreground focus is NOT required.");

    while(!g_shuttingDown) {
        HWND h=FindOwnGameWindowH63();
        BOOL valid=(h && h!=g_menuHwnd && IsWindow(h));

        if(valid) {
            if(h==candidate) {
                ++stableSamples;
            } else {
                candidate=h;
                stableSamples=1;
                quietTicks=0;
                LogA("[WINDOW H63] Process-owned game window candidate detected.");
            }

            // Half a second is enough to reject transient launch windows while
            // avoiding the old 3-second foreground-only dependency.
            if(stableSamples>=5) {
                LONG_PTR old=SetWindowLongPtrW(candidate,GWLP_WNDPROC,(LONG_PTR)GameWndProc);
                if(old) {
                    g_gameHwnd=candidate;
                    g_oldGameWndProc=(WNDPROC)old;
                    LogA("[OK] H63 process-owned game window subclass installed - F1 overlay ready");
                    return TRUE;
                }

                LogA("[WINDOW H63] Subclass attempt failed; will keep retrying instead of terminating the mod.");
                candidate=0;
                stableSamples=0;
            }
        } else {
            candidate=0;
            stableSamples=0;

            // Keep the log useful without spamming it forever.
            ++quietTicks;
            if(quietTicks==1 || (quietTicks%100)==0)
                LogA("[WINDOW H63] Game window not ready yet; retrying in background.");
        }

        Sleep(100);
    }

    return FALSE;
}

static DWORD __stdcall ModThread(void*) {
    BuildPaths();
    LoadConfig();

    // H39 runs before normal H13 initialization and calls Unity's own native
    // CancelSplashScreen when the exact state machine enters Begin/Fade.
    H39WatchAndCancelNativeSplash();

    LogA("============================================================");
    LogA("PostalBorkenMenu V2A42-H71 VIRTUAL CROSS-MODE WHEEL TEST");
    LogA("[OK] PostalBorkenMenu.asi loaded by ASI loader");
    LogA("Target: native IL2CPP developer command bridge");
    LogA("No EXE/GameAssembly/Unity asset file patching is performed.");
    LogIntA("[SPLASH H39] UnityPlayer exact-build verification: ",(int)g_h39UnityPlayerVerified);
    LogIntA("[SPLASH H39] Real CancelSplashScreen calls: ",(int)g_h39CancelCalls);
    LogIntA("[SPLASH H39] Confirmed state=3/active=0 after cancel: ",(int)g_h39CancelConfirmed);
    LogIntA("[SPLASH H39] Re-Begin events seen after cancel: ",(int)g_h39RebeginsSeen);
    LogIntA("[SPLASH H39] Last observed native splash state: ",(int)g_h39LastState);
    if (g_dormantExactNameResolver)
        LogA("[OK] Exact-name WeaponId resolver linked; cleaned Arsenal/Special dispatcher active.");
    else
        LogA("[ERROR] Exact-name WeaponId resolver was not retained.");
    LogA(g_skipStartupLogos ? "[SPLASH H40] Skip Startup Logos config: ENABLED (default)." : "[SPLASH H40] Skip Startup Logos config: DISABLED.");
    if (!InitIl2Cpp()) return 0;

    // H62: pay the one global Sprite enumeration during startup instead of
    // stalling the first wheel shortcut. Later opens use cached references.
    PrewarmWheelSpriteCacheH62();

    // H71 resolves WeaponIds and scene mode before hotkeys become active.
    RefreshVirtualWheelRuntimeH71();

    if (!CreateOverlayWindow()) return 0;
    if (!HookGameWindow()) return 0;
    LogA("[OK] Async keyboard polling enabled; hotkeys no longer depend on Unity WM_KEYDOWN delivery.");
    LogA("[READY] H71 preserves H65 native wheels in native modes; cross-mode uses a separate cached virtual wheel with no native wheel remap.");

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
