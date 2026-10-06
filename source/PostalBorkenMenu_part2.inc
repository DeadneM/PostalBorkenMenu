
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

__declspec(noinline) static void FormatExceptionToLog(Il2CppException* ex) {
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
static const MethodInfo* g_findObjectsOfTypeAll1 = 0;
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

static void LogCurrentHookState(const char* phase);

static BOOL GiveAllWeaponsOnly() {
    if (!g_inventoryClass || !g_giveAllWeapons) { LogA("[ERROR] Give All Weapons bridge unresolved"); return FALSE; }
    Il2CppObject* inv=FindLiveObject(g_inventoryClass);
    if (!inv) { LogA("[ERROR] PlayerInventoryComponent instance not found"); return FALSE; }

    // V2A31: observation only. Snapshot the known hook state immediately
    // before and after the validated native GiveAllWeapons() call.
    LogCurrentHookState("[GIVEALL HOOK] BEFORE GiveAllWeapons");

    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(g_giveAllWeapons,inv,0,&ex);
    if (ex) { LogA("[ERROR] GiveAllWeapons() threw:"); FormatExceptionToLog(ex); return FALSE; }
    LogA("[OK] Give All Weapons executed (quest items untouched)");

    LogCurrentHookState("[GIVEALL HOOK] AFTER GiveAllWeapons");
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

static void LogCurrentHookState(const char* phase) {
    if (phase) LogA(phase);

    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if (!inventoryClass || !weaponsInventoryClass || !weaponIdClass) {
        LogA("[GIVEALL HOOK] metadata unresolved");
        return;
    }

    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const FieldInfo* hookWeaponField=g_il2cpp.class_get_field_from_name(weaponsInventoryClass,"_hookWeapon");
    const MethodInfo* getHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"get_Hook",0);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    if (!weaponsControllerField || !hookWeaponField || !getHook || !getCategory || !getSlot) {
        LogA("[GIVEALL HOOK] targeted bridge unresolved");
        return;
    }

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) {
        LogA("[GIVEALL HOOK] PlayerInventoryComponent not found");
        return;
    }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) {
        LogA("[GIVEALL HOOK] _weaponsController is null");
        return;
    }

    Il2CppObject* fieldValue=0;
    g_il2cpp.field_get_value(weaponsInv,hookWeaponField,&fieldValue);

    Il2CppException* ex=0;
    Il2CppObject* getterValue=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[GIVEALL HOOK] get_Hook() threw:");
        FormatExceptionToLog(ex);
        getterValue=0;
    }

    LogA(fieldValue ? "[GIVEALL HOOK] _hookWeapon: NON-NULL" : "[GIVEALL HOOK] _hookWeapon: NULL");
    LogA(getterValue ? "[GIVEALL HOOK] get_Hook(): NON-NULL" : "[GIVEALL HOOK] get_Hook(): NULL");

    if (fieldValue && getterValue)
        LogA(fieldValue==getterValue ? "[GIVEALL HOOK] _hookWeapon == get_Hook()"
                                    : "[GIVEALL HOOK] _hookWeapon != get_Hook()");

    Il2CppObject* hook=getterValue ? getterValue : fieldValue;
    if (!hook) return;

    ex=0;
    Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,hook,0,&ex);
    if (!ex && catBox && g_il2cpp.object_unbox(catBox))
        LogIntA("[GIVEALL HOOK] WeaponId category: ",*(int*)g_il2cpp.object_unbox(catBox));

    ex=0;
    Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,hook,0,&ex);
    if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
        LogIntA("[GIVEALL HOOK] WeaponId slot: ",*(int*)g_il2cpp.object_unbox(slotBox));
}

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

static void CatalogAppendA(char* dst, int cap, const char* src) {
    if (!dst || cap<=0) return;
    int d=StrLenA(dst);
    if (d>=cap-1 || !src) return;
    int i=0;
    while (src[i] && d+i<cap-1) {
        dst[d+i]=src[i];
        ++i;
    }
    dst[d+i]=0;
}

static void CatalogAppendInt(char* dst, int cap, int v) {
    char tmp[32]; int p=0;
    unsigned int u;
    if (v<0) { tmp[p++]='-'; u=(unsigned int)(-v); }
    else u=(unsigned int)v;
    char rev[16]; int n=0;
    do { rev[n++]=(char)('0'+(u%10)); u/=10; } while (u && n<15);
    while (n>0 && p<(int)sizeof(tmp)-1) tmp[p++]=rev[--n];
    tmp[p]=0;
    CatalogAppendA(dst,cap,tmp);
}

static void CatalogManagedStringToAscii(Il2CppString* str, char* out, int cap) {
    if (!out || cap<=0) return;
    out[0]=0;
    if (!str || !g_il2cpp.string_chars || !g_il2cpp.string_length) return;
    const WCHAR* chars=g_il2cpp.string_chars(str);
    int len=g_il2cpp.string_length(str);
    if (!chars || len<=0) return;
    int n=0;
    for (int i=0;i<len && n<cap-1;i++) {
        WCHAR w=chars[i];
        if (w>=32 && w<=126) out[n++]=(char)w;
        else if (w==L'\t') out[n++]=' ';
        else out[n++]='?';
    }
    out[n]=0;
}

static BOOL RunWeaponCatalog() {
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object") : 0;
    if (!weaponIdClass || !resourcesClass || !unityObjectClass) {
        LogA("[WEAPON CATALOG] Required metadata classes unresolved.");
        return FALSE;
    }

    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    const MethodInfo* toString=g_il2cpp.class_get_method_from_name(weaponIdClass,"ToString",0);
    const MethodInfo* getName=g_il2cpp.class_get_method_from_name(unityObjectClass,"get_name",0);
    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);
    if (!getCategory || !getSlot || !getName || !findAll) {
        LogA("[WEAPON CATALOG] WeaponId/Object/Resources bridge unresolved.");
        return FALSE;
    }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) {
        LogA("[WEAPON CATALOG] WeaponId System.Type unavailable.");
        return FALSE;
    }

    void* findPars[1]; findPars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) {
        LogA("[WEAPON CATALOG] Resources.FindObjectsOfTypeAll(WeaponId) threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }
    if (!arrObj) {
        LogA("[WEAPON CATALOG] WeaponId scan returned null.");
        return FALSE;
    }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) {
        LogA("[WEAPON CATALOG] WeaponId scan exceeded defensive limit.");
        return FALSE;
    }

    Il2CppObject* playerInv=inventoryClass ? FindLiveObject(inventoryClass) : 0;
    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    int baseCount=0, dlcCount=0, otherCount=0, namedCount=0;
    LogA("[WEAPON CATALOG] ===== BEGIN =====");
    LogIntA("[WEAPON CATALOG] Loaded WeaponId objects: ",(int)n);

    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        int category=-999, slot=-999;
        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (!ex && catBox && g_il2cpp.object_unbox(catBox))
            category=*(int*)g_il2cpp.object_unbox(catBox);

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
            slot=*(int*)g_il2cpp.object_unbox(slotBox);

        char unityName[256]; unityName[0]=0;
        ex=0;
        Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,weapon,0,&ex);
        if (!ex && nameObj)
            CatalogManagedStringToAscii((Il2CppString*)nameObj,unityName,256);

        char toStringText[256]; toStringText[0]=0;
        if (toString) {
            ex=0;
            Il2CppObject* strObj=g_il2cpp.runtime_invoke(toString,weapon,0,&ex);
            if (!ex && strObj)
                CatalogManagedStringToAscii((Il2CppString*)strObj,toStringText,256);
        }

        int collected=playerInv ? IsWeaponCollected(playerInv,inventoryClass,weapon) : -1;

        if (category==0) ++baseCount;
        else if (category==1) ++dlcCount;
        else ++otherCount;
        if (unityName[0]) ++namedCount;

        char line[1024]; line[0]=0;
        CatalogAppendA(line,1024,"[WEAPON CATALOG] #");
        CatalogAppendInt(line,1024,(int)i);
        CatalogAppendA(line,1024," | name=");
        CatalogAppendA(line,1024,unityName[0] ? unityName : "<empty>");
        CatalogAppendA(line,1024," | toString=");
        CatalogAppendA(line,1024,toStringText[0] ? toStringText : "<empty>");
        CatalogAppendA(line,1024," | category=");
        CatalogAppendInt(line,1024,category);
        CatalogAppendA(line,1024,category==0 ? "(BASE)" : category==1 ? "(PTSD)" : "(OTHER)");
        CatalogAppendA(line,1024," | slot=");
        CatalogAppendInt(line,1024,slot);
        CatalogAppendA(line,1024," | collected=");
        CatalogAppendA(line,1024,collected==1 ? "YES" : collected==0 ? "NO" : "UNKNOWN");
        LogA(line);
    }

    LogIntA("[WEAPON CATALOG] Base count: ",baseCount);
    LogIntA("[WEAPON CATALOG] PTSD/DLC count: ",dlcCount);
    LogIntA("[WEAPON CATALOG] Other category count: ",otherCount);
    LogIntA("[WEAPON CATALOG] Non-empty Unity names: ",namedCount);
    LogA("[WEAPON CATALOG] ===== END =====");
    return TRUE;
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

static BOOL DecodeNamedWeaponCommand(int cmdIndex, const char** exactName, BOOL* special) {
    if (cmdIndex<0 || cmdIndex>=CMD_COUNT || !exactName || !special) return FALSE;
    const char* n=g_cmds[cmdIndex].className;
    const char* arsenalPrefix="__Arsenal_";
    const char* specialPrefix="__Special_";

    if (StrStartsWithA(n,arsenalPrefix)) {
        *exactName=n+StrLenA(arsenalPrefix);
        *special=FALSE;
    } else if (StrStartsWithA(n,specialPrefix)) {
        *exactName=n+StrLenA(specialPrefix);
        *special=TRUE;
    } else {
        return FALSE;
    }

    return *exactName && StrStartsWithA(*exactName,"WEAPON_");
}

static BOOL ExecuteNamedWeaponByUnityName(const char* exactName, BOOL special) {
    if (!exactName || !exactName[0]) return FALSE;

    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object") : 0;
    if (!weaponIdClass || !inventoryClass || !weaponsInventoryClass || !resourcesClass || !unityObjectClass) {
        LogA("[NAMED WEAPON] Required metadata classes unresolved.");
        return FALSE;
    }

    if (!g_il2cpp.string_chars || !g_il2cpp.string_length) {
        LogA("[NAMED WEAPON] IL2CPP string helpers unavailable; exact-name lookup cannot run.");
        return FALSE;
    }

    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    const MethodInfo* getName=g_il2cpp.class_get_method_from_name(unityObjectClass,"get_name",0);
    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);
    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    if (!getCategory || !getSlot || !getName || !findAll || !weaponsControllerField) {
        LogA("[NAMED WEAPON] Exact-name bridge unresolved.");
        return FALSE;
    }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) {
        LogA("[NAMED WEAPON] WeaponId System.Type unavailable.");
        return FALSE;
    }

    void* findPars[1]; findPars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) {
        LogA("[NAMED WEAPON] Resources.FindObjectsOfTypeAll(WeaponId) threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }
    if (!arrObj) {
        LogA("[NAMED WEAPON] WeaponId scan returned null.");
        return FALSE;
    }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) {
        LogA("[NAMED WEAPON] WeaponId scan exceeded defensive limit.");
        return FALSE;
    }

    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
    Il2CppObject* selected=0;
    int matches=0;

    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        ex=0;
        Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,weapon,0,&ex);
        if (ex || !nameObj) continue;

        char unityName[256]; unityName[0]=0;
        CatalogManagedStringToAscii((Il2CppString*)nameObj,unityName,256);
        if (!StrEqA(unityName,exactName)) continue;

        ++matches;
        if (!selected) selected=weapon;
    }

    char wantedLine[512]; wantedLine[0]=0;
    CatalogAppendA(wantedLine,512,special ? "[SPECIAL] Requested exact WeaponId: " : "[ARSENAL] Requested exact WeaponId: ");
    CatalogAppendA(wantedLine,512,exactName);
    LogA(wantedLine);
    LogIntA(special ? "[SPECIAL] Exact-name matches: " : "[ARSENAL] Exact-name matches: ",matches);

    if (matches!=1 || !selected) {
        LogA("[NAMED WEAPON] Refusing action because the exact Unity name is not unique.");
        return FALSE;
    }

    int category=-999, slot=-999;
    ex=0;
    Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,selected,0,&ex);
    if (!ex && catBox && g_il2cpp.object_unbox(catBox))
        category=*(int*)g_il2cpp.object_unbox(catBox);

    ex=0;
    Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,selected,0,&ex);
    if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
        slot=*(int*)g_il2cpp.object_unbox(slotBox);

    LogIntA(special ? "[SPECIAL] Category: " : "[ARSENAL] Category: ",category);
    LogIntA(special ? "[SPECIAL] Slot: " : "[ARSENAL] Slot: ",slot);

    if (category==1 && !IsTheseSunnyDazeOwned()) {
        LogA("[ARSENAL] PTSD/DLC weapon blocked because These Sunny Daze ownership was not confirmed.");
        return FALSE;
    }

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) {
        LogA("[NAMED WEAPON] PlayerInventoryComponent instance not found.");
        return FALSE;
    }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) {
        LogA("[NAMED WEAPON] PlayerInventoryComponent._weaponsController is null.");
        return FALSE;
    }

    // V2A42-H3: exact Hook-only experiment.
    // Historical tests covered InitHook-before-AddWeapon and V2A52 accidentally
    // skipped InitHook whenever AddWeapon already exposed get_Hook().
    // H3 tests the one sequence that has never actually been exercised:
    // AddWeapon(WEAPON_Hook) first, then InitHook() unconditionally.
    // Ordinary EquipWeapon remains intentionally skipped.
    if (StrEqA(exactName,"WEAPON_Hook")) {
        LogA("[HOOK H3] Exact sequence: AddWeapon if needed -> InitHook ALWAYS -> verify.");

        const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
        const MethodInfo* initHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"InitHook",0);
        const MethodInfo* getHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"get_Hook",0);
        if (!addWeapon || !initHook || !getHook) {
            LogA("[HOOK H3] AddWeapon/InitHook/get_Hook bridge unresolved.");
            return FALSE;
        }

        int collected=IsWeaponCollected(playerInv,inventoryClass,selected);
        if (collected==1) {
            LogA("[HOOK H3] WEAPON_Hook already collected; AddWeapon skipped.");
        } else {
            if (collected<0) {
                LogA("[HOOK H3] Could not inspect CollectedWeapons; refusing Hook mutation.");
                return FALSE;
            }

            void* addPars[1]; addPars[0]=selected;
            ex=0;
            g_il2cpp.runtime_invoke(addWeapon,weaponsInv,addPars,&ex);
            if (ex) {
                LogA("[HOOK H3] AddWeapon(WEAPON_Hook) threw:");
                FormatExceptionToLog(ex);
                return FALSE;
            }
            LogA("[HOOK H3] AddWeapon(WEAPON_Hook) completed.");
        }

        ex=0;
        Il2CppObject* hookBeforeInit=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
        if (ex) {
            LogA("[HOOK H3] get_Hook() before InitHook threw:");
            FormatExceptionToLog(ex);
            return FALSE;
        }
        LogA(hookBeforeInit==selected ? "[HOOK H3] get_Hook() already returns exact WEAPON_Hook BEFORE InitHook."
                                     : hookBeforeInit ? "[HOOK H3] get_Hook() non-null but different BEFORE InitHook."
                                                      : "[HOOK H3] get_Hook() NULL BEFORE InitHook.");

        // Critical H3 difference: DO NOT early-return here.
        LogA("[HOOK H3] Calling native WeaponsInventory.InitHook() AFTER AddWeapon state.");
        ex=0;
        g_il2cpp.runtime_invoke(initHook,weaponsInv,0,&ex);
        if (ex) {
            LogA("[HOOK H3] WeaponsInventory.InitHook() threw:");
            FormatExceptionToLog(ex);
            return FALSE;
        }

        ex=0;
        Il2CppObject* hookAfterInit=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
        if (ex) {
            LogA("[HOOK H3] get_Hook() after InitHook threw:");
            FormatExceptionToLog(ex);
            return FALSE;
        }

        if (hookAfterInit==selected) {
            LogA("[HOOK H3] SUCCESS: get_Hook() returns exact WEAPON_Hook AFTER forced post-Add InitHook.");
            return TRUE;
        }

        LogA(hookAfterInit ? "[HOOK H3] InitHook completed, but get_Hook() points to another WeaponId."
                           : "[HOOK H3] InitHook completed, but get_Hook() is NULL.");
        return FALSE;
    }

    BOOL ok=SafeAddAndEquipWeaponId(playerInv,weaponsInv,inventoryClass,weaponsInventoryClass,weaponIdClass,selected,
                                    special ? "[SPECIAL] Native AddWeapon(exact WeaponId) completed"
                                            : category==1 ? "[ARSENAL] Native AddWeapon(exact PTSD WeaponId) completed"
                                                          : "[ARSENAL] Native AddWeapon(exact base WeaponId) completed");
    if (ok) {
        char done[512]; done[0]=0;
        CatalogAppendA(done,512,special ? "[SPECIAL] Exact WeaponId active: " : "[ARSENAL] Exact WeaponId equipped: ");
        CatalogAppendA(done,512,exactName);
        LogA(done);
    }
    return ok;
}

