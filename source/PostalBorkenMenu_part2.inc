
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

static int ParseFirstIntW(const WCHAR* s, int fallback) {
    if (!s) return fallback;
    int i=0;
    while (s[i]==L' ' || s[i]==L'\t') ++i;
    BOOL neg=FALSE;
    if (s[i]==L'-') { neg=TRUE; ++i; }
    int v=0, digits=0;
    while (s[i]>=L'0' && s[i]<=L'9') {
        v=v*10+(int)(s[i]-L'0');
        ++i; ++digits;
        if (v>999) break;
    }
    if (!digits) return fallback;
    return neg ? -v : v;
}

static void LogIntA(const char* prefix, int v) {
    char b[192]; int p=0;
    if (prefix) while (*prefix && p<170) b[p++]=*prefix++;
    unsigned int u;
    if (v<0) { if(p<180)b[p++]='-'; u=(unsigned int)(-v); }
    else u=(unsigned int)v;
    char tmp[16]; int n=0;
    do { tmp[n++]=(char)('0'+(u%10)); u/=10; } while (u && n<15);
    while (n>0 && p<190) b[p++]=tmp[--n];
    b[p]=0; LogA(b);
}


static BOOL IsTheseSunnyDazeOwned();

static int CollectionContainsWeaponId(Il2CppObject* collection, Il2CppObject* weapon) {
    if (!collection || !weapon) return 0;

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    if (!objectGetClass) return -1;

    Il2CppClass* klass=objectGetClass(collection);
    if (!klass) return -1;
    const char* name=g_il2cpp.class_get_name(klass);
    if (!name) return -1;

    int n=StrLenA(name);
    if (n>=2 && name[n-2]=='[' && name[n-1]==']') {
        Il2CppArray* arr=(Il2CppArray*)collection;
        SIZE_T len=g_il2cpp.array_length(arr);
        if (len>1024) return -1;
        unsigned int hdr=g_il2cpp.array_object_header_size();
        Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
        for (SIZE_T i=0;i<len;i++) if (elems[i]==weapon) return 1;
        return 0;
    }

    const MethodInfo* contains=g_il2cpp.class_get_method_from_name(klass,"Contains",1);
    if (!contains) return -1;

    void* pars[1]; pars[0]=weapon;
    Il2CppException* ex=0;
    Il2CppObject* boxed=g_il2cpp.runtime_invoke(contains,collection,pars,&ex);
    if (ex || !boxed) return -1;
    void* raw=g_il2cpp.object_unbox(boxed);
    if (!raw) return -1;
    return (*(BYTE*)raw)!=0 ? 1 : 0;
}

static int IsWeaponCollected(Il2CppObject* playerInv, Il2CppClass* inventoryClass, Il2CppObject* weapon) {
    if (!playerInv || !inventoryClass || !weapon) return -1;
    const MethodInfo* getCollected=g_il2cpp.class_get_method_from_name(inventoryClass,"get_CollectedWeapons",0);
    if (!getCollected) return -1;

    Il2CppException* ex=0;
    Il2CppObject* collection=g_il2cpp.runtime_invoke(getCollected,playerInv,0,&ex);
    if (ex) return -1;
    if (!collection) return 0;

    return CollectionContainsWeaponId(collection,weapon);
}

