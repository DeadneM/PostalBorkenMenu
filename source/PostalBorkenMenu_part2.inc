
static Il2CppClass* FindClassBySimpleName(const char* target) {
    if (!g_assemblyCSharp || !target) return 0;
    SIZE_T n=g_il2cpp.image_get_class_count(g_assemblyCSharp);
    for (SIZE_T i=0;i<n;i++) {
        Il2CppClass* c=g_il2cpp.image_get_class(g_assemblyCSharp,i);
        if (!c) continue;
        const char* name=g_il2cpp.class_get_name(c);
        if (name && StrEqA(name,target)) return c;
    }
    return 0;
}

static const MethodInfo* FindMethodExact(Il2CppClass* klass, const char* name, int count,
                                         Il2CppClass* p0=0, Il2CppClass* p1=0) {
    if (!klass || !name) return 0;
    void* iter=0;
    const MethodInfo* m=0;
    while ((m=g_il2cpp.class_get_methods(klass,&iter))!=0) {
        const char* mn=g_il2cpp.method_get_name(m);
        if (!mn || !StrEqA(mn,name)) continue;
        if ((int)g_il2cpp.method_get_param_count(m)!=count) continue;
        if (count>0 && p0) {
            const Il2CppType* t=g_il2cpp.method_get_param(m,0);
            if (!t || g_il2cpp.class_from_type(t)!=p0) continue;
        }
        if (count>1 && p1) {
            const Il2CppType* t=g_il2cpp.method_get_param(m,1);
            if (!t || g_il2cpp.class_from_type(t)!=p1) continue;
        }
        return m;
    }
    return 0;
}

static void LogResolvedCommand(const char* className, BOOL ok) {
    char b[160]; int p=0;
    const char* a=ok ? "[OK] Resolved command: " : "[WARN] Unresolved command: ";
    while (*a && p<150) b[p++]=*a++;
    if (className) while (*className && p<150) b[p++]=*className++;
    b[p]=0; LogA(b);
}

static void FormatExceptionToLog(Il2CppException* ex) {
    if (!ex) return;
    char b[1024]; b[0]=0;
    g_il2cpp.format_exception(ex,b,1024);
    if (b[0]) LogA(b); else LogA("[ERROR] IL2CPP exception (no formatted text)");
}

static Il2CppClass* g_inventoryClass = 0;
static const MethodInfo* g_giveAllWeapons = 0;
static Il2CppClass* g_crosshairControllerClass = 0;
static const FieldInfo* g_currentCrosshairField = 0;
static const MethodInfo* g_findObjectOfType2 = 0;
static const MethodInfo* g_componentGetGameObject = 0;
static const MethodInfo* g_gameObjectGetComponentsInChildren = 0;
static const MethodInfo* g_canvasRendererSetAlpha = 0;
static Il2CppObject* g_canvasRendererTypeObject = 0;
static Il2CppClass* g_timeManagerClass = 0;
static const MethodInfo* g_timeScaleSetter = 0;
static const MethodInfo* g_timeScaleGetter = 0;

static BOOL IsSynthetic(const char* n) {
    return n && n[0]=='_' && n[1]=='_';
}

static BOOL EnsureIl2CppThread(BOOL* attachedByUs, void** threadOut) {
    if (attachedByUs) *attachedByUs=FALSE;
    if (threadOut) *threadOut=0;
    if (!g_domain) return FALSE;
    void* cur=g_il2cpp.thread_current ? g_il2cpp.thread_current() : 0;
    if (cur) { if(threadOut)*threadOut=cur; return TRUE; }
    void* t=g_il2cpp.thread_attach(g_domain);
    if (!t) return FALSE;
    if (attachedByUs) *attachedByUs=TRUE;
    if (threadOut) *threadOut=t;
    return TRUE;
}

static void ReleaseIl2CppThread(BOOL attachedByUs, void* thread) {
    if (attachedByUs && thread && g_il2cpp.thread_detach) g_il2cpp.thread_detach(thread);
}

static int ParseArgsToAscii(const WCHAR* src, char out[8][128]) {
    int count=0; int i=0;
    if (!src) return 0;
    while (src[i] && count<8) {
        while (src[i]==L' ' || src[i]==L'\t') ++i;
        if (!src[i]) break;
        BOOL quoted=FALSE; if (src[i]==L'\"') { quoted=TRUE; ++i; }
        int j=0;
        while (src[i] && j<127) {
            if (quoted) { if (src[i]==L'\"') { ++i; break; } }
            else { if (src[i]==L' ' || src[i]==L'\t') break; }
            WCHAR w=src[i++]; out[count][j++]=(w>=32 && w<=126)?(char)w:'?';
        }
        out[count][j]=0;
        while (src[i] && src[i]!=L' ' && src[i]!=L'\t') ++i;
        ++count;
    }
    return count;
}