// V2A38 bisect: keep the V2A35 exact-name resolver linked into the ASI,
// but do not expose or execute it. Reading this volatile pointer is harmless
// and prevents /OPT:REF from discarding the dormant resolver during linking.
static BOOL (*volatile g_dormantExactNameResolver)(const char*, BOOL)=ExecuteNamedWeaponByUnityName;

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
        LogA("[HOOK SAFE] These Sunny Daze ownership not confirmed; hook probe blocked.");
        return FALSE;
    }

    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if (!inventoryClass || !weaponsInventoryClass || !weaponIdClass) {
        LogA("[ERROR] DLC hook metadata classes unresolved");
        return FALSE;
    }

    LogHookLikeMethods(inventoryClass,"[HOOK SAFE] PlayerInventoryComponent hook-related methods:");
    LogHookLikeMethods(weaponsInventoryClass,"[HOOK SAFE] WeaponsInventory hook-related methods:");
    LogA("[HOOK SAFE] Global Assembly-CSharp scan is DISABLED.");

    const FieldInfo* weaponsControllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const FieldInfo* hookWeaponField=g_il2cpp.class_get_field_from_name(weaponsInventoryClass,"_hookWeapon");
    const MethodInfo* getHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"get_Hook",0);
    const MethodInfo* initHook=g_il2cpp.class_get_method_from_name(weaponsInventoryClass,"InitHook",0);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);

    if (!weaponsControllerField || !hookWeaponField || !getHook || !initHook || !getCategory || !getSlot) {
        LogA("[ERROR] Safe hook bridge unresolved (_hookWeapon/get_Hook/InitHook/WeaponId metadata)");
        return FALSE;
    }

    LogA("[HOOK SAFE] WeaponsInventory.get_Hook return metadata:");
    LogMethodReturnType(getHook);

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if (!playerInv) {
        LogA("[ERROR] PlayerInventoryComponent instance not found for hook probe");
        return FALSE;
    }

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,weaponsControllerField,&weaponsInv);
    if (!weaponsInv) {
        LogA("[ERROR] PlayerInventoryComponent._weaponsController is null for hook probe");
        return FALSE;
    }

    Il2CppObject* hookFieldValue=0;
    g_il2cpp.field_get_value(weaponsInv,hookWeaponField,&hookFieldValue);
    LogA(hookFieldValue ? "[HOOK SAFE] _hookWeapon field BEFORE InitHook: NON-NULL"
                        : "[HOOK SAFE] _hookWeapon field BEFORE InitHook: NULL");

    Il2CppException* ex=0;
    Il2CppObject* hookGetterValue=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.get_Hook() threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }
    LogA(hookGetterValue ? "[HOOK SAFE] get_Hook() BEFORE InitHook: NON-NULL"
                         : "[HOOK SAFE] get_Hook() BEFORE InitHook: NULL");

    if (hookFieldValue && hookGetterValue)
        LogA(hookFieldValue==hookGetterValue ? "[HOOK SAFE] _hookWeapon == get_Hook()"
                                            : "[HOOK SAFE] _hookWeapon != get_Hook()");

    Il2CppObject* inspectWeapon=hookGetterValue ? hookGetterValue : hookFieldValue;
    if (inspectWeapon) {
        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,inspectWeapon,0,&ex);
        if (!ex && catBox && g_il2cpp.object_unbox(catBox))
            LogIntA("[HOOK SAFE] Hook WeaponId category BEFORE InitHook: ",*(int*)g_il2cpp.object_unbox(catBox));

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,inspectWeapon,0,&ex);
        if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
            LogIntA("[HOOK SAFE] Hook WeaponId slot BEFORE InitHook: ",*(int*)g_il2cpp.object_unbox(slotBox));
    }

    if (hookGetterValue) {
        LogA("[HOOK SAFE] Hook WeaponId already exists; InitHook skipped.");
        return TRUE;
    }

    LogA("[HOOK SAFE] Calling native WeaponsInventory.InitHook() only.");
    ex=0;
    g_il2cpp.runtime_invoke(initHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.InitHook() threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    hookFieldValue=0;
    g_il2cpp.field_get_value(weaponsInv,hookWeaponField,&hookFieldValue);
    LogA(hookFieldValue ? "[HOOK SAFE] _hookWeapon field AFTER InitHook: NON-NULL"
                        : "[HOOK SAFE] _hookWeapon field AFTER InitHook: NULL");

    ex=0;
    hookGetterValue=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] WeaponsInventory.get_Hook() after InitHook threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }
    LogA(hookGetterValue ? "[HOOK SAFE] get_Hook() AFTER InitHook: NON-NULL"
                         : "[HOOK SAFE] get_Hook() AFTER InitHook: NULL");

    inspectWeapon=hookGetterValue ? hookGetterValue : hookFieldValue;
    if (inspectWeapon) {
        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,inspectWeapon,0,&ex);
        if (!ex && catBox && g_il2cpp.object_unbox(catBox))
            LogIntA("[HOOK SAFE] Hook WeaponId category AFTER InitHook: ",*(int*)g_il2cpp.object_unbox(catBox));

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,inspectWeapon,0,&ex);
        if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
            LogIntA("[HOOK SAFE] Hook WeaponId slot AFTER InitHook: ",*(int*)g_il2cpp.object_unbox(slotBox));
    }

    if (hookGetterValue || hookFieldValue) {
        LogA("[HOOK SLOT99] Hook WeaponId is already available after InitHook.");
        return TRUE;
    }

    LogA("[HOOK SLOT99] InitHook left both _hookWeapon and get_Hook() null.");
    LogA("[HOOK SLOT99] Searching loaded WeaponIds for the V2A31 signature: category 0 / slot 99.");

    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources") : 0;
    const MethodInfo* findAll=resourcesClass ? FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass) : 0;
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    if (!resourcesClass || !findAll || !addWeapon) {
        LogA("[ERROR] Hook slot99 Resources/AddWeapon bridge unresolved.");
        return FALSE;
    }

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if (!typeObj) {
        LogA("[ERROR] Hook slot99 WeaponId System.Type unavailable.");
        return FALSE;
    }

    void* findPars[1]; findPars[0]=typeObj;
    ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,findPars,&ex);
    if (ex) {
        LogA("[ERROR] Hook slot99 Resources.FindObjectsOfTypeAll(WeaponId) threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }
    if (!arrObj) {
        LogA("[ERROR] Hook slot99 WeaponId scan returned null.");
        return FALSE;
    }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if (n>1024) {
        LogA("[ERROR] Hook slot99 WeaponId scan exceeded defensive limit.");
        return FALSE;
    }

    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
    Il2CppObject* candidate=0;
    int matches=0;

    for (SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if (!weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if (ex || !catBox || !g_il2cpp.object_unbox(catBox)) continue;
        int category=*(int*)g_il2cpp.object_unbox(catBox);
        if (category!=0) continue;

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if (ex || !slotBox || !g_il2cpp.object_unbox(slotBox)) continue;
        int slot=*(int*)g_il2cpp.object_unbox(slotBox);
        if (slot!=99) continue;

        ++matches;
        if (!candidate) candidate=weapon;
    }

    LogIntA("[HOOK SLOT99] Loaded category0/slot99 WeaponId matches: ",matches);
    if (matches!=1 || !candidate) {
        LogA("[HOOK SLOT99] Refusing mutation because the slot99 candidate is not unique.");
        return FALSE;
    }

    int collected=IsWeaponCollected(playerInv,inventoryClass,candidate);
    if (collected==1) {
        LogA("[HOOK SLOT99] Candidate is already in CollectedWeapons; AddWeapon skipped.");
    } else {
        if (collected<0)
            LogA("[HOOK SLOT99] CollectedWeapons could not be inspected; refusing AddWeapon.");
        if (collected<0) return FALSE;

        LogA("[HOOK SLOT99] Candidate is not collected; invoking native AddWeapon ONLY (no EquipWeapon).");
        void* addPars[1]; addPars[0]=candidate;
        ex=0;
        g_il2cpp.runtime_invoke(addWeapon,weaponsInv,addPars,&ex);
        if (ex) {
            LogA("[ERROR] Hook slot99 AddWeapon threw:");
            FormatExceptionToLog(ex);
            return FALSE;
        }
        LogA("[HOOK SLOT99] Native AddWeapon(category0/slot99) completed.");
    }

    ex=0;
    hookGetterValue=g_il2cpp.runtime_invoke(getHook,weaponsInv,0,&ex);
    if (ex) {
        LogA("[ERROR] get_Hook() after slot99 AddWeapon threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    LogA(hookGetterValue ? "[HOOK SLOT99] get_Hook() AFTER slot99 AddWeapon: NON-NULL"
                         : "[HOOK SLOT99] get_Hook() AFTER slot99 AddWeapon: NULL");

    hookFieldValue=0;
    g_il2cpp.field_get_value(weaponsInv,hookWeaponField,&hookFieldValue);
    LogA(hookFieldValue ? "[HOOK SLOT99] _hookWeapon AFTER slot99 AddWeapon: NON-NULL"
                        : "[HOOK SLOT99] _hookWeapon AFTER slot99 AddWeapon: NULL");

    if (!hookGetterValue) {
        LogA("[HOOK SLOT99] AddWeapon(slot99) was insufficient to expose the Hook WeaponId.");
        return FALSE;
    }

    LogA(hookGetterValue==candidate ? "[HOOK SLOT99] SUCCESS: get_Hook() returns the exact category0/slot99 candidate."
                                    : "[HOOK SLOT99] get_Hook() is non-null but returns a different WeaponId pointer.");

    ex=0;
    Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,hookGetterValue,0,&ex);
    if (!ex && catBox && g_il2cpp.object_unbox(catBox))
        LogIntA("[HOOK SLOT99] get_Hook() category: ",*(int*)g_il2cpp.object_unbox(catBox));

    ex=0;
    Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,hookGetterValue,0,&ex);
    if (!ex && slotBox && g_il2cpp.object_unbox(slotBox))
        LogIntA("[HOOK SLOT99] get_Hook() slot: ",*(int*)g_il2cpp.object_unbox(slotBox));

    return TRUE;
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

static BOOL InvokeInputManagerWheelEventFastH51(BOOL down);
static int ReadPlayerWheelEnabledH49();
static volatile LONG g_h51BaseFilterPending=0;
static BOOL g_h51BaseUsesInputEvent=FALSE;


static void LogUnityObjectNameH55(Il2CppObject* obj,const char* label);
static void LogImageSpriteH55(Il2CppObject* imageObj,const char* label);
static BOOL GetWeaponNameH54(Il2CppObject* weapon,char* out,int cap);

// H78 visual audit is defined later, after the H55 helpers, but the BASE
// mirror path appears earlier in this translation unit.
static BOOL g_h78AuditBaseInDlcDone=FALSE;
static BOOL g_h78AuditDlcInBaseDone=FALSE;
static void AuditMappedButtonChildImagesH78(const char* direction,
                                             Il2CppObject** buttons,
                                             int count,
                                             Il2CppClass* buttonClass,
                                             const MethodInfo* getWeapon);

// H57: the two native scenes expose complementary 5-weapon variants.
// Keep the 4 common sectors untouched and temporarily mirror only the five
// mode-specific WeaponId pointers while a wheel is open.
static BOOL g_h57BaseSwapApplied=FALSE;
static Il2CppObject* g_h57BaseMappedButtons[5]={0,0,0,0,0};
static Il2CppObject* g_h57BaseOriginalWeapons[5]={0,0,0,0,0};
static int g_h57BaseMappedCount=0;

static const char* g_h57BaseNames[5]={
    "WEAPON_Pistol",
    "WEAPON_Shovel",
    "WEAPON_CatCanon",
    "WEAPON_Shotgun",
    "WEAPON_DildoBow"
};

static const char* g_h57DlcNames[5]={
    "WEAPON_UmDrill",
    "WEAPON_PissGun",
    "WEAPON_MeatShotgun",
    "WEAPON_BubbleGumMachineGun",
    "WEAPON_NuclearSyringe"
};

// H59: authoritative wheel-pairing key.
// These are the five slots that differ between base game and These Sunny Daze.
// Slots 5,6,7,9 stay native/common and must never be remapped.
static const int g_h59VariableSlots[5]={1,2,3,4,8};
static int SlotIndexH59(int slot);

// H61: safe visual-only mapping. These names come from the game's own
// native wheel snapshots logged by H56. We change ONLY _weaponSelectedImage.sprite.
// No WeaponId, slot, category, selection or enable logic is modified here.
static const char* g_h61BaseIconNames[5]={
    "Bez-nazwy-1_0000_Warstwa-18", // slot 1 Shovel
    "Bez-nazwy-1_0005_Warstwa-13", // slot 2 Pistol
    "Bez-nazwy-1_0002_Warstwa-16", // slot 3 Shotgun
    "Bez-nazwy-1_0004_Warstwa-14", // slot 4 MachineGun
    "Bez-nazwy-1_0008_Warstwa-10"  // slot 8 DildoBow
};

static const char* g_h61DlcIconNames[5]={
    "Weapon Icon_NyanBrella_Rose",      // slot 1 UmDrill
    "Weapon Icon_Piss Gun_Rose",        // slot 2 PissGun
    "Weapon Icon_Wiener-Grinder_Rose",  // slot 3 MeatShotgun
    "Weapon Icon_Bubble Gun2_Rose",     // slot 4 BubbleGumMachineGun
    "Weapon Icon_UrAnusium2_Rose"       // slot 8 NuclearSyringe
};

static Il2CppObject* g_h61BaseIconButtons[5]={0,0,0,0,0};
static Il2CppObject* g_h61BaseOriginalSprites[5]={0,0,0,0,0};
static Il2CppObject* g_h61DlcIconButtons[5]={0,0,0,0,0};
static Il2CppObject* g_h61DlcOriginalSprites[5]={0,0,0,0,0};

// H62: cache all ten wheel sprites in ONE Resources scan instead of the H61
// five-scans-per-open path.
static Il2CppObject* g_h62BaseSpriteCache[5]={0,0,0,0,0};
static Il2CppObject* g_h62DlcSpriteCache[5]={0,0,0,0,0};
static volatile LONG g_h62SpriteScanAttempts=0;
static volatile LONG g_h62SpriteCacheComplete=0;

// H62: some WeaponWheelButton prefabs render the weapon through more than one
// Image. H61 changed only _weaponSelectedImage, exposing the old icon below it.
// Keep a tiny per-button backup for matching local Image layers.
static Il2CppObject* g_h62BaseExtraImages[5][8]={{0}};
static Il2CppObject* g_h62BaseExtraOriginalSprites[5][8]={{0}};
static int g_h62BaseExtraCount[5]={0,0,0,0,0};
static Il2CppObject* g_h62DlcExtraImages[5][8]={{0}};
static Il2CppObject* g_h62DlcExtraOriginalSprites[5][8]={{0}};
static int g_h62DlcExtraCount[5]={0,0,0,0,0};

static void ClearExtraLayersH62(BOOL baseState,int index) {
    if(index<0 || index>=5) return;
    if(baseState) {
        g_h62BaseExtraCount[index]=0;
        for(int j=0;j<8;j++) {
            g_h62BaseExtraImages[index][j]=0;
            g_h62BaseExtraOriginalSprites[index][j]=0;
        }
    } else {
        g_h62DlcExtraCount[index]=0;
        for(int j=0;j<8;j++) {
            g_h62DlcExtraImages[index][j]=0;
            g_h62DlcExtraOriginalSprites[index][j]=0;
        }
    }
}

static void ClearBaseVisualStateH61() {
    for(int i=0;i<5;i++) {
        g_h61BaseIconButtons[i]=0;
        g_h61BaseOriginalSprites[i]=0;
        ClearExtraLayersH62(TRUE,i);
    }
}

static void ClearDlcVisualStateH61() {
    for(int i=0;i<5;i++) {
        g_h61DlcIconButtons[i]=0;
        g_h61DlcOriginalSprites[i]=0;
        ClearExtraLayersH62(FALSE,i);
    }
}

static int CountCachedWheelSpritesH62() {
    int n=0;
    for(int i=0;i<5;i++) {
        if(g_h62BaseSpriteCache[i]) ++n;
        if(g_h62DlcSpriteCache[i]) ++n;
    }
    return n;
}

static BOOL BuildWheelSpriteCacheH62(BOOL forceRetry) {
    int cached=CountCachedWheelSpritesH62();
    if(cached==10) {
        g_h62SpriteCacheComplete=1;
        return TRUE;
    }

    LONG attempts=g_h62SpriteScanAttempts;
    if(!forceRetry && attempts>=2) return FALSE;
    g_h62SpriteScanAttempts=attempts+1;

    Il2CppClass* spriteClass=g_coreImage ? g_il2cpp.class_from_name(
        g_coreImage,"UnityEngine","Sprite") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(
        g_coreImage,"UnityEngine","Resources") : 0;
    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(
        g_coreImage,"UnityEngine","Object") : 0;
    if(!spriteClass || !resourcesClass || !unityObjectClass) return FALSE;

    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);
    const MethodInfo* getName=g_il2cpp.class_get_method_from_name(unityObjectClass,"get_name",0);
    if(!findAll || !getName) return FALSE;

    const Il2CppType* st=g_il2cpp.class_get_type(spriteClass);
    Il2CppObject* typeObj=st ? g_il2cpp.type_get_object(st) : 0;
    if(!typeObj) return FALSE;

    LogA("[WHEEL H62] Building 10-sprite wheel cache with ONE Resources scan...");

    void* pars[1]; pars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,pars,&ex);
    if(ex || !arrObj) {
        if(ex) FormatExceptionToLog(ex);
        return FALSE;
    }

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if(n>32768) n=32768;

    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    for(SIZE_T i=0;i<n;i++) {
        Il2CppObject* sprite=elems[i];
        if(!sprite) continue;

        ex=0;
        Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,sprite,0,&ex);
        if(ex || !nameObj) continue;

        char name[192]; name[0]=0;
        CatalogManagedStringToAscii((Il2CppString*)nameObj,name,192);

        for(int k=0;k<5;k++) {
            if(!g_h62BaseSpriteCache[k] && StrEqA(name,g_h61BaseIconNames[k]))
                g_h62BaseSpriteCache[k]=sprite;
            if(!g_h62DlcSpriteCache[k] && StrEqA(name,g_h61DlcIconNames[k]))
                g_h62DlcSpriteCache[k]=sprite;
        }
    }

    cached=CountCachedWheelSpritesH62();
    LogIntA("[WHEEL H62] cached wheel sprites: ",cached);
    if(cached==10) {
        g_h62SpriteCacheComplete=1;
        LogA("[WHEEL H62] Sprite cache complete. Shortcut opens require no global sprite scan.");
        return TRUE;
    }

    LogA("[WHEEL H62] Sprite cache partial; one later retry is allowed.");
    return FALSE;
}