static BOOL SafeAddAndEquipWeaponId(Il2CppObject* playerInv,
                                    Il2CppObject* weaponsInv,
                                    Il2CppClass* inventoryClass,
                                    Il2CppClass* weaponsInventoryClass,
                                    Il2CppClass* weaponIdClass,
                                    Il2CppObject* selected,
                                    const char* logPrefix) {
    if (!playerInv || !weaponsInv || !inventoryClass || !weaponsInventoryClass || !weaponIdClass || !selected)
        return FALSE;

    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    const MethodInfo* equipWeapon=FindMethodExact(weaponsInventoryClass,"EquipWeapon",1,weaponIdClass);
    const MethodInfo* getCurrent=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"get_CurrentWeaponNoAkimbo",0);
    if (!addWeapon || !equipWeapon) return FALSE;

    Il2CppException* ex=0;
    if (getCurrent) {
        Il2CppObject* current=g_il2cpp.runtime_invoke(getCurrent,weaponsInv,0,&ex);
        if (!ex && current==selected) {
            LogA("[SAFE] Selected WeaponId is already equipped; skipping AddWeapon and EquipWeapon.");
            return TRUE;
        }
    }

    int collected=IsWeaponCollected(playerInv,inventoryClass,selected);
    if (collected==1) {
        LogA("[SAFE] Selected WeaponId is already collected; skipping AddWeapon.");
    } else {
        if (collected<0)
            LogA("[SAFE] CollectedWeapons state could not be inspected; keeping validated AddWeapon fallback.");
        else
            LogA("[SAFE] Selected WeaponId is not collected yet; AddWeapon is required.");

        void* wp[1]; wp[0]=selected;
        ex=0;
        g_il2cpp.runtime_invoke(addWeapon,weaponsInv,wp,&ex);
        if (ex) {
            LogA("[ERROR] Safe direct AddWeapon threw:");
            FormatExceptionToLog(ex);
            return FALSE;
        }
        LogA(logPrefix ? logPrefix : "[SAFE] Native AddWeapon completed");
    }

    void* wp[1]; wp[0]=selected;
    ex=0;
    g_il2cpp.runtime_invoke(equipWeapon,weaponsInv,wp,&ex);
    if (ex) {
        LogA("[ERROR] Safe direct EquipWeapon threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    LogA("[SAFE] Native EquipWeapon completed without duplicate AddWeapon.");
    return TRUE;
}

static BOOL IsHookDiscoveryName(const char* name) {
    if (!name) return FALSE;
    return StrContainsNoCaseA(name,"hook") ||
           StrContainsNoCaseA(name,"grap") ||
           StrContainsNoCaseA(name,"rope") ||
           StrContainsNoCaseA(name,"cable") ||
           StrContainsNoCaseA(name,"tether");
}

static void LogClassIdentity(Il2CppClass* klass, const char* prefix) {
    if (!klass) return;
    const char* ns=g_il2cpp.class_get_namespace(klass);
    const char* name=g_il2cpp.class_get_name(klass);
    LogA(prefix ? prefix : "[HOOK DEEP] class");
    if (ns && ns[0]) { LogA("[HOOK DEEP] namespace:"); LogA(ns); }
    if (name) { LogA("[HOOK DEEP] class name:"); LogA(name); }
}

static void LogMethodReturnType(const MethodInfo* m) {
    if (!m || !g_il2cpp.method_get_return_type) return;
    const Il2CppType* rt=g_il2cpp.method_get_return_type(m);
    Il2CppClass* rc=rt ? g_il2cpp.class_from_type(rt) : 0;
    if (!rc) {
        LogA("[HOOK DEEP] return type: <unresolved>");
        return;
    }
    const char* ns=g_il2cpp.class_get_namespace(rc);
    const char* name=g_il2cpp.class_get_name(rc);
    LogA("[HOOK DEEP] return type:");
    if (ns && ns[0]) LogA(ns);
    if (name) LogA(name);
}

static void LogAllFieldsForClass(Il2CppClass* klass, const char* header) {
    if (!klass || !g_il2cpp.class_get_fields || !g_il2cpp.field_get_name) return;
    LogA(header);
    void* iter=0;
    const FieldInfo* field=0;
    int count=0;
    while ((field=g_il2cpp.class_get_fields(klass,&iter))!=0 && count<128) {
        const char* name=g_il2cpp.field_get_name(field);
        if (name) {
            LogA("[HOOK FIELD]");
            LogA(name);
            ++count;
        }
    }
    LogIntA("[HOOK DEEP] fields logged: ",count);
}

static void LogHookLikeMethods(Il2CppClass* klass, const char* header) {
    if (!klass) return;
    LogA(header);
    void* iter=0;
    const MethodInfo* m=0;
    int found=0;
    while ((m=g_il2cpp.class_get_methods(klass,&iter))!=0) {
        const char* name=g_il2cpp.method_get_name(m);
        if (!name || !IsHookDiscoveryName(name)) continue;
        ++found;
        LogA("[HOOK METHOD]");
        LogA(name);
        LogIntA("[HOOK DEEP] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
        LogMethodReturnType(m);
    }
    if (!found) LogA("[HOOK DEEP] no Hook/Grap/Rope/Cable/Tether method names found on this class.");
}

static void LogAllMembersForDiscoveryClass(Il2CppClass* klass) {
    if (!klass) return;
    LogClassIdentity(klass,"[HOOK DEEP] matching class");

    void* mi=0;
    const MethodInfo* m=0;
    int methodCount=0;
    while ((m=g_il2cpp.class_get_methods(klass,&mi))!=0 && methodCount<128) {
        const char* mn=g_il2cpp.method_get_name(m);
        if (!mn) continue;
        LogA("[HOOK CLASS METHOD]");
        LogA(mn);
        LogIntA("[HOOK DEEP] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
        LogMethodReturnType(m);
        ++methodCount;
    }

    if (g_il2cpp.class_get_fields && g_il2cpp.field_get_name) {
        void* fi=0;
        const FieldInfo* fld=0;
        int fieldCount=0;
        while ((fld=g_il2cpp.class_get_fields(klass,&fi))!=0 && fieldCount<128) {
            const char* fn=g_il2cpp.field_get_name(fld);
            if (!fn) continue;
            LogA("[HOOK CLASS FIELD]");
            LogA(fn);
            ++fieldCount;
        }
    }
}

static void LogGlobalHookDiscovery() {
    if (!g_assemblyCSharp) return;
    LogA("[HOOK DEEP] ===== Assembly-CSharp Hook/Grap discovery begin =====");

    SIZE_T classCount=g_il2cpp.image_get_class_count(g_assemblyCSharp);
    int matchingClasses=0;
    int matchingMembers=0;

    for (SIZE_T i=0;i<classCount;i++) {
        Il2CppClass* klass=g_il2cpp.image_get_class(g_assemblyCSharp,i);
        if (!klass) continue;

        const char* cn=g_il2cpp.class_get_name(klass);
        const char* ns=g_il2cpp.class_get_namespace(klass);
        BOOL classMatch=IsHookDiscoveryName(cn) || IsHookDiscoveryName(ns);

        if (classMatch && matchingClasses<64) {
            LogAllMembersForDiscoveryClass(klass);
            ++matchingClasses;
            continue;
        }

        BOOL headerLogged=FALSE;
        void* mi=0;
        const MethodInfo* m=0;
        while ((m=g_il2cpp.class_get_methods(klass,&mi))!=0 && matchingMembers<256) {
            const char* mn=g_il2cpp.method_get_name(m);
            if (!mn || !IsHookDiscoveryName(mn)) continue;
            if (!headerLogged) {
                LogClassIdentity(klass,"[HOOK DEEP] class with matching member");
                headerLogged=TRUE;
            }
            LogA("[HOOK MEMBER METHOD]");
            LogA(mn);
            LogIntA("[HOOK DEEP] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
            LogMethodReturnType(m);
            ++matchingMembers;
        }

        if (g_il2cpp.class_get_fields && g_il2cpp.field_get_name && matchingMembers<256) {
            void* fi=0;
            const FieldInfo* fld=0;
            while ((fld=g_il2cpp.class_get_fields(klass,&fi))!=0 && matchingMembers<256) {
                const char* fn=g_il2cpp.field_get_name(fld);
                if (!fn || !IsHookDiscoveryName(fn)) continue;
                if (!headerLogged) {
                    LogClassIdentity(klass,"[HOOK DEEP] class with matching member");
                    headerLogged=TRUE;
                }
                LogA("[HOOK MEMBER FIELD]");
                LogA(fn);
                ++matchingMembers;
            }
        }
    }

    LogIntA("[HOOK DEEP] matching class count: ",matchingClasses);
    LogIntA("[HOOK DEEP] matching member count: ",matchingMembers);
    LogA("[HOOK DEEP] ===== Assembly-CSharp Hook/Grap discovery end =====");
}

static BOOL InitOwnedDlcHook() {
    if (!IsTheseSunnyDazeOwned()) {
        LogA("[HOOK] These Sunny Daze ownership not confirmed; hook init blocked.");
        return FALSE;
    }

    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    if (!inventoryClass || !weaponsInventoryClass) {
        LogA("[ERROR] DLC hook metadata classes unresolved");
        return FALSE;
    }

    LogHookLikeMethods(inventoryClass,"[HOOK AUDIT] PlayerInventoryComponent Hook/Grap methods:");
    LogHookLikeMethods(weaponsInventoryClass,"[HOOK AUDIT] WeaponsInventory Hook/Grap methods:");
    LogAllFieldsForClass(inventoryClass,"[HOOK DEEP] PlayerInventoryComponent fields:");
    LogAllFieldsForClass(weaponsInventoryClass,"[HOOK DEEP] WeaponsInventory fields:");
    LogGlobalHookDiscovery();

    // V2A25: hook probe must be side-effect free with respect to weapon grants.
    // Do NOT call GiveAllWeapons here: user confirmed that doing so only grants
    // the weapon set and does not make the DLC hook usable.
    LogA("[HOOK] Pure hook probe: no GiveAllWeapons call will be made.");

    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const MethodInfo* getHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"get_Hook",0);
    const MethodInfo* initHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"InitHook",0);
    if (getHook) {
        LogA("[HOOK DEEP] WeaponsInventory.get_Hook return metadata:");
        LogMethodReturnType(getHook);
    }
    if (!weaponsControllerField || !getHook || !initHook) {
        LogA("[ERROR] WeaponsInventory get_Hook / InitHook bridge unresolved");
        return FALSE;
    }

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) {
        LogA("[ERROR] PlayerInventoryComponent instance not found for hook init");
        return FALSE;
    }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) {
        LogA("[ERROR] PlayerInventoryComponent._weaponsController is null for hook init");
        return FALSE;
    }

    Il2CppException* ex=0;
    Il2CppObject* hook=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.get_Hook() threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    if (hook) {
        LogA("[HOOK] Hook object already exists; InitHook skipped.");
        return TRUE;
    }

    LogA("[HOOK] Hook object missing; calling native WeaponsInventory.InitHook().");
    ex=0;
    g_il2cpp.runtime_invoke(initHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.InitHook() threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    ex=0;
    hook=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.get_Hook() after InitHook threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    if (hook) {
        LogA("[OK] DLC hook object initialized without Give All Weapons.");
        return TRUE;
    }

    LogA("[WARN] InitHook completed but get_Hook() is still null.");
    return FALSE;
}

static BOOL EquipOwnedDlcWeaponInSlot(int cmdIndex) {
    if (cmdIndex<0 || cmdIndex>=CMD_COUNT) return FALSE;

    // V2A15 safety rule: every DLC-specific class/method is resolved lazily
    // only after the user runs this command. Startup remains the V2A8 path.
    Il2CppClass* dlcCategoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.Base","DLCCategory") : 0;
    Il2CppClass* downloadableContentClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.Base","DownloadableContent") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;

    if (!dlcCategoryClass || !downloadableContentClass || !weaponIdClass || !inventoryClass || !weaponsInventoryClass) {
        LogA("[ERROR] Direct DLC equip metadata classes unresolved");
        return FALSE;
    }

    const MethodInfo* isPurchased=FindMethodExact(downloadableContentClass,"IsDlcPurchased",1,dlcCategoryClass);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    const MethodInfo* equipWeapon=FindMethodExact(weaponsInventoryClass,"EquipWeapon",1,weaponIdClass);
    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");

    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    const MethodInfo* findAll=resourcesClass ? FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass) : 0;

    if (!isPurchased || !getCategory || !getSlot || !addWeapon || !equipWeapon || !weaponsControllerField || !findAll) {
        LogA("[ERROR] Direct DLC equip native bridge unresolved");
        return FALSE;
    }

    int dlc=1; // DLCCategory.PTSD / These Sunny Daze
    void* ownPars[1]; ownPars[0]=&dlc;
    Il2CppException* ex=0;
    Il2CppObject* ownBox=g_il2cpp.runtime_invoke(isPurchased,0,ownPars,&ex);
    if (ex) { LogA("[ERROR] IsDlcPurchased(PTSD) threw:"); FormatExceptionToLog(ex); return FALSE; }
    if (!ownBox || !g_il2cpp.object_unbox(ownBox)) { LogA("[ERROR] IsDlcPurchased(PTSD) returned no bool"); return FALSE; }
    if ((*(BYTE*)g_il2cpp.object_unbox(ownBox))==0) {
        LogA("[DLC] These Sunny Daze ownership: NO");
        LogA("[DLC] Direct DLC equip blocked. Ownership check remains enforced.");
        return FALSE;
    }
    LogA("[DLC] These Sunny Daze ownership: YES");

    int wantedSlot=ParseFirstIntW(g_cmds[cmdIndex].args,1);
    if (wantedSlot<0 || wantedSlot>12) {
        LogA("[ERROR] Equip DLC Weapon slot must be between 0 and 12");
        return FALSE;
    }
    LogIntA("[DLC] Requested native WeaponId slot: ",wantedSlot);

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) { LogA("[ERROR] PlayerInventoryComponent instance not found"); return FALSE; }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) { LogA("[ERROR] PlayerInventoryComponent._weaponsController is null"); return FALSE; }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) { LogA("[ERROR] WeaponId System.Type unavailable"); return FALSE; }

    void* findPars[1]; findPars[0]=typeObj;
    ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) { LogA("[ERROR] Resources.FindObjectsOfTypeAll(WeaponId) threw:"); FormatExceptionToLog(ex); return FALSE; }
    if (!arrObj) { LogA("[ERROR] Resources.FindObjectsOfTypeAll(WeaponId) returned null"); return FALSE; }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) { LogA("[ERROR] WeaponId scan exceeded defensive limit"); return FALSE; }
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    Il2CppObject* selected=0;
    unsigned int ptsdCount=0, matchingCount=0;
    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox || !g_il2cpp.object_unbox(catBox)) continue;
        int category=*(int*)g_il2cpp.object_unbox(catBox);
        if (category!=1) continue;
        ++ptsdCount;

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if (ex || !slotBox || !g_il2cpp.object_unbox(slotBox)) continue;
        int slot=*(int*)g_il2cpp.object_unbox(slotBox);

        LogIntA("[DLC] Loaded PTSD WeaponId slot: ",slot);
        if (slot==wantedSlot) {
            ++matchingCount;
            if (!selected) selected=weapon;
        }
    }

    LogIntA("[DLC] Loaded PTSD WeaponIds found: ",(int)ptsdCount);
    LogIntA("[DLC] PTSD WeaponIds matching requested slot: ",(int)matchingCount);
    if (!selected) {
        LogA("[DLC] No loaded PTSD WeaponId uses the requested slot.");
        return FALSE;
    }

    return SafeAddAndEquipWeaponId(playerInv,weaponsInv,inventoryClass,weaponsInventoryClass,weaponIdClass,selected,
                                   "[DLC] Native AddWeapon(selected PTSD WeaponId) completed");
}