static Il2CppArray* BuildManagedArgs(const WCHAR* src) {
    char tok[8][128];
    int n=ParseArgsToAscii(src,tok);
    Il2CppArray* arr=g_il2cpp.array_new(g_stringClass,(SIZE_T)n);
    if (!arr) return 0;
    unsigned int hdr=g_il2cpp.array_object_header_size();
    void** elems=(void**)(((BYTE*)arr)+hdr);
    for (int i=0;i<n;i++) {
        Il2CppString* str=g_il2cpp.string_new(tok[i]);
        g_il2cpp.gc_wbarrier_set_field((Il2CppObject*)arr,&elems[i],str);
    }
    return arr;
}

static Il2CppObject* FindLiveObject(Il2CppClass* klass) {
    if (!klass || !g_findObjectOfType2) return 0;
    const Il2CppType* t=g_il2cpp.class_get_type(klass);
    if (!t) return 0;
    Il2CppObject* typeObj=g_il2cpp.type_get_object(t);
    if (!typeObj) return 0;
    BOOL includeInactive=TRUE;
    void* pars[2]; pars[0]=typeObj; pars[1]=&includeInactive;
    Il2CppException* ex=0;
    Il2CppObject* obj=g_il2cpp.runtime_invoke(g_findObjectOfType2,0,pars,&ex);
    if (ex) { LogA("[ERROR] FindObjectOfType(Type,bool) threw:"); FormatExceptionToLog(ex); return 0; }
    return obj;
}

static BOOL InvokeNativeCommand(int i) {
    if (i<0 || i>=CMD_COUNT) return FALSE;
    CommandRec& c=g_cmds[i];
    if (!c.execute) { LogA("[ERROR] Requested native command is unresolved"); return FALSE; }

    Il2CppObject* obj=0;
    unsigned int handle=0;
    if (g_il2cpp.method_is_instance(c.execute)) {
        obj=g_il2cpp.object_new(c.klass);
        if (!obj) { LogA("[ERROR] il2cpp_object_new failed"); return FALSE; }
        handle=g_il2cpp.gchandle_new(obj,FALSE);
        const MethodInfo* ctor=g_il2cpp.class_get_method_from_name(c.klass,".ctor",0);
        if (ctor) {
            Il2CppException* ex=0;
            g_il2cpp.runtime_invoke(ctor,obj,0,&ex);
            if (ex) {
                LogA("[ERROR] Command constructor threw:"); FormatExceptionToLog(ex);
                if(handle) g_il2cpp.gchandle_free(handle);
                return FALSE;
            }
        }
    }

    Il2CppArray* arr=BuildManagedArgs(c.args);
    if (!arr) {
        LogA("[ERROR] Failed to allocate command argument array");
        if(handle) g_il2cpp.gchandle_free(handle);
        return FALSE;
    }
    void* pars[1]; pars[0]=arr;
    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(c.execute,obj,pars,&ex);
    if (handle) g_il2cpp.gchandle_free(handle);
    if (ex) { LogA("[ERROR] Command Execute() threw:"); FormatExceptionToLog(ex); return FALSE; }
    LogResolvedCommand(c.className,TRUE);
    return TRUE;
}

static BOOL GiveAllWeaponsOnly() {
    if (!g_inventoryClass || !g_giveAllWeapons) { LogA("[ERROR] Give All Weapons bridge unresolved"); return FALSE; }
    Il2CppObject* inv=FindLiveObject(g_inventoryClass);
    if (!inv) { LogA("[ERROR] PlayerInventoryComponent instance not found"); return FALSE; }
    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(g_giveAllWeapons,inv,0,&ex);
    if (ex) { LogA("[ERROR] GiveAllWeapons() threw:"); FormatExceptionToLog(ex); return FALSE; }
    LogA("[OK] Give All Weapons executed (quest items untouched)");
    return TRUE;
}