static Il2CppObject* FindLoadedSpriteByExactNameH61(const char* wantedName) {
    if(!wantedName || !wantedName[0]) return 0;

    for(int k=0;k<5;k++) {
        if(StrEqA(wantedName,g_h61BaseIconNames[k])) {
            if(!g_h62BaseSpriteCache[k]) BuildWheelSpriteCacheH62(FALSE);
            return g_h62BaseSpriteCache[k];
        }
        if(StrEqA(wantedName,g_h61DlcIconNames[k])) {
            if(!g_h62DlcSpriteCache[k]) BuildWheelSpriteCacheH62(FALSE);
            return g_h62DlcSpriteCache[k];
        }
    }
    return 0;
}

static BOOL SetImageSpriteObjectH62(Il2CppObject* image,Il2CppObject* sprite,Il2CppObject** outOriginal) {
    if(outOriginal) *outOriginal=0;
    if(!image || !outOriginal) return FALSE;

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    Il2CppClass* imageClass=objectGetClass ? objectGetClass(image) : 0;
    if(!imageClass) return FALSE;

    const MethodInfo* getSprite=g_il2cpp.class_get_method_from_name(imageClass,"get_sprite",0);
    const MethodInfo* setSprite=g_il2cpp.class_get_method_from_name(imageClass,"set_sprite",1);
    if(!getSprite || !setSprite) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* original=g_il2cpp.runtime_invoke(getSprite,image,0,&ex);
    if(ex) return FALSE;

    void* sp[1]; sp[0]=sprite;
    ex=0;
    g_il2cpp.runtime_invoke(setSprite,image,sp,&ex);
    if(ex) return FALSE;

    *outOriginal=original;
    return TRUE;
}

static BOOL SetButtonIconSpriteH61(Il2CppObject* button,
                                   Il2CppClass* buttonClass,
                                   Il2CppObject* sprite,
                                   Il2CppObject** outOriginalSprite) {
    if(outOriginalSprite) *outOriginalSprite=0;
    if(!button || !buttonClass || !sprite || !outOriginalSprite) return FALSE;

    const FieldInfo* imageField=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponSelectedImage");
    if(!imageField) return FALSE;

    Il2CppObject* image=0;
    g_il2cpp.field_get_value(button,imageField,&image);
    if(!image) return FALSE;

    if(!SetImageSpriteObjectH62(image,sprite,outOriginalSprite)) {
        LogA("[WHEEL H62] failed to set _weaponSelectedImage sprite.");
        return FALSE;
    }
    return TRUE;
}

static BOOL ReplaceMatchingLocalIconLayersH62(Il2CppObject* button,
                                               Il2CppObject* originalSprite,
                                               Il2CppObject* targetSprite,
                                               BOOL baseState,
                                               int index) {
    // H65 clean baseline: preserve H61 visual behavior exactly.
    // H62 cache stays enabled for performance, but no secondary/background
    // Image is modified here.
    (void)button; (void)originalSprite; (void)targetSprite;
    ClearExtraLayersH62(baseState,index);
    return TRUE;
}

static BOOL RestoreExtraIconLayersH62(BOOL baseState,int index) {
    if(index<0 || index>=5) return TRUE;

    int count=baseState ? g_h62BaseExtraCount[index] : g_h62DlcExtraCount[index];
    BOOL ok=TRUE;

    for(int j=0;j<count;j++) {
        Il2CppObject* image=baseState ? g_h62BaseExtraImages[index][j] : g_h62DlcExtraImages[index][j];
        Il2CppObject* sprite=baseState ? g_h62BaseExtraOriginalSprites[index][j] : g_h62DlcExtraOriginalSprites[index][j];
        if(!image) continue;

        Il2CppObject* ignored=0;
        if(!SetImageSpriteObjectH62(image,sprite,&ignored))
            ok=FALSE;
    }

    ClearExtraLayersH62(baseState,index);
    return ok;
}

static BOOL RestoreButtonIconSpriteH61(Il2CppObject* button,
                                       Il2CppClass* buttonClass,
                                       Il2CppObject* originalSprite) {
    if(!button || !buttonClass) return TRUE;

    const FieldInfo* imageField=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponSelectedImage");
    if(!imageField) return FALSE;

    Il2CppObject* image=0;
    g_il2cpp.field_get_value(button,imageField,&image);
    if(!image) return FALSE;

    Il2CppObject* ignored=0;
    return SetImageSpriteObjectH62(image,originalSprite,&ignored);
}

static BOOL ApplySlotIconH61(Il2CppObject* button,
                             Il2CppClass* buttonClass,
                             int slot,
                             BOOL wantDlc,
                             BOOL baseVisualState,
                             int visualIndex) {
    int k=SlotIndexH59(slot);
    if(k<0 || visualIndex<0 || visualIndex>=5) return FALSE;

    const char* wanted=wantDlc ? g_h61DlcIconNames[k] : g_h61BaseIconNames[k];
    Il2CppObject* sprite=FindLoadedSpriteByExactNameH61(wanted);
    if(!sprite) {
        LogIntA("[WHEEL H62] target sprite NOT cached for slot: ",slot);
        LogA(wanted);
        return FALSE;
    }

    Il2CppObject* original=0;
    if(!SetButtonIconSpriteH61(button,buttonClass,sprite,&original)) {
        LogIntA("[WHEEL H62] failed to set primary icon sprite for slot: ",slot);
        return FALSE;
    }

    // H62: replace any second/underlay Image still drawing the old icon.
    ReplaceMatchingLocalIconLayersH62(button,original,sprite,baseVisualState,visualIndex);

    if(baseVisualState) {
        g_h61BaseIconButtons[visualIndex]=button;
        g_h61BaseOriginalSprites[visualIndex]=original;
    } else {
        g_h61DlcIconButtons[visualIndex]=button;
        g_h61DlcOriginalSprites[visualIndex]=original;
    }

    LogIntA(wantDlc ? "[WHEEL H62] DLC icon layers applied to slot: " : "[WHEEL H62] BASE icon layers applied to slot: ",slot);
    return TRUE;
}

static void PrewarmWheelSpriteCacheH62() {
    if(!g_ready || g_shuttingDown || g_h62SpriteCacheComplete) return;

    BOOL attached=FALSE; void* thread=0;
    if(!EnsureIl2CppThread(&attached,&thread)) return;
    BuildWheelSpriteCacheH62(TRUE);
    ReleaseIl2CppThread(attached,thread);
}

static int SlotIndexH59(int slot) {
    for(int i=0;i<5;i++) if(g_h59VariableSlots[i]==slot) return i;
    return -1;
}

static BOOL GetWeaponSlotH59(Il2CppObject* weapon,int* outSlot) {
    if(outSlot) *outSlot=-1;
    if(!weapon || !outSlot) return FALSE;

    Il2CppClass* weaponClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if(!weaponClass) return FALSE;

    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponClass,"get_Slot",0);
    if(!getSlot) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* boxed=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
    if(ex || !boxed) return FALSE;

    void* raw=g_il2cpp.object_unbox(boxed);
    if(!raw) return FALSE;

    *outSlot=*(int*)raw;
    return TRUE;
}

static BOOL IsAkimboLikeWeaponNameH59(Il2CppObject* weapon) {
    if(!weapon) return FALSE;

    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(
        g_coreImage,"UnityEngine","Object") : 0;
    const MethodInfo* getName=unityObjectClass ? g_il2cpp.class_get_method_from_name(
        unityObjectClass,"get_name",0) : 0;
    if(!getName) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,weapon,0,&ex);
    if(ex || !nameObj) return FALSE;

    char name[128]; name[0]=0;
    CatalogManagedStringToAscii((Il2CppString*)nameObj,name,128);
    return StrContainsNoCaseA(name,"akimbo") ||
           StrContainsNoCaseA(name,"dual") ||
           StrContainsNoCaseA(name,"double");
}