static BOOL EquipBaseWeaponInSlot(int cmdIndex) {
    if (cmdIndex<0 || cmdIndex>=CMD_COUNT) return FALSE;

    // V2A16 deliberately mirrors the validated V2A15 direct-equip path.
    // The only selection difference is DLCCategory.NONE (base game).
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;

    if (!weaponIdClass || !inventoryClass || !weaponsInventoryClass) {
        LogA("[ERROR] Direct base weapon equip metadata classes unresolved");
        return FALSE;
    }

    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    const MethodInfo* equipWeapon=FindMethodExact(weaponsInventoryClass,"EquipWeapon",1,weaponIdClass);
    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");

    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    const MethodInfo* findAll=resourcesClass ? FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass) : 0;

    if (!getCategory || !getSlot || !addWeapon || !equipWeapon || !weaponsControllerField || !findAll) {
        LogA("[ERROR] Direct base weapon equip native bridge unresolved");
        return FALSE;
    }

    int wantedSlot=ParseFirstIntW(g_cmds[cmdIndex].args,1);
    if (wantedSlot<0 || wantedSlot>12) {
        LogA("[ERROR] Equip Base Weapon slot must be between 0 and 12");
        return FALSE;
    }
    LogIntA("[BASE] Requested native WeaponId slot: ",wantedSlot);

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) { LogA("[ERROR] PlayerInventoryComponent instance not found"); return FALSE; }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) { LogA("[ERROR] PlayerInventoryComponent._weaponsController is null"); return FALSE; }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) { LogA("[ERROR] WeaponId System.Type unavailable"); return FALSE; }

    void* findPars[1]; findPars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) { LogA("[ERROR] Resources.FindObjectsOfTypeAll(WeaponId) threw:"); FormatExceptionToLog(ex); return FALSE; }
    if (!arrObj) { LogA("[ERROR] Resources.FindObjectsOfTypeAll(WeaponId) returned null"); return FALSE; }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) { LogA("[ERROR] WeaponId scan exceeded defensive limit"); return FALSE; }
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    Il2CppObject* selected=0;
    unsigned int baseCount=0, matchingCount=0;
    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox || !g_il2cpp.object_unbox(catBox)) continue;
        int category=*(int*)g_il2cpp.object_unbox(catBox);
        if (category!=0) continue; // DLCCategory.NONE = base game
        ++baseCount;

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if (ex || !slotBox || !g_il2cpp.object_unbox(slotBox)) continue;
        int slot=*(int*)g_il2cpp.object_unbox(slotBox);

        LogIntA("[BASE] Loaded base WeaponId slot: ",slot);
        if (slot==wantedSlot) {
            ++matchingCount;
            if (!selected) selected=weapon;
        }
    }

    LogIntA("[BASE] Loaded base WeaponIds found: ",(int)baseCount);
    LogIntA("[BASE] Base WeaponIds matching requested slot: ",(int)matchingCount);
    if (!selected) {
        LogA("[BASE] No loaded base WeaponId uses the requested slot.");
        return FALSE;
    }

    return SafeAddAndEquipWeaponId(playerInv,weaponsInv,inventoryClass,weaponsInventoryClass,weaponIdClass,selected,
                                   "[BASE] Native AddWeapon(selected base WeaponId) completed");
}