static BOOL SetCrosshairVisualAlpha(float alpha) {
    if (!g_crosshairControllerClass || !g_currentCrosshairField || !g_componentGetGameObject ||
        !g_gameObjectGetComponentsInChildren || !g_canvasRendererSetAlpha || !g_canvasRendererTypeObject) {
        LogA("[ERROR] No Crosshair CanvasRenderer bridge unresolved");
        return FALSE;
    }

    Il2CppObject* controller=FindLiveObject(g_crosshairControllerClass);
    if (!controller) { LogA("[WARN] PlayerCrosshairController instance not found"); return FALSE; }

    // IMPORTANT: read the currently assigned Crosshair component, but NEVER clear or replace it.
    // Weapon aim logic continues to use the native crosshair object and its target position.
    Il2CppObject* crosshair=0;
    g_il2cpp.field_get_value(controller,g_currentCrosshairField,&crosshair);
    if (!crosshair) { LogA("[WARN] Current Crosshair object unavailable"); return FALSE; }

    Il2CppException* ex=0;
    Il2CppObject* go=g_il2cpp.runtime_invoke(g_componentGetGameObject,crosshair,0,&ex);
    if (ex || !go) { if(ex)FormatExceptionToLog(ex); return FALSE; }

    BOOL includeInactive=TRUE;
    void* pars[2]; pars[0]=g_canvasRendererTypeObject; pars[1]=&includeInactive;
    ex=0;
    Il2CppObject* boxedArr=g_il2cpp.runtime_invoke(g_gameObjectGetComponentsInChildren,go,pars,&ex);
    if (ex || !boxedArr) { if(ex)FormatExceptionToLog(ex); return FALSE; }

    Il2CppArray* arr=(Il2CppArray*)boxedArr;
    SIZE_T n=g_il2cpp.array_length(arr);
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
    int changed=0;
    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* renderer=elems[i];
        if (!renderer) continue;
        void* alphaPars[1]; alphaPars[0]=&alpha;
        ex=0;
        g_il2cpp.runtime_invoke(g_canvasRendererSetAlpha,renderer,alphaPars,&ex);
        if (ex) { FormatExceptionToLog(ex); continue; }
        ++changed;
    }
    if (!changed) { LogA("[WARN] No CanvasRenderer found under current Crosshair"); return FALSE; }
    return TRUE;
}

static BOOL ToggleNoCrosshair() {
    g_noCrosshair = g_noCrosshair ? 0 : 1;
    float a = g_noCrosshair ? 0.0f : 1.0f;
    BOOL ok=SetCrosshairVisualAlpha(a);
    WritePrivateProfileStringW(L"Settings",L"NoCrosshair",g_noCrosshair?L"1":L"0",g_iniPath);
    WritePrivateProfileStringW(0,0,0,g_iniPath);
    if (ok) LogA(g_noCrosshair ? "[OK] No Crosshair enabled (current Crosshair CanvasRenderers alpha=0; aim logic preserved)"
                               : "[OK] No Crosshair disabled");
    return ok;
}

static float ParseTimeScale(const WCHAR* a) {
    if (!a) return 1.0f;
    if (a[0]==L'0' && a[1]==L'.' && a[2]==L'5' && a[3]==0) return 0.5f;
    if (a[0]==L'2' && a[1]==0) return 2.0f;
    return 1.0f;
}

static float GetEngineTimeScale() {
    if (!g_timeScaleGetter) return g_lastAppliedTimeScale;
    Il2CppException* ex=0;
    Il2CppObject* boxed=g_il2cpp.runtime_invoke(g_timeScaleGetter,0,0,&ex);
    if (ex || !boxed) return g_lastAppliedTimeScale;
    void* raw=g_il2cpp.object_unbox(boxed);
    if (!raw) return g_lastAppliedTimeScale;
    return *(float*)raw;
}

static BOOL ApplyTimeScaleToggle(int i) {
    if (!g_timeScaleSetter || i<0 || i>=CMD_COUNT) { LogA("[ERROR] TimeScale bridge unresolved"); return FALSE; }
    float selected=ParseTimeScale(g_cmds[i].args);
    float current=GetEngineTimeScale();
    float target=selected;
    if (selected!=1.0f) {
        float d=current-selected; if(d<0)d=-d;
        if (d<0.001f) target=1.0f;
    }
    void* pars[1]; pars[0]=&target;
    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(g_timeScaleSetter,0,pars,&ex);
    if (ex) { LogA("[ERROR] TimeManager.SetGameTimeScale threw:"); FormatExceptionToLog(ex); return FALSE; }
    g_lastAppliedTimeScale=target;
    if (target==1.0f) LogA("[OK] TimeScale applied: 1.0x");
    else if (target==0.5f) LogA("[OK] TimeScale applied: 0.5x");
    else if (target==2.0f) LogA("[OK] TimeScale applied: 2.0x");
    return TRUE;
}