static int CollectCanonicalWeaponsBySlotsH59(int wantedCategory,Il2CppObject* out[5]) {
    for(int i=0;i<5;i++) out[i]=0;

    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* resourcesClass=g_coreImage ? g_il2cpp.class_from_name(
        g_coreImage,"UnityEngine","Resources") : 0;
    if(!weaponIdClass || !resourcesClass) return 0;

    const MethodInfo* findAll=FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Slot",0);
    if(!findAll || !getCategory || !getSlot) return 0;

    const Il2CppType* wt=g_il2cpp.class_get_type(weaponIdClass);
    Il2CppObject* typeObj=wt ? g_il2cpp.type_get_object(wt) : 0;
    if(!typeObj) return 0;

    void* pars[1]; pars[0]=typeObj;
    Il2CppException* ex=0;
    Il2CppObject* arrObj=g_il2cpp.runtime_invoke(findAll,0,pars,&ex);
    if(ex || !arrObj) return 0;

    Il2CppArray* arr=(Il2CppArray*)arrObj;
    SIZE_T n=g_il2cpp.array_length(arr);
    if(n>1024) return 0;

    unsigned int hdr=g_il2cpp.array_object_header_size();
    Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

    for(SIZE_T i=0;i<n;i++) {
        Il2CppObject* weapon=elems[i];
        if(!weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if(ex || !catBox) continue;
        void* catRaw=g_il2cpp.object_unbox(catBox);
        if(!catRaw || *(int*)catRaw!=wantedCategory) continue;

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if(ex || !slotBox) continue;
        void* slotRaw=g_il2cpp.object_unbox(slotBox);
        if(!slotRaw) continue;

        int slot=*(int*)slotRaw;
        int k=SlotIndexH59(slot);
        if(k<0) continue;

        // Prefer the canonical single-weapon WeaponId over Akimbo/dual variants
        // when multiple WeaponIds share a gameplay slot.
        if(!out[k] || (IsAkimboLikeWeaponNameH59(out[k]) && !IsAkimboLikeWeaponNameH59(weapon)))
            out[k]=weapon;
    }

    int found=0;
    for(int i=0;i<5;i++) {
        if(out[i]) {
            ++found;
            char name[128]; name[0]=0;
            GetWeaponNameH54(out[i],name,128);
            LogIntA(wantedCategory==0 ? "[WHEEL H59] BASE target slot: " : "[WHEEL H59] DLC target slot: ",g_h59VariableSlots[i]);
            LogA(name);
        }
    }
    return found;
}

static void ClearBaseSwapStateH57() {
    for(int i=0;i<5;i++) {
        g_h57BaseMappedButtons[i]=0;
        g_h57BaseOriginalWeapons[i]=0;
    }
    g_h57BaseMappedCount=0;
    ClearBaseVisualStateH61();
}

static BOOL SetWheelButtonWeaponH57(Il2CppObject* button,
                                    Il2CppClass* buttonClass,
                                    const MethodInfo* getWeapon,
                                    Il2CppObject* weapon) {
    if(!button || !buttonClass || !getWeapon || !weapon) return FALSE;

    const FieldInfo* field=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponId");
    if(!field) return FALSE;

    typedef void (*FnSetObject)(Il2CppObject*,const FieldInfo*,Il2CppObject*);
    FnSetObject setObject=(FnSetObject)Proc("il2cpp_field_set_value_object");
    if(!setObject) return FALSE;

    setObject(button,field,weapon);

    Il2CppException* ex=0;
    Il2CppObject* verify=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
    return !ex && verify==weapon;
}

static int FindExactBaseTargetsH57(Il2CppObject* out[5]) {
    return CollectCanonicalWeaponsBySlotsH59(0,out);
}

static int EnsureBaseTargetsCollectedH57(Il2CppObject* targets[5]) {
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    if(!weaponIdClass || !inventoryClass || !weaponsInventoryClass) return 0;

    const FieldInfo* controllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    if(!controllerField || !addWeapon) return 0;

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if(!playerInv) return 0;

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,controllerField,&weaponsInv);
    if(!weaponsInv) return 0;

    int ready=0;
    for(int i=0;i<5;i++) {
        Il2CppObject* weapon=targets[i];
        if(!weapon) continue;

        int collected=IsWeaponCollected(playerInv,inventoryClass,weapon);
        if(collected==1) {
            ++ready;
            continue;
        }
        if(collected<0) continue;

        void* wp[1]; wp[0]=weapon;
        Il2CppException* ex=0;
        g_il2cpp.runtime_invoke(addWeapon,weaponsInv,wp,&ex);
        if(!ex) ++ready;
    }
    return ready;
}

static BOOL RestoreBaseSwapH57() {
    if(!g_h57BaseSwapApplied || g_h57BaseMappedCount<=0) {
        ClearBaseSwapStateH57();
        g_h57BaseSwapApplied=FALSE;
        return TRUE;
    }

    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    if(!buttonClass) return FALSE;

    const MethodInfo* getWeapon=g_il2cpp.class_get_method_from_name(buttonClass,"get_WeaponId",0);
    if(!getWeapon) return FALSE;

    BOOL ok=TRUE;
    for(int i=0;i<g_h57BaseMappedCount;i++) {
        if(g_h61BaseIconButtons[i]) {
            if(!RestoreExtraIconLayersH62(TRUE,i)) ok=FALSE;
            if(!RestoreButtonIconSpriteH61(g_h61BaseIconButtons[i],buttonClass,g_h61BaseOriginalSprites[i]))
                ok=FALSE;
        }

        if(!SetWheelButtonWeaponH57(g_h57BaseMappedButtons[i],buttonClass,getWeapon,g_h57BaseOriginalWeapons[i]))
            ok=FALSE;
    }

    ClearBaseSwapStateH57();
    g_h57BaseSwapApplied=FALSE;
    return ok;
}

static BOOL g_h56LoggedNormalLayout=FALSE;
static BOOL g_h56LoggedDlcLayout=FALSE;

static void LogButtonIdH56(Il2CppObject* button,Il2CppClass* buttonClass) {
    if(!button || !buttonClass) return;

    const FieldInfo* idField=g_il2cpp.class_get_field_from_name(buttonClass,"_id");
    if(!idField) {
        LogA("[WHEEL H56] _id field unresolved.");
        return;
    }

    typedef const Il2CppType* (*H56FieldGetTypeFn)(const FieldInfo*);
    H56FieldGetTypeFn fieldGetType=(H56FieldGetTypeFn)Proc("il2cpp_field_get_type");
    if(!fieldGetType) return;

    const Il2CppType* t=fieldGetType(idField);
    Il2CppClass* tc=t ? g_il2cpp.class_from_type(t) : 0;
    const char* ns=tc ? g_il2cpp.class_get_namespace(tc) : 0;
    const char* cn=tc ? g_il2cpp.class_get_name(tc) : 0;

    LogA("[WHEEL H56] _id declared type:");
    if(ns && ns[0]) LogA(ns);
    if(cn) LogA(cn);

    BOOL primitive=FALSE;
    if(ns && StrEqA(ns,"System") && cn) {
        primitive=StrEqA(cn,"Int32") || StrEqA(cn,"UInt32") ||
                  StrEqA(cn,"Int16") || StrEqA(cn,"UInt16") ||
                  StrEqA(cn,"Byte") || StrEqA(cn,"SByte") ||
                  StrEqA(cn,"Int64") || StrEqA(cn,"UInt64") ||
                  StrEqA(cn,"Boolean");
    }

    typedef int (*FnClassIsEnum)(Il2CppClass*);
    FnClassIsEnum classIsEnum=(FnClassIsEnum)Proc("il2cpp_class_is_enum");
    BOOL isEnum=(tc && classIsEnum && classIsEnum(tc)) ? TRUE : FALSE;

    if(primitive || isEnum) {
        unsigned __int64 raw=0;
        g_il2cpp.field_get_value(button,idField,&raw);
        LogIntA("[WHEEL H56] _id numeric low32: ",(int)(raw & 0xFFFFFFFFu));
    }
}

static void AuditCurrentWheelLayoutH56(const char* label,int baseCount,int dlcCount) {
    if(baseCount==9 && dlcCount==0) {
        if(g_h56LoggedNormalLayout) return;
        g_h56LoggedNormalLayout=TRUE;
    } else if(baseCount==4 && dlcCount==5) {
        if(g_h56LoggedDlcLayout) return;
        g_h56LoggedDlcLayout=TRUE;
    } else {
        return;
    }

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* weaponClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if(!wheelClass || !buttonClass || !weaponClass) return;

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const FieldInfo* selectedImageField=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponSelectedImage");
    const FieldInfo* backgroundField=g_il2cpp.class_get_field_from_name(buttonClass,"_backgroundImage");
    const MethodInfo* getWeapon=g_il2cpp.class_get_method_from_name(buttonClass,"get_WeaponId",0);
    const MethodInfo* getEnabled=g_il2cpp.class_get_method_from_name(buttonClass,"get_Enabled",0);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponClass,"get_Category",0);
    const MethodInfo* getSlot=g_il2cpp.class_get_method_from_name(weaponClass,"get_Slot",0);
    if(!buttonsField || !getWeapon || !getCategory || !getSlot) return;

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    if(!wheel) return;

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if(!collection) return;

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if(!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) return;

    LogA("[WHEEL H56] ============================================================");
    LogA("[WHEEL H56] NATIVE WHEEL MIRROR SNAPSHOT");
    if(label) LogA(label);
    LogIntA("[WHEEL H56] base count: ",baseCount);
    LogIntA("[WHEEL H56] dlc count: ",dlcCount);
    LogIntA("[WHEEL H56] button count: ",count);

    for(int i=0;i<count;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if(!button) continue;

        LogA("[WHEEL H56] ------------------------------------------------------------");
        LogIntA("[WHEEL H56] button index: ",i);
        LogUnityObjectNameH55(button,"[WHEEL H56] button object name:");
        LogButtonIdH56(button,buttonClass);

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
        if(ex || !weapon) {
            if(ex) FormatExceptionToLog(ex);
            LogA("[WHEEL H56] WeaponId: <NULL>");
            continue;
        }

        char weaponName[128]; weaponName[0]=0;
        GetWeaponNameH54(weapon,weaponName,128);
        LogA("[WHEEL H56] WeaponId:");
        LogA(weaponName);

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if(!ex && catBox) {
            void* raw=g_il2cpp.object_unbox(catBox);
            if(raw) LogIntA("[WHEEL H56] category: ",*(int*)raw);
        }

        ex=0;
        Il2CppObject* slotBox=g_il2cpp.runtime_invoke(getSlot,weapon,0,&ex);
        if(!ex && slotBox) {
            void* raw=g_il2cpp.object_unbox(slotBox);
            if(raw) LogIntA("[WHEEL H56] weapon slot: ",*(int*)raw);
        }

        if(getEnabled) {
            ex=0;
            Il2CppObject* enabledBox=g_il2cpp.runtime_invoke(getEnabled,button,0,&ex);
            if(!ex && enabledBox) {
                void* raw=g_il2cpp.object_unbox(enabledBox);
                if(raw) LogIntA("[WHEEL H56] Enabled: ",(*(BYTE*)raw)?1:0);
            }
        }

        Il2CppObject* selectedImage=0;
        Il2CppObject* backgroundImage=0;
        if(selectedImageField) g_il2cpp.field_get_value(button,selectedImageField,&selectedImage);
        if(backgroundField) g_il2cpp.field_get_value(button,backgroundField,&backgroundImage);

        LogImageSpriteH55(selectedImage,"[WHEEL H56] _weaponSelectedImage:");
        LogImageSpriteH55(backgroundImage,"[WHEEL H56] _backgroundImage:");
    }

    LogA("[WHEEL H56] ============================================================");
}

static BOOL ShowNativeBaseWeaponWheel() {
    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelButtonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if (!wheelClass || !wheelButtonClass || !wheelViewClass || !weaponIdClass) {
        LogA("[ERROR] H51 base wheel metadata classes unresolved");
        return FALSE;
    }

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* buttonGetWeapon=g_il2cpp.class_get_method_from_name(wheelButtonClass,"get_WeaponId",0);
    const MethodInfo* buttonEnable=g_il2cpp.class_get_method_from_name(wheelButtonClass,"Enable",1);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    const MethodInfo* showMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Show",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    if (!buttonsField || !buttonGetWeapon || !buttonEnable || !getCategory || !showMethod || !restoreMethod) {
        LogA("[ERROR] H51 base wheel native bridge unresolved");
        return FALSE;
    }

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);
    if (!wheel || !view) {
        LogA("[ERROR] H51 live base weapon wheel/view not found");
        return FALSE;
    }

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if (!collection) return FALSE;

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if (!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) return FALSE;

    int baseCount=0,dlcCount=0,otherCount=0;
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
        if(cat==0) ++baseCount;
        else if(cat==1) ++dlcCount;
        else ++otherCount;
    }

    LogIntA("[WHEEL H51] BASE buttons before open: ",baseCount);
    LogIntA("[WHEEL H51] DLC buttons before open: ",dlcCount);
    // H74: runtime H55/H56 wheel-layout audit disabled.

    if(baseCount<=0) {
        LogA("[WHEEL H51] No base buttons available.");
        return FALSE;
    }

    // DLC scene: the real wheel setup is asynchronous. Fire the proven native
    // input event first, then wait until Player.IsWheelEnabled == 1 before
    // applying the base-only filter.
    if(dlcCount>0) {
        if(!InvokeInputManagerWheelEventFastH51(TRUE)) {
            LogA("[WHEEL H51] Native OnWheelDown failed for base-wheel DLC path.");
            return FALSE;
        }
        g_h51BaseUsesInputEvent=TRUE;
        g_h51BaseFilterPending=1;
        g_baseWheelActive=1;
        LogA("[WHEEL H51] Base wheel native input fired; waiting for active wheel before filtering DLC buttons.");
        return TRUE;
    }

    // H74: normal scene also uses the real InputManager OnWheelDown event.
    // This preserves the game's native tap semantics:
    // short press = swap with previous weapon, hold = show/select wheel.
    if(!InvokeInputManagerWheelEventFastH51(TRUE)) {
        LogA("[WHEEL H74] Native OnWheelDown failed in normal scene.");
        return FALSE;
    }

    g_h51BaseUsesInputEvent=TRUE;
    g_h51BaseFilterPending=0;
    g_baseWheelActive=1;
    LogA("[WHEEL H74] Base wheel opened through native OnWheelDown; tap/hold semantics preserved.");
    return TRUE;
}