static BOOL IsTheseSunnyDazeOwned() {
    Il2CppClass* dlcCategoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.Base","DLCCategory") : 0;
    Il2CppClass* downloadableContentClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.Base","DownloadableContent") : 0;
    if (!dlcCategoryClass || !downloadableContentClass) return FALSE;

    const MethodInfo* isPurchased=FindMethodExact(downloadableContentClass,"IsDlcPurchased",1,dlcCategoryClass);
    if (!isPurchased) return FALSE;

    int dlc=1;
    void* pars[1]; pars[0]=&dlc;
    Il2CppException* ex=0;
    Il2CppObject* boxed=g_il2cpp.runtime_invoke(isPurchased,0,pars,&ex);
    if (ex) { LogA("[ERROR] IsDlcPurchased(PTSD) threw:"); FormatExceptionToLog(ex); return FALSE; }
    if (!boxed) return FALSE;
    void* raw=g_il2cpp.object_unbox(boxed);
    if (!raw) return FALSE;
    return (*(BYTE*)raw)!=0;
}

static BOOL GiveAllOwnedDlcWeapons() {
    if (!IsTheseSunnyDazeOwned()) {
        LogA("[DLC] Give All DLC Weapons blocked: These Sunny Daze ownership not confirmed.");
        return FALSE;
    }

    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;

    if (!weaponIdClass || !inventoryClass || !weaponsInventoryClass || !resourcesClass) {
        LogA("[ERROR] Give All DLC Weapons metadata classes unresolved");
        return FALSE;
    }

    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);

    if (!getCategory || !addWeapon || !weaponsControllerField || !findAll) {
        LogA("[ERROR] Give All DLC Weapons native bridge unresolved");
        return FALSE;
    }

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) { LogA("[ERROR] PlayerInventoryComponent instance not found"); return FALSE; }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) { LogA("[ERROR] PlayerInventoryComponent._weaponsController is null"); return FALSE; }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) return FALSE;

    void* findPars[1]; findPars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) { LogA("[ERROR] FindObjectsOfTypeAll(WeaponId) threw:"); FormatExceptionToLog(ex); return FALSE; }
    if (!arrObj) return FALSE;

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) return FALSE;
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    unsigned int found=0, added=0, already=0, errors=0;
    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;
        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox) { ++errors; continue; }
        void* catRaw=g_il2cpp.object_unbox(catBox);
        if (!catRaw || *(int*)catRaw!=1) continue;
        ++found;

        int collected=IsWeaponCollected(playerInv,inventoryClass,weapon);
        if (collected==1) { ++already; continue; }

        void* wp[1]; wp[0]=weapon;
        ex=0;
        g_il2cpp.runtime_invoke(addWeapon,weaponsInv,wp,&ex);
        if (ex) { ++errors; continue; }
        ++added;
    }

    LogIntA("[DLC] Give All DLC Weapons found PTSD WeaponIds: ",(int)found);
    LogIntA("[DLC] Give All DLC Weapons already collected / skipped: ",(int)already);
    LogIntA("[DLC] Give All DLC Weapons AddWeapon completed: ",(int)added);
    if (errors) LogIntA("[DLC] Give All DLC Weapons errors: ",(int)errors);
    if (added || already) LogA("[OK] Give All DLC Weapons completed without duplicate AddWeapon on known collected weapons.");
    return (added+already)>0;
}