static void ExecuteCommandIndex(int i) {
    if (!g_ready || g_shuttingDown || i<0 || i>=CMD_COUNT) return;
    BOOL attached=FALSE; void* thread=0;
    if (!EnsureIl2CppThread(&attached,&thread)) return;

    if (StrEqA(g_cmds[i].className,"__NoCrosshair")) ToggleNoCrosshair();
    else if (StrEqA(g_cmds[i].className,"__GiveAllWeapons")) GiveAllWeaponsOnly();
    else if (StrEqA(g_cmds[i].className,"TimeScale")) ApplyTimeScaleToggle(i);
    else InvokeNativeCommand(i);

    ReleaseIl2CppThread(attached,thread);
}

static BOOL InitIl2Cpp() {
    LogA("[ .. ] Waiting for GameAssembly.dll...");
    for (int k=0;k<600 && !g_gameAssembly;k++) { g_gameAssembly=GetModuleHandleW(L"GameAssembly.dll"); if(!g_gameAssembly) Sleep(100); }
    if (!g_gameAssembly) { LogA("[ERROR] GameAssembly.dll was not loaded"); return FALSE; }
    LogA("[OK] GameAssembly.dll found");
    if (!ResolveIl2CppApi()) return FALSE;
    LogA("[OK] IL2CPP exports resolved");
    LogA("[ .. ] Delaying IL2CPP domain access by 2500 ms to avoid racing Unity startup...");
    Sleep(2500);
    for (int k=0;k<300 && !g_domain;k++) { g_domain=g_il2cpp.domain_get(); if(!g_domain) Sleep(100); }
    if (!g_domain) { LogA("[ERROR] IL2CPP domain unavailable"); return FALSE; }

    BOOL attached=FALSE; void* thread=0;
    if (!EnsureIl2CppThread(&attached,&thread)) { LogA("[ERROR] Could not attach startup worker to IL2CPP"); return FALSE; }

    const Il2CppAssembly* ass=0;
    for (int k=0;k<300 && !ass;k++) { ass=g_il2cpp.domain_assembly_open(g_domain,"Assembly-CSharp"); if(!ass) Sleep(100); }
    if (!ass) { ReleaseIl2CppThread(attached,thread); LogA("[ERROR] Assembly-CSharp unavailable"); return FALSE; }
    g_assemblyCSharp=g_il2cpp.assembly_get_image(ass);
    if (!g_assemblyCSharp) { ReleaseIl2CppThread(attached,thread); LogA("[ERROR] Assembly-CSharp image unavailable"); return FALSE; }

    const Il2CppAssembly* coreAss=g_il2cpp.domain_assembly_open(g_domain,"UnityEngine.CoreModule");
    g_coreImage=coreAss ? g_il2cpp.assembly_get_image(coreAss) : 0;
    if (!g_coreImage) { ReleaseIl2CppThread(attached,thread); LogA("[ERROR] UnityEngine.CoreModule unavailable"); return FALSE; }
    const Il2CppAssembly* uiAss=g_il2cpp.domain_assembly_open(g_domain,"UnityEngine.UIModule");
    g_uiImage=uiAss ? g_il2cpp.assembly_get_image(uiAss) : 0;
    if (!g_uiImage) LogA("[WARN] UnityEngine.UIModule unavailable; No Crosshair visual bridge may be unavailable");

    const Il2CppImage* corlib=g_il2cpp.get_corlib();
    g_stringClass=corlib ? g_il2cpp.class_from_name(corlib,"System","String") : 0;
    g_systemTypeClass=corlib ? g_il2cpp.class_from_name(corlib,"System","Type") : 0;
    g_boolClass=corlib ? g_il2cpp.class_from_name(corlib,"System","Boolean") : 0;
    g_floatClass=corlib ? g_il2cpp.class_from_name(corlib,"System","Single") : 0;
    if (!g_stringClass || !g_systemTypeClass || !g_boolClass || !g_floatClass) {
        ReleaseIl2CppThread(attached,thread);
        LogA("[ERROR] Required System classes unavailable");
        return FALSE;
    }

    int found=0, nativeCount=0;
    for (int i=0;i<CMD_COUNT;i++) {
        if (IsSynthetic(g_cmds[i].className)) continue;
        ++nativeCount;
        g_cmds[i].klass=FindClassBySimpleName(g_cmds[i].className);
        if (g_cmds[i].klass) g_cmds[i].execute=g_il2cpp.class_get_method_from_name(g_cmds[i].klass,"Execute",1);
        if (g_cmds[i].klass && g_cmds[i].execute && g_il2cpp.method_get_param_count(g_cmds[i].execute)==1) {
            ++found; LogResolvedCommand(g_cmds[i].className,TRUE);
        } else { g_cmds[i].execute=0; LogResolvedCommand(g_cmds[i].className,FALSE); }
    }

    Il2CppClass* objectClass=g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object");
    Il2CppClass* componentClass=g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Component");
    Il2CppClass* gameObjectClass=g_il2cpp.class_from_name(g_coreImage,"UnityEngine","GameObject");
    Il2CppClass* canvasRendererClass=g_uiImage ? g_il2cpp.class_from_name(g_uiImage,"UnityEngine","CanvasRenderer") : 0;
    g_findObjectOfType2=objectClass ? FindMethodExact(objectClass,"FindObjectOfType",2,g_systemTypeClass,g_boolClass) : 0;
    g_componentGetGameObject=componentClass ? FindMethodExact(componentClass,"get_gameObject",0) : 0;
    g_gameObjectGetComponentsInChildren=gameObjectClass ? FindMethodExact(gameObjectClass,"GetComponentsInChildren",2,g_systemTypeClass,g_boolClass) : 0;
    g_canvasRendererSetAlpha=canvasRendererClass ? FindMethodExact(canvasRendererClass,"SetAlpha",1,g_floatClass) : 0;
    if (canvasRendererClass) {
        const Il2CppType* ct=g_il2cpp.class_get_type(canvasRendererClass);
        if (ct) g_canvasRendererTypeObject=g_il2cpp.type_get_object(ct);
    }

    g_inventoryClass=FindClassBySimpleName("PlayerInventoryComponent");
    g_giveAllWeapons=g_inventoryClass ? g_il2cpp.class_get_method_from_name(g_inventoryClass,"GiveAllWeapons",0) : 0;

    g_crosshairControllerClass=FindClassBySimpleName("PlayerCrosshairController");
    g_currentCrosshairField=g_crosshairControllerClass ? g_il2cpp.class_get_field_from_name(g_crosshairControllerClass,"_currentCrosshair") : 0;

    g_timeManagerClass=FindClassBySimpleName("TimeManager");
    g_timeScaleSetter=g_timeManagerClass ? FindMethodExact(g_timeManagerClass,"SetGameTimeScale",1,g_floatClass) : 0;
    g_timeScaleGetter=g_timeManagerClass ? FindMethodExact(g_timeManagerClass,"get_GameTimeScale",0) : 0;

    if (g_findObjectOfType2 && g_giveAllWeapons) LogA("[OK] Give All Weapons bridge resolved");
    else LogA("[WARN] Give All Weapons bridge incomplete");
    if (g_findObjectOfType2 && g_crosshairControllerClass && g_currentCrosshairField && g_componentGetGameObject &&
        g_gameObjectGetComponentsInChildren && g_canvasRendererSetAlpha && g_canvasRendererTypeObject)
        LogA("[OK] No Crosshair CanvasRenderer bridge resolved");
    else LogA("[WARN] No Crosshair CanvasRenderer bridge incomplete");
    if (g_timeScaleSetter) LogA("[OK] TimeManager.SetGameTimeScale bridge resolved");
    else LogA("[WARN] TimeManager.SetGameTimeScale bridge unresolved");
    if (g_timeScaleGetter) LogA("[OK] TimeManager.get_GameTimeScale bridge resolved");
    else LogA("[WARN] TimeManager.get_GameTimeScale bridge unavailable; local-state fallback active");

    ReleaseIl2CppThread(attached,thread);
    LogA("[OK] Startup worker detached from IL2CPP after metadata resolution");

    if (found==0) { LogA("[ERROR] No native developer command classes resolved"); return FALSE; }
    if (found==nativeCount) LogA("[OK] All native developer commands resolved by IL2CPP name");
    else LogA("[WARN] Only a subset of native developer commands resolved");
    g_ready=1;
    return TRUE;
}