static BOOL ApplyDelayedBaseFilterH51() {
    if(!g_baseWheelActive || !g_h51BaseUsesInputEvent || !g_h51BaseFilterPending) return TRUE;
    if(ReadPlayerWheelEnabledH49()!=1) return FALSE;

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* weaponClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if(!wheelClass || !buttonClass || !weaponClass) return FALSE;

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* getWeapon=g_il2cpp.class_get_method_from_name(buttonClass,"get_WeaponId",0);
    const MethodInfo* enable=g_il2cpp.class_get_method_from_name(buttonClass,"Enable",1);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponClass,"get_Category",0);
    if(!buttonsField || !getWeapon || !enable || !getCategory) return FALSE;

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    if(!wheel) return FALSE;

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if(!collection) return FALSE;

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if(!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) return FALSE;

    // H57: in the DLC scene the native 9-button layout is 4 common BASE +
    // 5 DLC variants. H51 only disabled the five DLC buttons, which is why the
    // "base" wheel contained only four weapons. Mirror those five variant
    // pointers back to their exact base WeaponIds instead.
    Il2CppObject* baseTargets[5]={0,0,0,0,0};
    int found=FindExactBaseTargetsH57(baseTargets);
    LogIntA("[WHEEL H57] exact BASE targets found: ",found);
    if(found!=5) {
        g_h51BaseFilterPending=0;
        return FALSE;
    }

    int ready=EnsureBaseTargetsCollectedH57(baseTargets);
    LogIntA("[WHEEL H57] BASE targets collected/ready: ",ready);
    if(ready!=5) {
        g_h51BaseFilterPending=0;
        return FALSE;
    }

    ClearBaseSwapStateH57();

    int baseEnabled=0;
    for(int i=0;i<count;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if(!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
        if(ex || !weapon) continue;

        char currentName[128]; currentName[0]=0;
        GetWeaponNameH54(weapon,currentName,128);

        ex=0;
        Il2CppObject* box=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if(ex || !box) continue;
        void* raw=g_il2cpp.object_unbox(box);
        if(!raw) continue;

        int cat=*(int*)raw;
        BOOL enabled=FALSE;

        if(cat==0) {
            // The four common base buttons remain native and untouched.
            enabled=TRUE;
        } else if(cat==1) {
            int sourceSlot=-1;
            if(GetWeaponSlotH59(weapon,&sourceSlot)) {
                int k=SlotIndexH59(sourceSlot);
                if(k>=0 && baseTargets[k]) {
                    char targetName[128]; targetName[0]=0;
                    GetWeaponNameH54(baseTargets[k],targetName,128);
                    LogIntA("[WHEEL H59] DLC->BASE pairing slot: ",sourceSlot);
                    LogA(currentName);
                    LogA(targetName);

                    if(SetWheelButtonWeaponH57(button,buttonClass,getWeapon,baseTargets[k])) {
                        int m=g_h57BaseMappedCount++;
                        g_h57BaseMappedButtons[m]=button;
                        g_h57BaseOriginalWeapons[m]=weapon;

                        // H61 visual-only: DLC scene currently shows DLC sprites,
                        // so replace just the icon with the base-game sprite.
                        ApplySlotIconH61(button,buttonClass,sourceSlot,FALSE,TRUE,m);

                        enabled=TRUE;
                    }
                }
            }
        }

        void* ep[1]; ep[0]=&enabled;
        ex=0;
        g_il2cpp.runtime_invoke(enable,button,ep,&ex);
        if(!ex && enabled) ++baseEnabled;
    }

    LogIntA("[WHEEL H57] DLC->BASE variant slots swapped: ",g_h57BaseMappedCount);
    LogIntA("[WHEEL H57] total BASE wheel buttons enabled: ",baseEnabled);

    if(g_h57BaseMappedCount!=5 || baseEnabled!=9) {
        for(int i=0;i<g_h57BaseMappedCount;i++) {
            if(g_h61BaseIconButtons[i]) {
                RestoreExtraIconLayersH62(TRUE,i);
                RestoreButtonIconSpriteH61(g_h61BaseIconButtons[i],buttonClass,g_h61BaseOriginalSprites[i]);
            }
            SetWheelButtonWeaponH57(g_h57BaseMappedButtons[i],buttonClass,getWeapon,g_h57BaseOriginalWeapons[i]);
        }
        ClearBaseSwapStateH57();
        g_h51BaseFilterPending=0;
        LogA("[WHEEL H57] BASE mirror incomplete; temporary mappings rolled back.");
        return FALSE;
    }

    g_h57BaseSwapApplied=TRUE;
    g_h51BaseFilterPending=0;

    if(!g_h78AuditBaseInDlcDone) {
        AuditMappedButtonChildImagesH78(
            "[WHEEL H78] direction: DLC scene -> BASE wheel",
            g_h57BaseMappedButtons,
            g_h57BaseMappedCount,
            buttonClass,
            getWeapon);
        g_h78AuditBaseInDlcDone=TRUE;
    }

    LogA("[WHEEL H78] H77/H74 behavior preserved; visual audit only.");
    return TRUE;
}

static void ExecuteDelayedBaseFilterH51() {
    if(!g_ready || g_shuttingDown) return;
    BOOL attached=FALSE; void* thread=0;
    if(!EnsureIl2CppThread(&attached,&thread)) return;
    ApplyDelayedBaseFilterH51();
    ReleaseIl2CppThread(attached,thread);
}