static BOOL EquipMappedWeaponCategorySlot(int category, int wantedSlot) {
    if (wantedSlot<1 || wantedSlot>9) return FALSE;
    if (category==1 && !IsTheseSunnyDazeOwned()) return FALSE;

    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    if (!weaponIdClass || !inventoryClass || !weaponsInventoryClass || !resourcesClass) return FALSE;

    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    const MethodInfo* equipWeapon=FindMethodExact(weaponsInventoryClass,"EquipWeapon",1,weaponIdClass);
    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);
    if (!getCategory || !getSlot || !addWeapon || !equipWeapon || !weaponsControllerField || !findAll) return FALSE;

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) return FALSE;
    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) return FALSE;

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) return FALSE;

    void* fp[1]; fp[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,fp,&ex);
    if (ex || !arrObj) return FALSE;

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) return FALSE;
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    Il2CppObject* selected=0;
    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox) continue;
        void* catRaw=g_il2cpp.object_unbox(catBox);
        if (!catRaw || *(int*)catRaw!=category) continue;

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if (ex || !slotBox) continue;
        void* slotRaw=g_il2cpp.object_unbox(slotBox);
        if (!slotRaw || *(int*)slotRaw!=wantedSlot) continue;

        selected=weapon;
        break;
    }

    if (!selected) {
        if (category==1) LogIntA("[KEYMAP] No PTSD WeaponId found in slot ",wantedSlot);
        else LogIntA("[KEYMAP] No base WeaponId found in slot ",wantedSlot);
        return FALSE;
    }

    BOOL ok=SafeAddAndEquipWeaponId(playerInv,weaponsInv,inventoryClass,weaponsInventoryClass,weaponIdClass,selected,
                                        category==1 ? "[KEYMAP] AddWeapon(DLC) completed" : "[KEYMAP] AddWeapon(BASE) completed");
    if (!ok) return FALSE;

    if (category==1) LogIntA("[KEYMAP] Equipped DLC weapon in slot ",wantedSlot);
    else LogIntA("[KEYMAP] Equipped base weapon in slot ",wantedSlot);
    return TRUE;
}

static BOOL DecodeWeaponListCommand(int cmdIndex, int* category, int* slot) {
    if (cmdIndex<0 || cmdIndex>=CMD_COUNT || !category || !slot) return FALSE;
    const char* name=g_cmds[cmdIndex].className;
    const char* basePrefix="__WeaponListBase";
    const char* dlcPrefix="__WeaponListDlc";
    const char* suffix=0;

    if (StrStartsWithA(name,basePrefix)) {
        *category=0;
        suffix=name+StrLenA(basePrefix);
    } else if (StrStartsWithA(name,dlcPrefix)) {
        *category=1;
        suffix=name+StrLenA(dlcPrefix);
    } else {
        return FALSE;
    }

    // V2A25: the slot is the visible Argument value, not the row-name suffix.
    int value=0;
    int p=0;
    while (g_cmds[cmdIndex].args[p]==L' ' || g_cmds[cmdIndex].args[p]==L'\t') ++p;
    while (g_cmds[cmdIndex].args[p]>=L'0' && g_cmds[cmdIndex].args[p]<=L'9') {
        value=value*10+(int)(g_cmds[cmdIndex].args[p]-L'0');
        ++p;
        if (value>99) break;
    }

    if (value>=1 && value<=9) {
        *slot=value;
        return TRUE;
    }

    // Keep the row in WEAPON LIST even if an old/bad INI contains an invalid
    // argument. Fall back to the row's original 1..9 identity.
    if (suffix && suffix[0]>='1' && suffix[0]<='9' && suffix[1]==0) {
        *slot=(int)(suffix[0]-'0');
        return TRUE;
    }

    return FALSE;
}

static void ExecuteWeaponKeyMapping(int slot, int mode) {
    if (!g_ready || g_shuttingDown || slot<1 || slot>9) return;

    BOOL attached=FALSE; void* thread=0;
    if (!EnsureIl2CppThread(&attached,&thread)) return;

    if (mode==1) {
        EquipMappedWeaponCategorySlot(0,slot);
    } else if (mode==2) {
        EquipMappedWeaponCategorySlot(1,slot);
    } else if (mode==3) {
        // ALT starts with DLC because the game's own number-key handler has
        // already selected its base weapon on key-down. On the next press of
        // that same slot, switch back to base, then DLC, etc.
        int category=(g_altSlotFlip[slot] & 1) ? 0 : 1;
        g_altSlotFlip[slot]^=1;
        if (!EquipMappedWeaponCategorySlot(category,slot))
            EquipMappedWeaponCategorySlot(category==1 ? 0 : 1,slot);
    }

    ReleaseIl2CppThread(attached,thread);
}


static BOOL GetBaseWheelButtonsCollection(Il2CppObject* collection, Il2CppClass** outClass, int* outCount, BOOL* outArray) {
    if (!collection || !outClass || !outCount || !outArray) return FALSE;
    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    if (!objectGetClass) return FALSE;

    Il2CppClass* klass=objectGetClass(collection);
    if (!klass) return FALSE;
    const char* name=g_il2cpp.class_get_name(klass);
    if (!name) return FALSE;

    int n=StrLenA(name);
    BOOL isArray=(n>=2 && name[n-2]=='[' && name[n-1]==']');
    int count=0;
    if (isArray) {
        SIZE_T len=g_il2cpp.array_length((Il2CppArray*)collection);
        if (len>64) return FALSE;
        count=(int)len;
    } else {
        const MethodInfo* getCount=g_il2cpp.class_get_method_from_name(klass,"get_Count",0);
        if (!getCount) return FALSE;
        Il2CppException* ex=0;
        Il2CppObject* boxed=g_il2cpp.runtime_invoke(getCount,collection,0,&ex);
        if (ex || !boxed) return FALSE;
        void* raw=g_il2cpp.object_unbox(boxed);
        if (!raw) return FALSE;
        count=*(int*)raw;
        if (count<0 || count>64) return FALSE;
    }

    *outClass=klass;
    *outCount=count;
    *outArray=isArray;
    return TRUE;
}

static Il2CppObject* GetBaseWheelButtonFromCollection(Il2CppObject* collection, Il2CppClass* collectionClass, BOOL isArray, int index) {
    if (!collection || !collectionClass || index<0) return 0;
    if (isArray) {
        Il2CppArray* arr=(Il2CppArray*)collection;
        SIZE_T len=g_il2cpp.array_length(arr);
        if ((SIZE_T)index>=len) return 0;
        unsigned int hdr=g_il2cpp.array_object_header_size();
        Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
        return elems[index];
    }

    const MethodInfo* getItem=g_il2cpp.class_get_method_from_name(collectionClass,"get_Item",1);
    if (!getItem) return 0;
    void* pars[1]; pars[0]=&index;
    Il2CppException* ex=0;
    Il2CppObject* item=g_il2cpp.runtime_invoke(getItem,collection,pars,&ex);
    if (ex) return 0;
    return item;
}