static BOOL HideNativeBaseWeaponWheel() {
    if (!g_baseWheelActive) return TRUE;

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelViewClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerView","PlayerWheelView") : 0;
    if (!wheelClass || !wheelViewClass) {
        g_baseWheelActive=0;
        g_h51BaseFilterPending=0;
        g_h51BaseUsesInputEvent=FALSE;
        return FALSE;
    }

    const MethodInfo* hideMethod=g_il2cpp.class_get_method_from_name(wheelViewClass,"Hide",0);
    const MethodInfo* restoreMethod=g_il2cpp.class_get_method_from_name(wheelClass,"EnableButtons",0);
    Il2CppObject* wheel=FindLiveObject(wheelClass);
    Il2CppObject* view=FindLiveObject(wheelViewClass);

    BOOL ok=TRUE;
    Il2CppException* ex=0;

    if(g_h51BaseUsesInputEvent) {
        // Keep the temporary H57 mapping alive through native OnWheelUp so the
        // selected sector resolves to the intended base WeaponId.
        if(!InvokeInputManagerWheelEventFastH51(FALSE)) ok=FALSE;
    } else {
        if(view && hideMethod) {
            g_il2cpp.runtime_invoke(hideMethod,view,0,&ex);
            if(ex) { FormatExceptionToLog(ex); ok=FALSE; }
        } else ok=FALSE;
    }

    if(g_h57BaseSwapApplied) {
        if(!RestoreBaseSwapH57()) ok=FALSE;
        LogA("[WHEEL H57] Temporary DLC->BASE mappings restored after wheel close.");
    }

    ex=0;
    if(wheel && restoreMethod) {
        g_il2cpp.runtime_invoke(restoreMethod,wheel,0,&ex);
        if(ex) { FormatExceptionToLog(ex); ok=FALSE; }
    } else ok=FALSE;

    g_h51BaseFilterPending=0;
    g_h51BaseUsesInputEvent=FALSE;
    g_baseWheelActive=0;
    LogA("[WHEEL H51] Base wheel closed; native buttons restored.");
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

static BOOL g_h49InputWheelActive=FALSE;

static BOOL InspectNativeWheelCategoriesH49(Il2CppObject* wheel,
                                             const FieldInfo* buttonsField,
                                             const MethodInfo* buttonGetWeapon,
                                             const MethodInfo* getCategory,
                                             int* outBase,int* outDlc,int* outOther) {
    if(outBase) *outBase=0;
    if(outDlc) *outDlc=0;
    if(outOther) *outOther=0;
    if(!wheel || !buttonsField || !buttonGetWeapon || !getCategory) return FALSE;

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if(!collection) return FALSE;

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if(!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) return FALSE;

    int baseCount=0,dlcCount=0,otherCount=0;
    for(int i=0;i<count;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if(!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(buttonGetWeapon,button,0,&ex);
        if(ex || !weapon) continue;

        ex=0;
        Il2CppObject* catBox=g_il2cpp.runtime_invoke(getCategory,weapon,0,&ex);
        if(ex || !catBox) continue;

        void* raw=g_il2cpp.object_unbox(catBox);
        if(!raw) continue;

        int cat=*(int*)raw;
        if(cat==0) ++baseCount;
        else if(cat==1) ++dlcCount;
        else ++otherCount;
    }

    if(outBase) *outBase=baseCount;
    if(outDlc) *outDlc=dlcCount;
    if(outOther) *outOther=otherCount;
    return TRUE;
}

static int ReadPlayerWheelEnabledH49() {
    Il2CppClass* playerClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","Player") : 0;
    if(!playerClass) return -1;

    Il2CppObject* player=FindLiveObject(playerClass);
    if(!player) return -1;

    const MethodInfo* getter=g_il2cpp.class_get_method_from_name(playerClass,"get_IsWheelEnabled",0);
    if(!getter) return -1;

    Il2CppException* ex=0;
    Il2CppObject* boxed=g_il2cpp.runtime_invoke(getter,player,0,&ex);
    if(ex || !boxed) {
        if(ex) FormatExceptionToLog(ex);
        return -1;
    }

    void* raw=g_il2cpp.object_unbox(boxed);
    if(!raw) return -1;
    return (*(BYTE*)raw) ? 1 : 0;
}


typedef unsigned int (*H49FieldGetFlagsFn)(const FieldInfo*);
typedef SIZE_T (*H49FieldGetOffsetFn)(const FieldInfo*);
typedef const Il2CppType* (*H49FieldGetTypeFn)(const FieldInfo*);
typedef void (*H49FieldStaticGetValueFn)(const FieldInfo*, void*);

static void LogInputFieldMetaH49(Il2CppClass* inputClass,const char* fieldName) {
    if(!inputClass || !fieldName) return;

    const FieldInfo* f=g_il2cpp.class_get_field_from_name(inputClass,fieldName);
    if(!f) {
        LogA("[WHEEL H49] InputManager field unresolved:");
        LogA(fieldName);
        return;
    }

    H49FieldGetFlagsFn fieldGetFlags=(H49FieldGetFlagsFn)Proc("il2cpp_field_get_flags");
    H49FieldGetOffsetFn fieldGetOffset=(H49FieldGetOffsetFn)Proc("il2cpp_field_get_offset");
    H49FieldGetTypeFn fieldGetType=(H49FieldGetTypeFn)Proc("il2cpp_field_get_type");

    LogA("[WHEEL H49 FIELD]");
    LogA(fieldName);

    unsigned int flags=fieldGetFlags ? fieldGetFlags(f) : 0;
    LogIntA("[WHEEL H49] field flags: ",(int)flags);
    LogIntA("[WHEEL H49] field static: ",(flags & 0x10U) ? 1 : 0);

    if(fieldGetOffset)
        LogIntA("[WHEEL H49] field offset: ",(int)fieldGetOffset(f));

    if(fieldGetType) {
        const Il2CppType* t=fieldGetType(f);
        Il2CppClass* tc=t ? g_il2cpp.class_from_type(t) : 0;
        if(tc) {
            const char* ns=g_il2cpp.class_get_namespace(tc);
            const char* cn=g_il2cpp.class_get_name(tc);
            LogA("[WHEEL H49] declared field type:");
            if(ns && ns[0]) LogA(ns);
            if(cn) LogA(cn);
        } else {
            LogA("[WHEEL H49] declared field type unresolved.");
        }
    }
}

static BOOL ReadInputEventObjectH49(Il2CppClass* inputClass,
                                    Il2CppObject* input,
                                    const char* fieldName,
                                    Il2CppObject** outObject) {
    if(outObject) *outObject=0;
    if(!inputClass || !fieldName || !outObject) return FALSE;

    const FieldInfo* f=g_il2cpp.class_get_field_from_name(inputClass,fieldName);
    if(!f) return FALSE;

    H49FieldGetFlagsFn fieldGetFlags=(H49FieldGetFlagsFn)Proc("il2cpp_field_get_flags");
    H49FieldStaticGetValueFn fieldStaticGetValue=(H49FieldStaticGetValueFn)Proc("il2cpp_field_static_get_value");
    if(!fieldGetFlags) {
        LogA("[WHEEL H49] il2cpp_field_get_flags unavailable.");
        return FALSE;
    }

    unsigned int flags=fieldGetFlags(f);
    BOOL isStatic=(flags & 0x10U) ? TRUE : FALSE;
    Il2CppObject* value=0;

    if(isStatic) {
        if(!fieldStaticGetValue) {
            LogA("[WHEEL H49] static wheel field detected but il2cpp_field_static_get_value unavailable.");
            return FALSE;
        }
        fieldStaticGetValue(f,&value);
        LogA("[WHEEL H49] wheel event backing field read via STATIC storage.");
    } else {
        if(!input) {
            LogA("[WHEEL H49] instance wheel field requires live InputManager object.");
            return FALSE;
        }
        g_il2cpp.field_get_value(input,f,&value);
        LogA("[WHEEL H49] wheel event backing field read via INSTANCE storage.");
    }

    *outObject=value;
    return TRUE;
}

static BOOL InvokeValidatedZeroArgEventH49(Il2CppClass* inputClass,
                                           Il2CppObject* input,
                                           const char* fieldName) {
    const FieldInfo* f=inputClass ? g_il2cpp.class_get_field_from_name(inputClass,fieldName) : 0;
    if(!f) return FALSE;

    H49FieldGetTypeFn fieldGetType=(H49FieldGetTypeFn)Proc("il2cpp_field_get_type");
    if(!fieldGetType) {
        LogA("[WHEEL H49] il2cpp_field_get_type unavailable.");
        return FALSE;
    }

    const Il2CppType* declaredType=fieldGetType(f);
    Il2CppClass* declaredClass=declaredType ? g_il2cpp.class_from_type(declaredType) : 0;
    if(!declaredClass) {
        LogA("[WHEEL H49] declared event type unresolved.");
        return FALSE;
    }

    const MethodInfo* declaredInvoke=g_il2cpp.class_get_method_from_name(declaredClass,"Invoke",0);
    const char* dns=g_il2cpp.class_get_namespace(declaredClass);
    const char* dcn=g_il2cpp.class_get_name(declaredClass);
    LogA("[WHEEL H49] declared event type:");
    if(dns && dns[0]) LogA(dns);
    if(dcn) LogA(dcn);
    LogIntA("[WHEEL H49] declared zero-arg Invoke present: ",declaredInvoke?1:0);
    if(!declaredInvoke) return FALSE;

    Il2CppObject* eventObject=0;
    if(!ReadInputEventObjectH49(inputClass,input,fieldName,&eventObject)) return FALSE;
    LogIntA("[WHEEL H49] event object non-null: ",eventObject?1:0);
    if(!eventObject) return FALSE;

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    if(!objectGetClass) return FALSE;

    Il2CppClass* runtimeClass=objectGetClass(eventObject);
    if(!runtimeClass) return FALSE;

    const char* rns=g_il2cpp.class_get_namespace(runtimeClass);
    const char* rcn=g_il2cpp.class_get_name(runtimeClass);
    LogA("[WHEEL H49] runtime event object type:");
    if(rns && rns[0]) LogA(rns);
    if(rcn) LogA(rcn);

    const MethodInfo* invoke=g_il2cpp.class_get_method_from_name(runtimeClass,"Invoke",0);
    LogIntA("[WHEEL H49] runtime zero-arg Invoke present: ",invoke?1:0);
    if(!invoke) return FALSE;

    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(invoke,eventObject,0,&ex);
    if(ex) {
        LogA("[WHEEL H49] event Invoke() threw:");
        FormatExceptionToLog(ex);
        return FALSE;
    }

    LogA("[WHEEL H49] event Invoke() completed.");
    return TRUE;
}

static BOOL InvokeInputManagerWheelEventH49(BOOL down) {
    Il2CppClass* inputClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","InputManager") : 0;
    if(!inputClass) {
        LogA("[ERROR] H49 InputManager class unresolved.");
        return FALSE;
    }

    Il2CppObject* input=FindLiveObject(inputClass);
    LogIntA("[WHEEL H49] live InputManager object found: ",input?1:0);

    // Exact metadata only. This is intentionally tiny and replaces the broad H45 audit.
    LogInputFieldMetaH49(inputClass,"OnWheelDown");
    LogInputFieldMetaH49(inputClass,"OnWheelUp");
    LogInputFieldMetaH49(inputClass,"_wheelId");
    LogInputFieldMetaH49(inputClass,"_wheelPreviousItemId");
    LogInputFieldMetaH49(inputClass,"_wheelNextItemId");

    const char* fieldName=down ? "OnWheelDown" : "OnWheelUp";

    int before=ReadPlayerWheelEnabledH49();
    LogIntA("[WHEEL H49] Player.IsWheelEnabled before event: ",before);

    BOOL ok=InvokeValidatedZeroArgEventH49(inputClass,input,fieldName);

    int after=ReadPlayerWheelEnabledH49();
    LogIntA("[WHEEL H49] Player.IsWheelEnabled after event: ",after);
    return ok;
}


static const FieldInfo* g_h51WheelDownField=0;
static const FieldInfo* g_h51WheelUpField=0;
static H49FieldStaticGetValueFn g_h51StaticGetValue=0;

static BOOL ResolveFastWheelEventsH51() {
    if(g_h51WheelDownField && g_h51WheelUpField && g_h51StaticGetValue) return TRUE;

    Il2CppClass* inputClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","InputManager") : 0;
    if(!inputClass) return FALSE;

    g_h51WheelDownField=g_il2cpp.class_get_field_from_name(inputClass,"OnWheelDown");
    g_h51WheelUpField=g_il2cpp.class_get_field_from_name(inputClass,"OnWheelUp");
    g_h51StaticGetValue=(H49FieldStaticGetValueFn)Proc("il2cpp_field_static_get_value");

    return g_h51WheelDownField && g_h51WheelUpField && g_h51StaticGetValue;
}

static BOOL InvokeInputManagerWheelEventFastH51(BOOL down) {
    if(!ResolveFastWheelEventsH51()) {
        LogA("[WHEEL H51] Fast InputManager wheel-event bridge unresolved.");
        return FALSE;
    }

    const FieldInfo* field=down ? g_h51WheelDownField : g_h51WheelUpField;
    Il2CppObject* action=0;
    g_h51StaticGetValue(field,&action);
    if(!action) {
        LogA(down ? "[WHEEL H51] OnWheelDown Action is NULL." : "[WHEEL H51] OnWheelUp Action is NULL.");
        return FALSE;
    }

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    if(!objectGetClass) return FALSE;

    Il2CppClass* actionClass=objectGetClass(action);
    if(!actionClass) return FALSE;
    const MethodInfo* invoke=g_il2cpp.class_get_method_from_name(actionClass,"Invoke",0);
    if(!invoke) return FALSE;

    Il2CppException* ex=0;
    g_il2cpp.runtime_invoke(invoke,action,0,&ex);
    if(ex) {
        FormatExceptionToLog(ex);
        return FALSE;
    }
    return TRUE;
}


static volatile LONG g_h54DlcSwapPending=0;
static BOOL g_h54DlcSwapApplied=FALSE;
static BOOL g_h54ButtonMetaLogged=FALSE;
static Il2CppObject* g_h54MappedButtons[5]={0,0,0,0,0};
static Il2CppObject* g_h54OriginalWeapons[5]={0,0,0,0,0};
static int g_h54MappedCount=0;

static const char* g_h54BaseNames[5]={
    "WEAPON_Pistol",
    "WEAPON_Shovel",
    "WEAPON_CatCanon",
    "WEAPON_Shotgun",
    "WEAPON_DildoBow"
};

static const char* g_h54DlcNames[5]={
    "WEAPON_UmDrill",
    "WEAPON_PissGun",
    "WEAPON_MeatShotgun",
    "WEAPON_BubbleGumMachineGun",
    "WEAPON_NuclearSyringe"
};

static void ClearDlcSwapStateH54() {
    for(int i=0;i<5;i++) {
        g_h54MappedButtons[i]=0;
        g_h54OriginalWeapons[i]=0;
    }
    g_h54MappedCount=0;
    ClearDlcVisualStateH61();
}

static BOOL GetWeaponNameH54(Il2CppObject* weapon,char* out,int cap) {
    if(!weapon || !out || cap<=1) return FALSE;
    out[0]=0;

    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object") : 0;
    const MethodInfo* getName=unityObjectClass ? g_il2cpp.class_get_method_from_name(unityObjectClass,"get_name",0) : 0;
    if(!getName) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,weapon,0,&ex);
    if(ex || !nameObj) return FALSE;

    CatalogManagedStringToAscii((Il2CppString*)nameObj,out,cap);
    return out[0]!=0;
}

static int FindExactDlcTargetsH54(Il2CppObject* out[5]) {
    return CollectCanonicalWeaponsBySlotsH59(1,out);
}

static int EnsureDlcTargetsCollectedH54(Il2CppObject* targets[5]) {
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    Il2CppClass* inventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerInventoryComponent") : 0;
    Il2CppClass* weaponsInventoryClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.Inventory","WeaponsInventory") : 0;
    if(!weaponIdClass || !inventoryClass || !weaponsInventoryClass) return 0;

    const FieldInfo* controllerField=g_il2cpp.class_get_field_from_name(inventoryClass,"_weaponsController");
    const MethodInfo* addWeapon=FindMethodExact(weaponsInventoryClass,"AddWeapon",1,weaponIdClass);
    if(!controllerField || !addWeapon) return 0;

    Il2CppObject* playerInv=FindLiveObject(inventoryClass);
    if(!playerInv) return 0;

    Il2CppObject* weaponsInv=0;
    g_il2cpp.field_get_value(playerInv,controllerField,&weaponsInv);
    if(!weaponsInv) return 0;

    int ready=0;
    for(int i=0;i<5;i++) {
        Il2CppObject* weapon=targets[i];
        if(!weapon) continue;

        int collected=IsWeaponCollected(playerInv,inventoryClass,weapon);
        if(collected==1) {
            ++ready;
            continue;
        }
        if(collected<0) continue;

        void* wp[1]; wp[0]=weapon;
        Il2CppException* ex=0;
        g_il2cpp.runtime_invoke(addWeapon,weaponsInv,wp,&ex);
        if(!ex) ++ready;
    }
    return ready;
}

static BOOL SetWheelButtonWeaponH54(Il2CppObject* button,
                                    Il2CppClass* buttonClass,
                                    const MethodInfo* getWeapon,
                                    Il2CppObject* weapon) {
    if(!button || !buttonClass || !getWeapon || !weapon) return FALSE;

    const FieldInfo* field=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponId");
    if(!field) return FALSE;

    typedef void (*FnSetObject)(Il2CppObject*,const FieldInfo*,Il2CppObject*);
    FnSetObject setObject=(FnSetObject)Proc("il2cpp_field_set_value_object");
    if(!setObject) return FALSE;

    setObject(button,field,weapon);

    Il2CppException* ex=0;
    Il2CppObject* verify=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
    return !ex && verify==weapon;
}

static void LogWeaponWheelButtonMetaH54() {
    if(g_h54ButtonMetaLogged) return;
    g_h54ButtonMetaLogged=TRUE;

    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    if(!buttonClass) return;

    LogA("[WHEEL H54] ===== WeaponWheelButton metadata begin =====");

    void* mi=0;
    const MethodInfo* m=0;
    int mc=0;
    while((m=g_il2cpp.class_get_methods(buttonClass,&mi))!=0 && mc<96) {
        const char* n=g_il2cpp.method_get_name(m);
        if(!n) continue;
        LogA("[WHEEL H54 METHOD]");
        LogA(n);
        LogIntA("[WHEEL H54] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
        ++mc;
    }

    void* fi=0;
    const FieldInfo* f=0;
    int fc=0;
    while((f=g_il2cpp.class_get_fields(buttonClass,&fi))!=0 && fc<96) {
        const char* n=g_il2cpp.field_get_name(f);
        if(!n) continue;
        LogA("[WHEEL H54 FIELD]");
        LogA(n);
        ++fc;
    }

    LogA("[WHEEL H54] ===== WeaponWheelButton metadata end =====");
}


static BOOL g_h55VisualAuditLogged=FALSE;

static void LogUnityObjectNameH55(Il2CppObject* obj,const char* label) {
    if(label) LogA(label);
    if(!obj) {
        LogA("[WHEEL H55] <NULL>");
        return;
    }

    Il2CppClass* unityObjectClass=g_coreImage ? g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object") : 0;
    const MethodInfo* getName=unityObjectClass ? g_il2cpp.class_get_method_from_name(unityObjectClass,"get_name",0) : 0;
    if(!getName) {
        LogA("[WHEEL H55] UnityEngine.Object.get_name unavailable.");
        return;
    }

    Il2CppException* ex=0;
    Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,obj,0,&ex);
    if(ex || !nameObj) {
        if(ex) FormatExceptionToLog(ex);
        LogA("[WHEEL H55] object name unavailable.");
        return;
    }

    char name[256]; name[0]=0;
    CatalogManagedStringToAscii((Il2CppString*)nameObj,name,256);
    LogA(name);
}

static void LogImageSpriteH55(Il2CppObject* imageObj,const char* label) {
    if(label) LogA(label);
    if(!imageObj) {
        LogA("[WHEEL H55] Image object is NULL.");
        return;
    }

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    if(!objectGetClass) return;

    Il2CppClass* imageClass=objectGetClass(imageObj);
    if(!imageClass) return;

    const char* ns=g_il2cpp.class_get_namespace(imageClass);
    const char* cn=g_il2cpp.class_get_name(imageClass);
    LogA("[WHEEL H55] Image runtime type:");
    if(ns && ns[0]) LogA(ns);
    if(cn) LogA(cn);

    const MethodInfo* getSprite=g_il2cpp.class_get_method_from_name(imageClass,"get_sprite",0);
    if(!getSprite) {
        LogA("[WHEEL H55] get_sprite() unresolved on image runtime type.");
        return;
    }

    Il2CppException* ex=0;
    Il2CppObject* sprite=g_il2cpp.runtime_invoke(getSprite,imageObj,0,&ex);
    if(ex) {
        FormatExceptionToLog(ex);
        return;
    }

    LogUnityObjectNameH55(sprite,"[WHEEL H55] sprite name:");
}


static BOOL GetUnityObjectNameH78(Il2CppObject* obj,char* out,int cap) {
    if(out && cap>0) out[0]=0;
    if(!obj || !out || cap<=1 || !g_coreImage) return FALSE;

    Il2CppClass* objectClass=g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Object");
    const MethodInfo* getName=objectClass ? g_il2cpp.class_get_method_from_name(objectClass,"get_name",0) : 0;
    if(!getName) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* nameObj=g_il2cpp.runtime_invoke(getName,obj,0,&ex);
    if(ex || !nameObj) return FALSE;

    CatalogManagedStringToAscii((Il2CppString*)nameObj,out,cap);
    return out[0]!=0;
}

static BOOL GetImageSpriteH78(Il2CppObject* image,Il2CppObject** outSprite) {
    if(outSprite) *outSprite=0;
    if(!image || !outSprite) return FALSE;

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    Il2CppClass* imageClass=objectGetClass ? objectGetClass(image) : 0;
    if(!imageClass) return FALSE;

    const MethodInfo* getSprite=g_il2cpp.class_get_method_from_name(imageClass,"get_sprite",0);
    if(!getSprite) return FALSE;

    Il2CppException* ex=0;
    Il2CppObject* sprite=g_il2cpp.runtime_invoke(getSprite,image,0,&ex);
    if(ex) return FALSE;

    *outSprite=sprite;
    return TRUE;
}

static int GetImageEnabledH78(Il2CppObject* image) {
    if(!image) return -1;

    typedef Il2CppClass* (*FnObjectGetClass)(Il2CppObject*);
    FnObjectGetClass objectGetClass=(FnObjectGetClass)Proc("il2cpp_object_get_class");
    Il2CppClass* imageClass=objectGetClass ? objectGetClass(image) : 0;
    if(!imageClass) return -1;

    const MethodInfo* getEnabled=g_il2cpp.class_get_method_from_name(imageClass,"get_enabled",0);
    if(!getEnabled) return -1;

    Il2CppException* ex=0;
    Il2CppObject* box=g_il2cpp.runtime_invoke(getEnabled,image,0,&ex);
    if(ex || !box) return -1;
    void* raw=g_il2cpp.object_unbox(box);
    if(!raw) return -1;
    return (*(BYTE*)raw) ? 1 : 0;
}

static void AuditMappedButtonChildImagesH78(const char* direction,
                                             Il2CppObject** buttons,
                                             int count,
                                             Il2CppClass* buttonClass,
                                             const MethodInfo* getWeapon) {
    if(!buttons || count<=0 || !buttonClass || !getWeapon) return;
    if(!g_componentGetGameObject || !g_gameObjectGetComponentsInChildren || !g_uiImage) {
        LogA("[WHEEL H78] child Image audit bridge unavailable.");
        return;
    }

    Il2CppClass* imageClass=g_il2cpp.class_from_name(g_uiImage,"UnityEngine.UI","Image");
    if(!imageClass) {
        LogA("[WHEEL H78] UnityEngine.UI.Image unresolved.");
        return;
    }

    const Il2CppType* it=g_il2cpp.class_get_type(imageClass);
    Il2CppObject* imageTypeObj=it ? g_il2cpp.type_get_object(it) : 0;
    if(!imageTypeObj) return;

    const FieldInfo* primaryField=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponSelectedImage");
    const FieldInfo* backgroundField=g_il2cpp.class_get_field_from_name(buttonClass,"_backgroundImage");

    LogA("[WHEEL H78] ============================================================");
    LogA("[WHEEL H78] CROSS-MODE CHILD IMAGE AUDIT");
    if(direction) LogA(direction);

    for(int bi=0; bi<count && bi<5; ++bi) {
        Il2CppObject* button=buttons[bi];
        if(!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
        char weaponName[128]; weaponName[0]=0;
        if(!ex && weapon) GetWeaponNameH54(weapon,weaponName,128);

        Il2CppObject* primaryImage=0;
        Il2CppObject* backgroundImage=0;
        if(primaryField) g_il2cpp.field_get_value(button,primaryField,&primaryImage);
        if(backgroundField) g_il2cpp.field_get_value(button,backgroundField,&backgroundImage);

        LogA("[WHEEL H78] ------------------------------------------------------------");
        LogIntA("[WHEEL H78] mapped button index: ",bi);
        LogA("[WHEEL H78] current WeaponId:");
        LogA(weaponName[0] ? weaponName : "<unknown>");

        ex=0;
        Il2CppObject* go=g_il2cpp.runtime_invoke(g_componentGetGameObject,button,0,&ex);
        if(ex || !go) {
            if(ex) FormatExceptionToLog(ex);
            continue;
        }

        BOOL includeInactive=TRUE;
        void* pars[2]; pars[0]=imageTypeObj; pars[1]=&includeInactive;
        ex=0;
        Il2CppObject* arrObj=g_il2cpp.runtime_invoke(g_gameObjectGetComponentsInChildren,go,pars,&ex);
        if(ex || !arrObj) {
            if(ex) FormatExceptionToLog(ex);
            continue;
        }

        Il2CppArray* arr=(Il2CppArray*)arrObj;
        SIZE_T n=g_il2cpp.array_length(arr);
        if(n>32) n=32;
        LogIntA("[WHEEL H78] child Image count: ",(int)n);

        unsigned int hdr=g_il2cpp.array_object_header_size();
        Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);

        for(SIZE_T ii=0; ii<n; ++ii) {
            Il2CppObject* image=elems[ii];
            if(!image) continue;

            Il2CppObject* sprite=0;
            GetImageSpriteH78(image,&sprite);

            char spriteName[192]; spriteName[0]=0;
            char objectName[192]; objectName[0]=0;
            if(sprite) GetUnityObjectNameH78(sprite,spriteName,192);

            Il2CppObject* imageGo=0;
            ex=0;
            imageGo=g_il2cpp.runtime_invoke(g_componentGetGameObject,image,0,&ex);
            if(!ex && imageGo) GetUnityObjectNameH78(imageGo,objectName,192);

            LogIntA("[WHEEL H78] Image index: ",(int)ii);
            LogA("[WHEEL H78] GameObject:");
            LogA(objectName[0] ? objectName : "<unnamed>");
            LogA("[WHEEL H78] sprite:");
            LogA(spriteName[0] ? spriteName : "<null>");
            LogIntA("[WHEEL H78] enabled: ",GetImageEnabledH78(image));

            if(image==primaryImage) LogA("[WHEEL H78] role: _weaponSelectedImage");
            else if(image==backgroundImage) LogA("[WHEEL H78] role: _backgroundImage");
            else LogA("[WHEEL H78] role: OTHER");
        }
    }

    LogA("[WHEEL H78] ============================================================");
}

static BOOL IsVisualTypeNameH55(const char* ns,const char* cn) {
    if(!cn) return FALSE;
    if(StrContainsNoCaseA(cn,"Sprite")) return TRUE;
    if(StrContainsNoCaseA(cn,"Texture")) return TRUE;
    if(StrContainsNoCaseA(cn,"Image")) return TRUE;
    if(StrContainsNoCaseA(cn,"GameObject")) return TRUE;
    if(ns && StrContainsNoCaseA(ns,"UnityEngine") && StrContainsNoCaseA(cn,"Object")) return TRUE;
    return FALSE;
}

static void LogWeaponIdVisualMetadataH55(Il2CppObject* weapon) {
    if(!weapon) return;

    Il2CppClass* weaponClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if(!weaponClass) return;

    H49FieldGetTypeFn fieldGetType=(H49FieldGetTypeFn)Proc("il2cpp_field_get_type");
    if(!fieldGetType) return;

    char weaponName[128]; weaponName[0]=0;
    GetWeaponNameH54(weapon,weaponName,128);
    LogA("[WHEEL H55] ===== WeaponId visual inspection =====");
    LogA(weaponName);

    void* mi=0;
    const MethodInfo* m=0;
    int mc=0;
    while((m=g_il2cpp.class_get_methods(weaponClass,&mi))!=0 && mc<128) {
        const char* mn=g_il2cpp.method_get_name(m);
        if(!mn) continue;
        if(StrContainsNoCaseA(mn,"icon") ||
           StrContainsNoCaseA(mn,"sprite") ||
           StrContainsNoCaseA(mn,"image") ||
           StrContainsNoCaseA(mn,"visual") ||
           StrContainsNoCaseA(mn,"texture")) {
            LogA("[WHEEL H55 WEAPON METHOD]");
            LogA(mn);
            LogIntA("[WHEEL H55] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
        }
        ++mc;
    }

    void* fi=0;
    const FieldInfo* f=0;
    int fc=0;
    while((f=g_il2cpp.class_get_fields(weaponClass,&fi))!=0 && fc<128) {
        const char* fn=g_il2cpp.field_get_name(f);
        if(!fn) continue;

        const Il2CppType* ft=fieldGetType(f);
        Il2CppClass* fcType=ft ? g_il2cpp.class_from_type(ft) : 0;
        const char* fns=fcType ? g_il2cpp.class_get_namespace(fcType) : 0;
        const char* fcn=fcType ? g_il2cpp.class_get_name(fcType) : 0;

        LogA("[WHEEL H55 WEAPON FIELD]");
        LogA(fn);
        if(fns && fns[0]) LogA(fns);
        if(fcn) LogA(fcn);

        if(fcType && IsVisualTypeNameH55(fns,fcn)) {
            Il2CppObject* visual=0;
            g_il2cpp.field_get_value(weapon,f,&visual);
            LogUnityObjectNameH55(visual,"[WHEEL H55] visual field object name:");
        }
        ++fc;
    }
}

static void LogMappedButtonVisualsH55(Il2CppClass* buttonClass,const MethodInfo* getWeapon) {
    if(g_h55VisualAuditLogged || !buttonClass || !getWeapon) return;
    g_h55VisualAuditLogged=TRUE;

    const FieldInfo* selectedField=g_il2cpp.class_get_field_from_name(buttonClass,"_weaponSelectedImage");
    const FieldInfo* backgroundField=g_il2cpp.class_get_field_from_name(buttonClass,"_backgroundImage");

    LogA("[WHEEL H55] ===== mapped button visual audit begin =====");

    for(int i=0;i<g_h54MappedCount;i++) {
        Il2CppObject* button=g_h54MappedButtons[i];
        if(!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* weapon=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
        if(ex || !weapon) continue;

        char weaponName[128]; weaponName[0]=0;
        GetWeaponNameH54(weapon,weaponName,128);
        LogIntA("[WHEEL H55] mapped index: ",i);
        LogA("[WHEEL H55] current WeaponId:");
        LogA(weaponName);

        Il2CppObject* selectedImage=0;
        Il2CppObject* backgroundImage=0;
        if(selectedField) g_il2cpp.field_get_value(button,selectedField,&selectedImage);
        if(backgroundField) g_il2cpp.field_get_value(button,backgroundField,&backgroundImage);

        LogImageSpriteH55(selectedImage,"[WHEEL H55] _weaponSelectedImage:");
        LogImageSpriteH55(backgroundImage,"[WHEEL H55] _backgroundImage:");

        LogWeaponIdVisualMetadataH55(weapon);
    }

    LogA("[WHEEL H55] ===== mapped button visual audit end =====");
}

static BOOL ApplyDlcSwapAfterOpenH54() {
    if(!g_h54DlcSwapPending || !g_dlcWheelActive) return TRUE;
    if(ReadPlayerWheelEnabledH49()!=1) return FALSE;

    Il2CppObject* targets[5]={0,0,0,0,0};
    int found=FindExactDlcTargetsH54(targets);
    LogIntA("[WHEEL H54] exact DLC targets found: ",found);
    if(found!=5) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    int ready=EnsureDlcTargetsCollectedH54(targets);
    LogIntA("[WHEEL H54] DLC targets collected/ready: ",ready);
    if(ready!=5) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    if(!wheelClass || !buttonClass) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* getWeapon=g_il2cpp.class_get_method_from_name(buttonClass,"get_WeaponId",0);
    const MethodInfo* enable=g_il2cpp.class_get_method_from_name(buttonClass,"Enable",1);
    if(!buttonsField || !getWeapon || !enable) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    if(!wheel) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    Il2CppObject* collection=0;
    g_il2cpp.field_get_value(wheel,buttonsField,&collection);
    if(!collection) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    Il2CppClass* collectionClass=0;
    int count=0;
    BOOL isArray=FALSE;
    if(!GetBaseWheelButtonsCollection(collection,&collectionClass,&count,&isArray)) {
        g_h54DlcSwapPending=0;
        return FALSE;
    }

    ClearDlcSwapStateH54();

    for(int i=0;i<count && g_h54MappedCount<5;i++) {
        Il2CppObject* button=GetBaseWheelButtonFromCollection(collection,collectionClass,isArray,i);
        if(!button) continue;

        Il2CppException* ex=0;
        Il2CppObject* original=g_il2cpp.runtime_invoke(getWeapon,button,0,&ex);
        if(ex || !original) continue;

        char currentName[128]; currentName[0]=0;
        if(!GetWeaponNameH54(original,currentName,128)) continue;

        int sourceSlot=-1;
        if(GetWeaponSlotH59(original,&sourceSlot)) {
            int k=SlotIndexH59(sourceSlot);
            if(k>=0 && targets[k]) {
                char targetName[128]; targetName[0]=0;
                GetWeaponNameH54(targets[k],targetName,128);
                LogIntA("[WHEEL H59] BASE->DLC pairing slot: ",sourceSlot);
                LogA(currentName);
                LogA(targetName);

                if(SetWheelButtonWeaponH54(button,buttonClass,getWeapon,targets[k])) {
                    int m=g_h54MappedCount++;
                    g_h54MappedButtons[m]=button;
                    g_h54OriginalWeapons[m]=original;

                    // H61 visual-only: normal scene currently shows base sprites,
                    // so replace just the icon with the DLC sprite.
                    ApplySlotIconH61(button,buttonClass,sourceSlot,TRUE,FALSE,m);

                    BOOL enabled=TRUE;
                    void* ep[1]; ep[0]=&enabled;
                    ex=0;
                    g_il2cpp.runtime_invoke(enable,button,ep,&ex);
                }
            }
        }
    }

    LogIntA("[WHEEL H54] BASE->DLC slots swapped AFTER native open: ",g_h54MappedCount);

    if(g_h54MappedCount!=5) {
        for(int i=0;i<g_h54MappedCount;i++) {
            if(g_h61DlcIconButtons[i]) {
                RestoreExtraIconLayersH62(FALSE,i);
                RestoreButtonIconSpriteH61(g_h61DlcIconButtons[i],buttonClass,g_h61DlcOriginalSprites[i]);
            }
            SetWheelButtonWeaponH54(g_h54MappedButtons[i],buttonClass,getWeapon,g_h54OriginalWeapons[i]);
        }
        ClearDlcSwapStateH54();
        g_h54DlcSwapPending=0;
        LogA("[WHEEL H57] DLC mirror incomplete; temporary mappings rolled back.");
        return FALSE;
    }

    // H57 intentionally does NOT call PlayerWeaponWheelComponent.EnableButtons()
    // here. H54 manually enabled all five remapped sectors already; the global
    // native refresh is scene/category-aware and can immediately hide them again.

    g_h54DlcSwapApplied=TRUE;
    g_h54DlcSwapPending=0;

    if(!g_h78AuditDlcInBaseDone) {
        AuditMappedButtonChildImagesH78(
            "[WHEEL H78] direction: NORMAL scene -> DLC wheel",
            g_h54MappedButtons,
            g_h54MappedCount,
            buttonClass,
            getWeapon);
        g_h78AuditDlcInBaseDone=TRUE;
    }

    LogA("[WHEEL H78] H77/H74 behavior preserved; visual audit only.");
    return TRUE;
}

static void ExecuteDelayedDlcSwapH54() {
    if(!g_ready || g_shuttingDown) return;
    BOOL attached=FALSE; void* thread=0;
    if(!EnsureIl2CppThread(&attached,&thread)) return;
    ApplyDlcSwapAfterOpenH54();
    ReleaseIl2CppThread(attached,thread);
}

static BOOL RestoreDlcSwapH54() {
    if(!g_h54DlcSwapApplied || g_h54MappedCount<=0) {
        ClearDlcSwapStateH54();
        g_h54DlcSwapApplied=FALSE;
        return TRUE;
    }

    Il2CppClass* buttonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(
        g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    if(!buttonClass) return FALSE;

    const MethodInfo* getWeapon=g_il2cpp.class_get_method_from_name(buttonClass,"get_WeaponId",0);
    if(!getWeapon) return FALSE;

    BOOL ok=TRUE;
    for(int i=0;i<g_h54MappedCount;i++) {
        if(g_h61DlcIconButtons[i]) {
            if(!RestoreExtraIconLayersH62(FALSE,i)) ok=FALSE;
            if(!RestoreButtonIconSpriteH61(g_h61DlcIconButtons[i],buttonClass,g_h61DlcOriginalSprites[i]))
                ok=FALSE;
        }

        if(!SetWheelButtonWeaponH54(g_h54MappedButtons[i],buttonClass,getWeapon,g_h54OriginalWeapons[i]))
            ok=FALSE;
    }

    ClearDlcSwapStateH54();
    g_h54DlcSwapApplied=FALSE;
    return ok;
}

static BOOL ShowNativeDlcWeaponWheel() {
    if(!IsTheseSunnyDazeOwned()) {
        LogA("[WHEEL H54] DLC wheel blocked: These Sunny Daze ownership not confirmed.");
        return FALSE;
    }

    Il2CppClass* wheelClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents","PlayerWeaponWheelComponent") : 0;
    Il2CppClass* wheelButtonClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD.PlayerComponents.WeaponWheel","WeaponWheelButton") : 0;
    Il2CppClass* weaponIdClass=g_assemblyCSharp ? g_il2cpp.class_from_name(g_assemblyCSharp,"Hyperstrange.PBD","WeaponId") : 0;
    if(!wheelClass || !wheelButtonClass || !weaponIdClass) return FALSE;

    const FieldInfo* buttonsField=g_il2cpp.class_get_field_from_name(wheelClass,"_weaponWheelButtons");
    const MethodInfo* buttonGetWeapon=g_il2cpp.class_get_method_from_name(wheelButtonClass,"get_WeaponId",0);
    const MethodInfo* getCategory=g_il2cpp.class_get_method_from_name(weaponIdClass,"get_Category",0);
    if(!buttonsField || !buttonGetWeapon || !getCategory) return FALSE;

    Il2CppObject* wheel=FindLiveObject(wheelClass);
    if(!wheel) return FALSE;

    int baseCount=0,dlcCount=0,otherCount=0;
    InspectNativeWheelCategoriesH49(wheel,buttonsField,buttonGetWeapon,getCategory,&baseCount,&dlcCount,&otherCount);

    AuditCurrentWheelLayoutH56(
        (dlcCount>0) ? "[WHEEL H56] DLC SCENE native layout" : "[WHEEL H56] NORMAL SCENE native layout",
        baseCount,dlcCount);

    if(!InvokeInputManagerWheelEventFastH51(TRUE)) return FALSE;

    g_h54DlcSwapPending=(dlcCount==0) ? 1 : 0;
    g_h54DlcSwapApplied=FALSE;
    g_h49InputWheelActive=TRUE;
    g_dlcWheelActive=1;

    LogA(dlcCount==0
         ? "[WHEEL H54] Normal campaign wheel opened; DLC-only post-event swap pending."
         : "[WHEEL H54] Native DLC wheel opened unchanged inside DLC.");
    return TRUE;
}

static BOOL HideNativeDlcWeaponWheel() {
    if(!g_dlcWheelActive || !g_h49InputWheelActive) return TRUE;

    BOOL ok=InvokeInputManagerWheelEventFastH51(FALSE);

    if(g_h54DlcSwapApplied) {
        if(!RestoreDlcSwapH54()) ok=FALSE;
        LogA("[WHEEL H54] Temporary normal-campaign DLC mapping restored.");
    }

    g_h54DlcSwapPending=0;
    g_h49InputWheelActive=FALSE;
    g_dlcWheelActive=0;
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

static void LogTargetClassMetadataH13(Il2CppClass* klass, const char* label) {
    if (!klass) {
        if (label) {
            LogA(label);
            LogA("[H13 META] class unresolved");
        }
        return;
    }

    if (label) LogA(label);

    void* mi=0;
    const MethodInfo* m=0;
    int mc=0;
    while ((m=g_il2cpp.class_get_methods(klass,&mi))!=0 && mc<96) {
        const char* n=g_il2cpp.method_get_name(m);
        if (!n) continue;
        LogA("[H13 META METHOD]");
        LogA(n);
        LogIntA("[H13 META] parameter count: ",(int)g_il2cpp.method_get_param_count(m));
        ++mc;
    }
    LogIntA("[H13 META] methods logged: ",mc);

    void* fi=0;
    const FieldInfo* fld=0;
    int fc=0;
    while ((fld=g_il2cpp.class_get_fields(klass,&fi))!=0 && fc<96) {
        const char* n=g_il2cpp.field_get_name(fld);
        if (!n) continue;
        LogA("[H13 META FIELD]");
        LogA(n);
        ++fc;
    }
    LogIntA("[H13 META] fields logged: ",fc);
}

static void RunTargetedNoVideoAndCrosshairAuditH13() {
    LogA("[H13 META] ===== targeted DebugManager / NO_VIDEO audit =====");
    Il2CppClass* debugManager=FindClassBySimpleName("DebugManager");
    Il2CppClass* noVideoClass=FindClassBySimpleName("NO_VIDEO");
    LogTargetClassMetadataH13(debugManager,"[H13 META] DebugManager");
    LogTargetClassMetadataH13(noVideoClass,"[H13 META] NO_VIDEO class");
    LogA("[H13 META] ===== targeted PlayerCrosshairController audit =====");
    LogTargetClassMetadataH13(g_crosshairControllerClass,"[H13 META] PlayerCrosshairController");
    LogA("[H13 META] ===== audit end =====");
}

static BOOL SetCrosshairVisualAlpha(float alpha) {
    if (!g_crosshairControllerClass || !g_currentCrosshairField || !g_componentGetGameObject ||
        !g_gameObjectGetComponentsInChildren || !g_canvasRendererSetAlpha || !g_canvasRendererTypeObject) {
        LogA("[ERROR] No Crosshair CanvasRenderer bridge unresolved");
        return FALSE;
    }

    int controllerCount=0;
    int controllersWithCrosshair=0;
    int changed=0;

    // H13: DLC may load/retain more than one PlayerCrosshairController.
    // Prefer Resources.FindObjectsOfTypeAll(Type) so we do not silently target
    // only the first controller returned by FindObjectOfType.
    if (g_findObjectsOfTypeAll1) {
        const Il2CppType* ct=g_il2cpp.class_get_type(g_crosshairControllerClass);
        Il2CppObject* typeObj=ct ? g_il2cpp.type_get_object(ct) : 0;
        if (typeObj) {
            void* fp[1]; fp[0]=typeObj;
            Il2CppException* ex=0;
            Il2CppObject* arrObj=g_il2cpp.runtime_invoke(g_findObjectsOfTypeAll1,0,fp,&ex);
            if (ex) {
                LogA("[CROSSHAIR H13] Resources.FindObjectsOfTypeAll(PlayerCrosshairController) threw:");
                FormatExceptionToLog(ex);
            } else if (arrObj) {
                Il2CppArray* controllers=(Il2CppArray*)arrObj;
                SIZE_T count=g_il2cpp.array_length(controllers);
                if (count>64) count=64;
                unsigned int hdr=g_il2cpp.array_object_header_size();
                Il2CppObject** elems=(Il2CppObject**)(((BYTE*)controllers)+hdr);

                for (SIZE_T ci=0;ci<count;ci++) {
                    Il2CppObject* controller=elems[ci];
                    if (!controller) continue;
                    ++controllerCount;

                    Il2CppObject* crosshair=0;
                    g_il2cpp.field_get_value(controller,g_currentCrosshairField,&crosshair);
                    if (!crosshair) continue;
                    ++controllersWithCrosshair;

                    ex=0;
                    Il2CppObject* go=g_il2cpp.runtime_invoke(g_componentGetGameObject,crosshair,0,&ex);
                    if (ex || !go) {
                        if (ex) FormatExceptionToLog(ex);
                        continue;
                    }

                    BOOL includeInactive=TRUE;
                    void* pars[2]; pars[0]=g_canvasRendererTypeObject; pars[1]=&includeInactive;
                    ex=0;
                    Il2CppObject* boxedArr=g_il2cpp.runtime_invoke(g_gameObjectGetComponentsInChildren,go,pars,&ex);
                    if (ex || !boxedArr) {
                        if (ex) FormatExceptionToLog(ex);
                        continue;
                    }

                    Il2CppArray* arr=(Il2CppArray*)boxedArr;
                    SIZE_T n=g_il2cpp.array_length(arr);
                    unsigned int ah=g_il2cpp.array_object_header_size();
                    Il2CppObject** renderers=(Il2CppObject**)(((BYTE*)arr)+ah);
                    for (SIZE_T i=0;i<n;i++) {
                        Il2CppObject* renderer=renderers[i];
                        if (!renderer) continue;
                        void* alphaPars[1]; alphaPars[0]=&alpha;
                        ex=0;
                        g_il2cpp.runtime_invoke(g_canvasRendererSetAlpha,renderer,alphaPars,&ex);
                        if (ex) {
                            FormatExceptionToLog(ex);
                            continue;
                        }
                        ++changed;
                    }
                }
            }
        }
    }

    // Conservative fallback retains the validated H4 single-controller path.
    if (controllerCount==0) {
        Il2CppObject* controller=FindLiveObject(g_crosshairControllerClass);
        if (!controller) {
            LogA("[WARN] PlayerCrosshairController instance not found");
            return FALSE;
        }
        controllerCount=1;

        Il2CppObject* crosshair=0;
        g_il2cpp.field_get_value(controller,g_currentCrosshairField,&crosshair);
        if (!crosshair) {
            LogA("[WARN] Current Crosshair object unavailable");
            return FALSE;
        }
        controllersWithCrosshair=1;

        Il2CppException* ex=0;
        Il2CppObject* go=g_il2cpp.runtime_invoke(g_componentGetGameObject,crosshair,0,&ex);
        if (ex || !go) {
            if(ex) FormatExceptionToLog(ex);
            return FALSE;
        }

        BOOL includeInactive=TRUE;
        void* pars[2]; pars[0]=g_canvasRendererTypeObject; pars[1]=&includeInactive;
        ex=0;
        Il2CppObject* boxedArr=g_il2cpp.runtime_invoke(g_gameObjectGetComponentsInChildren,go,pars,&ex);
        if (ex || !boxedArr) {
            if(ex) FormatExceptionToLog(ex);
            return FALSE;
        }

        Il2CppArray* arr=(Il2CppArray*)boxedArr;
        SIZE_T n=g_il2cpp.array_length(arr);
        unsigned int hdr=g_il2cpp.array_object_header_size();
        Il2CppObject** elems=(Il2CppObject**)(((BYTE*)arr)+hdr);
        for (SIZE_T i=0;i<n;i++) {
            Il2CppObject* renderer=elems[i];
            if (!renderer) continue;
            void* alphaPars[1]; alphaPars[0]=&alpha;
            ex=0;
            g_il2cpp.runtime_invoke(g_canvasRendererSetAlpha,renderer,alphaPars,&ex);
            if (ex) {
                FormatExceptionToLog(ex);
                continue;
            }
            ++changed;
        }
    }

    LogIntA("[CROSSHAIR H13] PlayerCrosshairController objects seen: ",controllerCount);
    LogIntA("[CROSSHAIR H13] Controllers with current Crosshair: ",controllersWithCrosshair);
    LogIntA("[CROSSHAIR H13] CanvasRenderers alpha-updated: ",changed);

    if (!changed) {
        LogA("[WARN] No CanvasRenderer found under any current Crosshair");
        return FALSE;
    }
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

static BOOL ToggleSkipIntroVideos() {
    g_skipIntroVideos = g_skipIntroVideos ? 0 : 1;
    WritePrivateProfileStringW(L"Settings",L"SkipIntroVideos",g_skipIntroVideos?L"1":L"0",g_iniPath);
    WritePrivateProfileStringW(0,0,0,g_iniPath);
    LogA(g_skipIntroVideos
        ? "[INTRO H13] Skip Intro preference stored. Native NO_VIDEO mechanism is under targeted audit; no simulated input is used."
        : "[INTRO H13] Skip Intro preference disabled. No simulated input is used.");
    if (g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
    return TRUE;
}

static BOOL ToggleSkipStartupLogos() {
    g_skipStartupLogos = g_skipStartupLogos ? 0 : 1;
    WritePrivateProfileStringW(L"Settings",L"SkipStartupLogos",g_skipStartupLogos?L"1":L"0",g_iniPath);
    WritePrivateProfileStringW(0,0,0,g_iniPath);
    LogA(g_skipStartupLogos
        ? "[SPLASH H40] Skip Startup Logos ENABLED. H39 native CancelSplashScreen will apply on next launch."
        : "[SPLASH H40] Skip Startup Logos DISABLED. Native Unity startup logos will play on next launch.");
    if (g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
    return TRUE;
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

    const char* exactWeaponName=0;
    BOOL exactSpecial=FALSE;
    int listCategory=0, listSlot=0;
    if (DecodeNamedWeaponCommand(i,&exactWeaponName,&exactSpecial)) ExecuteNamedWeaponByUnityName(exactWeaponName,exactSpecial);
    else if (DecodeWeaponListCommand(i,&listCategory,&listSlot)) EquipMappedWeaponCategorySlot(listCategory,listSlot);
    else if (StrEqA(g_cmds[i].className,"__NoCrosshair")) ToggleNoCrosshair();
    else if (StrEqA(g_cmds[i].className,"__SkipIntroVideos")) ToggleSkipIntroVideos();
    else if (StrEqA(g_cmds[i].className,"__SkipStartupLogos")) ToggleSkipStartupLogos();
    else if (StrEqA(g_cmds[i].className,"__GiveAllWeapons")) GiveAllWeaponsOnly();
    else if (StrEqA(g_cmds[i].className,"__GiveAllDlcWeapons")) GiveAllOwnedDlcWeapons();
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
    LogA("[LAZY H77] User-triggered IL2CPP init; startup race delay skipped.");
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
    Il2CppClass* resourcesClass=g_il2cpp.class_from_name(g_coreImage,"UnityEngine","Resources");
    Il2CppClass* canvasRendererClass=g_uiImage ? g_il2cpp.class_from_name(g_uiImage,"UnityEngine","CanvasRenderer") : 0;
    g_findObjectOfType2=objectClass ? FindMethodExact(objectClass,"FindObjectOfType",2,g_systemTypeClass,g_boolClass) : 0;
    g_findObjectsOfTypeAll1=resourcesClass ? FindMethodExact(resourcesClass,"FindObjectsOfTypeAll",1,g_systemTypeClass) : 0;
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

    LogA("[LAZY H77] H13 metadata audit skipped during lazy init.");

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

static BOOL EnsureLazyIl2CppH77(const char* reason) {
    if(g_ready) return TRUE;
    if(g_shuttingDown) return FALSE;

    if(g_h77LazyInitState==1) {
        LogA("[LAZY H77] Initialization already in progress.");
        return FALSE;
    }
    if(g_h77LazyInitState==3) {
        LogA("[LAZY H77] Previous lazy initialization failed; not retrying in this session.");
        return FALSE;
    }

    g_h77LazyInitState=1;
    LogA("[LAZY H77] ========================================");
    LogA("[LAZY H77] Starting IL2CPP initialization on demand.");
    if(reason) LogA(reason);

    BOOL ok=InitIl2Cpp();
    if(ok) {
        g_h77LazyInitState=2;
        LogA("[LAZY H77] IL2CPP READY. Gameplay commands/hotkeys are now enabled.");
        if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
        return TRUE;
    }

    g_h77LazyInitState=3;
    LogA("[LAZY H77] IL2CPP initialization FAILED.");
    if(g_menuHwnd) InvalidateRect(g_menuHwnd,0,TRUE);
    return FALSE;
}