static BOOL ShowNativeBaseWeaponWheel() {
    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelButtonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if (!wheelClass || !wheelButtonClass || !wheelViewClass || !weaponIdClass) {
        LogA("[ERROR] Base weapon wheel metadata classes unresolved");
        return FALSE;
    }

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* buttonGetWeapon=g_il2cpp.class_get_method_from_name(wheelButtonClass,"get_WeaponId",0);
    const MethodInfo* buttonEnable=g_il2cpp.class_get_method_from_name(wheelButtonClass,"Enable",1);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* showMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Show",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    if (!buttonsField || !buttonGetWeapon || !buttonEnable || !getCategory || !showMethod || !restoreMethod) {
        LogA("[ERROR] Base weapon wheel native bridge unresolved");
        return FALSE;
    }

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);
    if (!wheel || !view) {
        LogA("[ERROR] Live base weapon wheel/view not found");
        return FALSE;
    }

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if (!collection) {
        LogA("[ERROR] PlayerWeaponWheelComponent._weaponWheelButtons is null");
        return FALSE;
    }

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if (!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) {
        LogA("[ERROR] Could not inspect native base wheel button collection");
        return FALSE;
    }

    int baseCount=0, otherCount=0;
    for (int i=0;i<count;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if (!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(buttonGetWeapon,button,0,&ex);
        if (ex || !weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox) continue;
        void* raw=g_il2cpp.object_unbox(catBox);
        if (!raw) continue;
        int cat=*(int*)raw;

        BOOL enabled=(cat==0) ? TRUE : FALSE;
        if (enabled) ++baseCount; else ++otherCount;

        void* enablePars[1]; enablePars[0]=&enabled;
        ex=0;
        g_il2cpp.runtime_invoke(buttonEnable,button,enablePars,&ex);
        if (ex) continue;
    }

    LogIntA("[WHEEL] Native BASE buttons enabled: ",baseCount);
    LogIntA("[WHEEL] Non-base buttons temporarily disabled: ",otherCount);

    if (!baseCount) {
        Il2CppException* ex=0;
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        LogA("[WHEEL] No base buttons available in native wheel");
        return FALSE;
    }

    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(showMethod,view,0,&ex);
    if (ex) {
        LogA("[ERROR] PlayerWheelView.Show() threw:");
        FormatExceptionToLog(ex);
        ex=0;
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        return FALSE;
    }

    g_baseWheelActive=1;
    LogA("[WHEEL] BASE native weapon wheel opened");
    return TRUE;
}

static BOOL HideNativeBaseWeaponWheel() {
    if (!g_baseWheelActive) return TRUE;

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    if (!wheelClass || !wheelViewClass) {
        g_baseWheelActive=0;
        return FALSE;
    }

    const MethodInfo* hideMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Hide",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);

    BOOL ok=TRUE;
    Il2CppException* ex=0;
    if (view && hideMethod) {
        g_il2cpp.runtime_invoke(hideMethod,view,0,&ex);
        if (ex) { FormatExceptionToLog(ex); ok=FALSE; }
    } else ok=FALSE;

    ex=0;
    if (wheel && restoreMethod) {
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        if (ex) { FormatExceptionToLog(ex); ok=FALSE; }
    } else ok=FALSE;

    g_baseWheelActive=0;
    LogA("[WHEEL] BASE native weapon wheel closed; native buttons restored");
    return ok;
}

static void ExecuteNativeBaseWheel(BOOL show) {
    if (!g_ready || g_shuttingDown) return;
    BOOL attached=FALSE; void* thread=0;
    if (!EnsureIl2CppThread(&attached,&thread)) return;

    if (show) ShowNativeBaseWeaponWheel();
    else HideNativeBaseWeaponWheel();

    ReleaseIl2CppThread(attached,thread);
}


static int FilterNativeWheelForCategory(Il2CppObject* wheel,
                                        const FieldInfo* buttonsField,
                                        const MethodInfo* buttonGetWeapon,
                                        const MethodInfo* buttonEnable,
                                        const MethodInfo* getCategory,
                                        int wantedCategory,
                                        const char* phase) {
    if (!wheel || !buttonsField || !buttonGetWeapon || !buttonEnable || !getCategory) return -1;

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if (!collection) {
        LogA("[ERROR] PlayerWeaponWheelComponent._weaponWheelButtons is null");
        return -1;
    }

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if (!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) {
        LogA("[ERROR] Could not inspect native wheel button collection");
        return -1;
    }

    int baseCount=0, dlcCount=0, otherCount=0, enabledCount=0;
    for (int i=0;i<count;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if (!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(buttonGetWeapon,button,0,&ex);
        if (ex || !weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox) continue;
        void* raw=g_il2cpp.object_unbox(catBox);
        if (!raw) continue;
        int cat=*(int*)raw;

        if (cat==0) ++baseCount;
        else if (cat==1) ++dlcCount;
        else ++otherCount;

        BOOL enabled=(cat==wantedCategory) ? TRUE : FALSE;
        void* ep[1]; ep[0]=&enabled;
        ex=0;
        g_il2cpp.runtime_invoke(buttonEnable,button,ep,&ex);
        if (!ex && enabled) ++enabledCount;
    }

    if (phase) LogA(phase);
    LogIntA("[WHEEL] Native wheel buttons total: ",count);
    LogIntA("[WHEEL] Native BASE buttons seen: ",baseCount);
    LogIntA("[WHEEL] Native DLC/PTSD buttons seen: ",dlcCount);
    if (otherCount) LogIntA("[WHEEL] Native other-category buttons seen: ",otherCount);
    LogIntA("[WHEEL] Buttons enabled for requested bank: ",enabledCount);
    return enabledCount;
}

static BOOL ShowNativeDlcWeaponWheel() {
    if (!IsTheseSunnyDazeOwned()) {
        LogA("[WHEEL] DLC wheel blocked: These Sunny Daze ownership not confirmed.");
        return FALSE;
    }

    // V2A29 deliberately follows the V2A20 Base Weapon Wheel architecture:
    // use the already-live native PlayerWeaponWheelComponent / PlayerWheelView,
    // filter what can be filtered, but still show the wheel even when the DLC
    // category currently has zero live buttons.
    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelButtonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if (!wheelClass || !wheelButtonClass || !wheelViewClass || !weaponIdClass) {
        LogA("[ERROR] DLC weapon wheel metadata classes unresolved");
        return FALSE;
    }

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* buttonGetWeapon=g_il2cpp.class_get_method_from_name(wheelButtonClass,"get_WeaponId",0);
    const MethodInfo* buttonEnable=g_il2cpp.class_get_method_from_name(wheelButtonClass,"Enable",1);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* showMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Show",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    if (!buttonsField || !buttonGetWeapon || !buttonEnable || !getCategory || !showMethod || !restoreMethod) {
        LogA("[ERROR] DLC weapon wheel native bridge unresolved");
        return FALSE;
    }

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);
    if (!wheel || !view) {
        LogA("[ERROR] Live DLC weapon wheel/view not found");
        return FALSE;
    }

    // Begin from the game's own button state exactly as the V2A20 wheel does.
    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
    if (ex) {
        LogA("[WARN] EnableButtons() pre-refresh threw before DLC wheel scan:");
        FormatExceptionToLog(ex);
    }

    int enabled=FilterNativeWheelForCategory(
        wheel,buttonsField,buttonGetWeapon,buttonEnable,getCategory,1,
        "[WHEEL] V2A20-style DLC wheel scan");

    // Important difference from V2A22/V2A27:
    // zero DLC buttons is NOT an error. Show PlayerWheelView anyway so the
    // wheel shell remains visible, matching the useful V2A20 behavior.
    if (enabled<=0)
        LogA("[WHEEL] No live DLC/PTSD buttons found; showing empty native wheel shell (V2A20-style behavior).");

    ex=0;
    g_il2cpp.runtime_invoke(showMethod,view,0,&ex);
    if (ex) {
        LogA("[ERROR] PlayerWheelView.Show() threw for DLC wheel:");
        FormatExceptionToLog(ex);
        ex=0;
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        return FALSE;
    }

    g_dlcWheelActive=1;
    LogA(enabled>0 ? "[WHEEL] DLC native weapon wheel opened"
                   : "[WHEEL] EMPTY DLC native weapon wheel opened");
    return TRUE;
}

static BOOL HideNativeDlcWeaponWheel() {
    if (!g_dlcWheelActive) return TRUE;

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    if (!wheelClass || !wheelViewClass) {
        g_dlcWheelActive=0;
        return FALSE;
    }

    const MethodInfo* hideMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Hide",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);

    BOOL ok=TRUE;
    Il2CppException* ex=0;
    if (view && hideMethod) {
        g_il2cpp.runtime_invoke(hideMethod,view,0,&ex);
        if (ex) { FormatExceptionToLog(ex); ok=FALSE; }
    } else ok=FALSE;

    ex=0;
    if (wheel && restoreMethod) {
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        if (ex) { FormatExceptionToLog(ex); ok=FALSE; }
    } else ok=FALSE;

    g_dlcWheelActive=0;
    LogA("[WHEEL] DLC native weapon wheel closed; native buttons restored");
    return ok;
}

static void ExecuteNativeDlcWheel(BOOL show) {
    if (!g_ready || g_shuttingDown) return;
    BOOL attached=FALSE; void* thread=0;
    if (!EnsureIl2CppThread(&attached,&thread)) return;

    if (show) {
        if (g_baseWheelActive) HideNativeBaseWeaponWheel();
        ShowNativeDlcWeaponWheel();
    } else {
        HideNativeDlcWeaponWheel();
    }

    ReleaseIl2CppThread(attached,thread);
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

    int listCategory=0, listSlot=0;
    if (DecodeWeaponListCommand(i,&listCategory,&listSlot)) EquipMappedWeaponCategorySlot(listCategory,listSlot);
    else if (StrEqA(g_cmds[i].className,"__NoCrosshair")) ToggleNoCrosshair();
    else if (StrEqA(g_cmds[i].className,"__GiveAllWeapons")) GiveAllWeaponsOnly();
    else if (StrEqA(g_cmds[i].className,"__GiveAllDlcWeapons")) GiveAllOwnedDlcWeapons();
    else if (StrEqA(g_cmds[i].className,"__InitDlcHook")) InitOwnedDlcHook();
    else if (StrEqA(g_cmds[i].className,"__WeaponKeysMode")) LogA("[KEYMAP] Weapon Keys Mode changed through its Argument field");
    else if (StrEqA(g_cmds[i].className,"__BaseWeaponWheel")) LogA("[WHEEL] Bind Base Weapon Wheel to a key, then HOLD that key to open it.");
    else if (StrEqA(g_cmds[i].className,"__DlcWeaponWheel")) LogA("[WHEEL] Bind DLC Weapon Wheel to a key, then HOLD it. V2A20-style wheel shell opens even when no DLC button is live.");
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

