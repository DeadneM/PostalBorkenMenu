# PostalBorkenMenu - Technical Project Notebook

This file is the cumulative engineering notebook for **PostalBorkenMenu**.

The project name is intentionally misspelled. **Borken** is a deliberate nod to *POSTAL: No Regerts*.

## Canonical state

Current canonical base: **V2A8**

V2A8 is the first build validated by the user with all current developer-menu features working in-game **and** clean process exit.

### Frozen V2A8 behavior

- x64 ASI for POSTAL: Brain-Damaged.
- No on-disk patching of the game EXE.
- No on-disk patching of GameAssembly.dll.
- No on-disk patching of UnityPlayer.dll.
- Dynamic IL2CPP lookup by metadata names where possible.
- Temporary IL2CPP attach/detach only.
- No permanent GC handles for command objects.
- Persistent keybinds and parameters in PostalBorkenMenu.ini.
- Hotkeys captured by GetAsyncKeyState polling, then execution posted back to the Unity window thread.
- Native developer-command entries are preserved.
- Synthetic commands are separate rows.
- Give All Weapons is validated and must not give quest items.
- TimeScale default selected value is 0.5x.
- Clicking the same non-1.0 TimeScale value again returns to 1.0x.
- No Crosshair is visual-only and must not alter logical aim.
- No Crosshair uses CanvasRenderer alpha, not SetCurrentCrosshair(NULL).
- Permanent periodic IL2CPP polling for crosshair refresh is rejected.
- Crosshair refresh is event-driven.
- Shutdown must fence all IL2CPP work before Unity teardown.

## Critical launch rule

Remove `-debug` from launch options.

PostalBorkenMenu directly uses the game's internal developer systems. Leaving the game's own `-debug` mode enabled made earlier loader tests ambiguous.

## Audited game build

Engine/runtime:

- x64 Unity IL2CPP
- Unity generation identified as 2021.3.14f1

Hashes:

```text
POSTAL Brain Damaged.exe
bdad7435995324cf35ca843bd0809dc28c600e283ca76b68b77e28dce0ac40f0

UnityPlayer.dll
27b88589c217589675c976bd303984286bb4b71516ab68efac1c178401a31e94

GameAssembly.dll
4281f23fc35afc17c6fe591b3dc06a4e29ba64ef0b1259de0ac8589c47e9c284

baselib.dll
78fd9445a545727bfcad40ed13f0593b83d608a5ed83be5f2a7ea3a67045bc07

global-metadata.dat
30537a062d887bb3bda9e0e48414f560d2f41d2a574073e9e34c2efb7cad5b98

PostalBorkenMenu.asi V2A8
0ca3f8d01257b84daca0d55b9ea9fe6cea6c0f6d16ef080a83815ae33ad0706a
```

## Native developer system discovered

Relevant game-side types include:

```text
Hyperstrange.PBD.DebugCommand
Hyperstrange.PBD.DebugConsole
Hyperstrange.PBD.DebugConsoleCommandComponent
Hyperstrange.PBD.DebugConsoleInputComponent
Hyperstrange.PBD.DebugConsoleWindowComponent
Hyperstrange.PBD.DebugManager
```

Native command classes discovered:

```text
Help
NextLevel
NoNpc
ForceAggro
GodMode
Noclip
NoTarget
NoHud
NoHudWithCrossHair
NoModel
TimeScale
LogTime
GiveAll
UnlockAll
NoMenu
KillPresident
```

The native command classes expose the existing game behavior. PostalBorkenMenu should use those existing paths rather than reimplementing effects when possible.

## Inventory / DLC discoveries reserved for future work

Relevant inventory types discovered during the initial audit:

```text
PlayerInventoryComponent
WeaponsInventory
WeaponId
DLCCategory
```

Useful methods/fields found:

```text
PlayerInventoryComponent.GiveAllWeapons()
WeaponsInventory.AddWeapon(WeaponId)
WeaponsInventory.EquipWeapon(WeaponId)
WeaponsInventory._allWeapons
WeaponId._dlcCategory
```

DLCCategory contains:

```text
NONE
PTSD
```

The internal `PTSD` category maps to the These Sunny Daze DLC.

Steam DLC AppID observed in the game's code:

```text
3768390
```

Ownership-check paths discovered:

```text
DownloadableContent.IsContentAccessible(DLCCategory)
DownloadableContent.IsDlcPurchased(DLCCategory)
DownloadableContent.GetDlcId(DLCCategory)
IsDlcPurchasedOnSteam(...)
IsDlcPurchasedOnGog(...)
```

Future DLC work must preserve normal ownership checks. The intended feature is to use owned DLC weapons in the base campaign, not bypass DLC ownership.

## V2A42-H22 - Startup class inventory / SceneCluster metadata candidate

Status: **diagnostic candidate, not canonical**.

Base:
- gameplay/source reset to **V2A42-H13**
- one-log-per-launch DXGI reset preserved

H21 runtime evidence:
- `RCLogoComponent` namespace `Hyperstrange.PBD.Ready`: `OnPointerClick`, `OnPointerEnter`, `OnPointerExit`; field `_cursorSprite`
- `IntroController` namespace `Hyperstrange.PBD`: video callbacks and `PlayVideo`, plus skip-progress logic and video fields
- `SkipIntroController`: preference-style `OnValueChanged` / `GetInitialValue`
- `SceneManager`: static `LoadIntroCluster()`, static `LoadTitleCluster()`, fields `_introCluster`, `_titleCluster`

Interpretation:
- RCLogoComponent is not currently supported as the startup-sequence driver.
- IntroController clearly belongs to the actual intro-video path.
- SceneManager is the strongest current transition-level candidate.
- H22 deliberately does not call LoadTitleCluster yet.

H22 enumerates Assembly-CSharp class metadata names matching:
`Intro`, `Logo`, `Ready`, `Title`, `Cluster`, `Splash`, `Startup`, `Boot`.

H22 also performs a full method/field metadata audit of `SceneCluster`.

Safety:
- no live Unity object enumeration
- no field writes
- no runtime invocation
- no startup method calls
- no synthetic input
- no PEB modification
- H13 timing/gameplay preserved

Canonical status remains **V2A42-H13**.

## V2A42-H21 - PlatformIntro metadata audit candidate

Status: **diagnostic candidate, not canonical**.

Base:
- source/gameplay lineage reset to user-validated **V2A42-H13**
- H20 code scanner is not inherited
- H19 live early-object watcher remains rejected
- H20 DXGI one-log-per-launch reset is preserved

Evidence motivating H21:
- H20 found `Assets/Scenes/PlatformIntro.unity`
- H20 found `Assets/Scenes/Intro.unity`
- H20 found `RCLogo` / `RCLogoImage` in `resources.assets`
- H20 found `VIDEO_Intro` and `Assets/Videos/Cinematics/VIDEO_Intro.webm` in `sharedassets1.assets`
- interpretation: PlatformIntro is the strongest current target for developer/publisher startup logos; VIDEO_Intro.webm is a separate opening cinematic. This remains an evidence-based inference until H21 metadata confirms the control path.

H21 targets:
- `RCLogoComponent`
- `IntroController`
- `SkipIntroController`
- `SceneManager`

H21 records metadata only:
- namespace/class name
- method name
- parameter count
- instance/static flag
- return type
- parameter types
- field names

H21 explicitly does **not**:
- enumerate live Unity objects
- write fields
- call `runtime_invoke`
- call startup/skip methods
- create an early IL2CPP worker
- simulate keyboard or mouse input
- modify the PEB/process command line
- patch game binaries

Timing:
- H13's normal 2500 ms IL2CPP startup delay remains intact
- the audit runs during the already validated H13 metadata-resolution phase

Packaging:
- `dxgi.dll`
- `PostalBorkenMenu.asi`
- `PostalBorkenMenu.ini`
- `README.txt`

Canonical status remains **V2A42-H13** until explicit user validation.

## Build history

### V1A

Goal:
- prove automatic loading
- resolve IL2CPP
- execute developer commands

Architecture:
- WinHTTP proxy proof of concept
- external/native menu window

Result:
- loader and IL2CPP resolution worked
- useful proof of concept
- UI architecture was not suitable as final design

### V1B

Goal:
- test D3D12 proxy loading

Important testing lesson:
- `-debug` had accidentally remained enabled during one test, making the result ambiguous
- this created the permanent README rule to remove `-debug`

### V1C

Goal:
- D3D11 path validation

Result:
- developer menu and most commands worked
- proved the command backend was viable

### V2A

Goal:
- migrate game-specific logic into an ASI
- keep the loader separate

Architecture decision:
- game-specific code belongs in PostalBorkenMenu.asi
- bootstrap/ASI loader should remain replaceable

Result:
- in-game functionality worked
- game crashed on exit

### V2A1

Attempted fix:
- restore original WndProc on window destruction
- shut down overlay cleanly

Result:
- rejected
- exit crash remained
- log did not reach the expected shutdown marker

### V2A2

Important lifetime fix:

Earlier builds kept a native worker attached to IL2CPP and kept command GC handles alive too long.

V2A2 changed the lifetime model:

- resolve il2cpp_thread_current
- resolve il2cpp_thread_detach
- resolve il2cpp_gchandle_free
- attach only temporarily
- detach after metadata work
- command objects created/invoked without permanent lifetime
- free handles immediately

User result:
- exit crash fixed

**V2A2 IL2CPP lifetime model became frozen.**

### V2A3

Validated improvements:
- settings/keybind persistence fix
- direct TimeScale bridge

Rejected design mistake:
- native NoHud was replaced by No Crosshair
- native GiveAll was replaced by Give All Weapons

Rule established:
- synthetic features must be ADDED as separate rows, never replace native rows

First No Crosshair and Give All Weapons implementations did not work correctly.

### V2A4

Command table expanded cleanly to 18 rows.

Native rows restored:
- NoHud
- GiveAll

Synthetic rows added separately:
- No Crosshair
- Give All Weapons

Give All Weapons path:
- live PlayerInventoryComponent
- PlayerInventoryComponent.GiveAllWeapons()

User result:
- **Give All Weapons worked**
- No Crosshair did not
- TimeScale second-click behavior did not

Give All Weapons became frozen.

### V2A5

No Crosshair experiment:
- PlayerCrosshairController.SetCurrentCrosshair(NULL)

User result:
- reticle disappeared
- weapon aim direction became wrong

Conclusion:
- current Crosshair object is part of aim logic
- SetCurrentCrosshair(NULL) is permanently rejected for visual hiding

Hotkey regression:
- many bound keys unreliable
- RUN button remained reliable

Diagnosis:
- command execution backend worked
- WM_KEYDOWN input path was unreliable under Unity

### V2A6

Hotkey fix:
- GetAsyncKeyState polling
- local edge detection
- PostMessage back to Unity window thread
- command execution remains on game thread

TimeScale fix:
- runtime_invoke on get_GameTimeScale()
- unbox returned System.Single
- if selected non-1 value is already active, apply 1.0

User result:
- TimeScale validated
- default requested value became 0.5x
- No Crosshair CanvasGroup attempt did not work

### V2A7

No Crosshair visual-only implementation:

- preserve _currentCrosshair
- preserve Crosshair object
- preserve aim logic
- locate current crosshair visuals
- enumerate CanvasRenderer children
- SetAlpha(0.0) to hide
- SetAlpha(1.0) to restore

User result:
- all in-game features worked
- reticle hidden
- weapons still aimed correctly
- exit crash returned

This validated the visual algorithm, but not the shutdown behavior.

### V2A8

Goal:
- preserve V2A7 behavior
- remove exit race

Changes:
- early shutdown fencing on WM_CLOSE
- g_shuttingDown set before Unity teardown
- g_ready cleared before teardown
- original WndProc restored early
- overlay shutdown requested before Unity object destruction
- fallback handling for WM_DESTROY / WM_NCDESTROY
- overlay destroyed on its creating thread
- permanent ~250 ms IL2CPP crosshair polling removed
- No Crosshair refresh changed to event-driven
- short delayed refresh after input likely to change weapon/crosshair

User result:
- **validated**
- all current features work
- game exits cleanly

V2A8 becomes the canonical base.

## Current command-table policy

Current list has native rows plus separate synthetic additions.

Important default bindings:

```text
F1 menu
F2 GodMode
F3 Noclip
F4 NoTarget
F5 NoHud
F6 GiveAll
F7 UnlockAll
```

Synthetic rows such as No Crosshair and Give All Weapons are unbound by default and can be assigned by the user.

## TimeScale policy

Use the direct TimeManager path.

Required UX:

```text
selected default: 0.5

1.0 -> click 0.5 -> 0.5
0.5 -> click 0.5 -> 1.0

1.0 -> click 2.0 -> 2.0
2.0 -> click 2.0 -> 1.0
```

Do not regress to the earlier debug-command-only implementation.

## No Crosshair policy

Rejected methods:

- SetCurrentCrosshair(NULL)
- disabling the logical crosshair GameObject
- hiding the whole HUD
- permanent periodic IL2CPP polling

Validated visual strategy:

- keep the logical crosshair alive
- affect only CanvasRenderer alpha
- use event-driven refresh when weapon/crosshair can change

## Give All Weapons policy

Validated behavior:

- separate from native GiveAll
- call PlayerInventoryComponent.GiveAllWeapons()
- do not call GiveAllItems()
- do not add quest items

## Shutdown policy

The game must exit cleanly.

Rules:

- no IL2CPP access after shutdown begins
- no permanent worker attachment
- no permanent command GC handles
- stop hotkey/crosshair work before Unity teardown
- restore WndProc before destruction
- overlay teardown happens on its own thread

## Packaging rule

User-facing update ZIPs contain exactly:

```text
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt
```

`PostalBorkenMenu.log` is runtime-generated and must not be pre-packaged in future archives.

ASI loader is intentionally separate.

## Next planned branch

After repository setup and source preservation, the planned next feature is the DLC-weapons branch:

- enumerate owned `PTSD` WeaponId entries
- confirm availability in base campaign
- use AddWeapon / EquipWeapon where appropriate
- preserve Steam/GOG ownership checks
- do not alter entitlement logic


### V2A22

Goal:
- retry a separate native DLC weapon wheel without removing the validated base wheel
- use the validated native PlayerInventoryComponent.GiveAllWeapons() path as a fallback when no PTSD/DLC wheel button is present
- preserve normal These Sunny Daze ownership checks

Architecture:
- adds independent Base Weapon Wheel and DLC Weapon Wheel HOLD bindings
- DLC wheel scans PlayerWeaponWheelComponent._weaponWheelButtons
- category 0 = base; category 1 = PTSD / These Sunny Daze
- if no DLC button is found, run native GiveAllWeapons(), attempt WeaponsInventory.InitHook(), refresh EnableButtons(), then rescan
- logs BASE/DLC button counts before and after fallback

User result:
- DLC weapon wheel appears to give/add the weapons, which is promising
- ALT mode regressed badly due to the V2A22 no-reswap guard

Rejected V2A22 ALT rule:
- do not skip EquipWeapon merely because the target WeaponId is already collected
- ALT depends on a post-vanilla EquipWeapon call to override the base number-key selection

### V2A23

Goal:
- preserve the promising V2A22 dual-wheel behavior
- restore ALT switching only

ALT correction:
- if target weapon is already collected, SafeAddAndEquipWeaponId skips duplicate AddWeapon but still calls EquipWeapon
- if target weapon is not collected, AddWeapon then EquipWeapon
- this restores the intended BASE/DLC alternation after the game's own vanilla number-key handling

Packaging change:
- update ZIPs no longer include PostalBorkenMenu.log
- runtime log remains generated by the ASI when the game runs


### V2A24

Goal:
- clean the WEAPONS tab by removing the obsolete direct Equip Base/DLC rows and the old Base/DLC Weapon 1/2 shortcut rows
- create a dedicated WEAPON LIST tab
- expose fixed base/DLC weapon slots 1 through 9 with independent RUN and KEY controls
- improve the failed DLC hook investigation without disturbing the dual-wheel branch

UI:
- tabs are now Gameplay / Weapons / Weapon List / Visual
- Weapon List order is strictly paired by slot:
  - 1 Base Weapon 1
  - 1 DLC Weapon 1
  - ...
  - 9 Base Weapon 9
  - 9 DLC Weapon 9
- every Weapon List row is a fixed slot; there is no editable slot argument
- each row has its own persistent key binding

Weapon execution:
- fixed Weapon List rows reuse EquipMappedWeaponCategorySlot(category, slot)
- category 0 = base game
- category 1 = PTSD / These Sunny Daze
- duplicate AddWeapon is avoided for already collected WeaponIds
- EquipWeapon remains allowed so direct selection and ALT behavior are preserved

Hook status:
- user reports the hook still does not work
- V2A24 does not claim a hook fix
- Give / Init DLC Hook now first runs the validated native GiveAllWeapons path
- it enumerates methods on PlayerInventoryComponent and WeaponsInventory and logs names containing Hook or Grap, plus parameter counts
- it then runs the known get_Hook / InitHook / get_Hook sequence
- intended next step is to use the runtime log to identify the actual native hook/grapple path rather than guessing another WeaponId

Packaging:
- ZIP contains only PostalBorkenMenu.asi, PostalBorkenMenu.ini and README.txt
- PostalBorkenMenu.log is runtime-generated and is not distributed


### V2A25

User corrections:
- in WEAPON LIST, the final 1..9 value is the Argument, not part of the display label
- V2A24 Hook command was observed to give the weapon set because its diagnostic fallback explicitly called GiveAllWeapons()

Weapon List correction:
- display labels are now "1 Base Weapon", "1 DLC Weapon", ... "9 Base Weapon", "9 DLC Weapon"
- Args are initialized and persisted separately as 1..9
- DecodeWeaponListCommand reads the live Argument value and uses it as the actual WeaponId slot
- Argument cells are clickable and cycle 1..9
- RUN and KEY remain independent per row

Hook correction:
- removed GiveAllWeapons() entirely from InitOwnedDlcHook()
- command display renamed DLC Hook Probe
- probe now performs only ownership check, Hook/Grap method audit, get_Hook(), InitHook(), and final get_Hook()
- V2A25 does not claim the hook itself is fixed; it removes the weapon-grant side effect and gathers cleaner evidence


### V2A26

Evidence from V2A25 user log:
- DLC Hook Probe no longer invokes GiveAllWeapons
- get_Hook() is null before direct InitHook()
- InitHook() completes without an IL2CPP exception
- get_Hook() remains null after InitHook()
- V2A24 historical lines show get_Hook() already non-null after GiveAllWeapons(), before InitHook would have been needed

Conclusion:
- InitHook() alone is insufficient
- a prerequisite state or object is created/configured elsewhere on the native GiveAllWeapons path
- do not reintroduce GiveAllWeapons into the hook probe

V2A26 diagnostic expansion:
- add il2cpp_class_get_fields
- add il2cpp_field_get_name
- add il2cpp_method_get_return_type
- log exact get_Hook return class metadata
- dump fields of PlayerInventoryComponent and WeaponsInventory
- scan Assembly-CSharp for class, namespace, method or field names containing Hook, Grap, Rope, Cable or Tether
- for matching classes, dump methods, parameter counts, return classes and fields
- keep get_Hook -> InitHook -> get_Hook as the only active hook mutation
- no weapon grants are performed by the probe


### V2A29

User correction:
- V2A18 was not the desired wheel reference.
- desired reference is V2A20 because its weapon wheel remains displayable even when the player has no collected weapon.

Implementation:
- branch starts from V2A26 to preserve Weapon List, ALT and Hook Deep Probe.
- exact V2A20 Base Weapon Wheel block transplanted.
- DLC wheel keeps split V2A26 state/hotkey plumbing but removes all GiveAllWeapons and Hook fallbacks.
- DLC Show path restores native buttons, filters category 1 where available, and still calls PlayerWheelView.Show() when enabled DLC count is zero.
- zero DLC buttons is now a valid empty-shell state, not an error.
- close path still calls Hide() and EnableButtons() to restore the game's native wheel state.

Goal:
- reproduce the useful V2A20 visual behavior first.
- keep DLC wheel visible for further experimentation even before we solve how to populate it with DLC WeaponIds.


### V2A30

User result:
- V2A29 weapon-wheel behavior is validated as correct.
- Hook Deep Probe crashes the game.

Crash-log diagnosis:
- the log enters the global Assembly-CSharp hook discovery scan.
- the scan produces a large number of unrelated matches and ends abruptly mid-class, without the discovery end marker.
- this places the crash inside the broad metadata enumeration, before the normal get_Hook -> InitHook sequence completes.
- substring "grap" also catches unrelated names such as Graphic/GatherProperties, making the scan far broader than intended.

Useful evidence recovered before the crash:
- WeaponsInventory.get_Hook() return type = Hyperstrange.PBD.WeaponId.
- WeaponsInventory has a field named _hookWeapon.
- WeaponsInventory also exposes get_HookCooldown and InitHook.

V2A30 correction:
- preserve V2A29 wheels unchanged.
- remove LogGlobalHookDiscovery() from the active probe path.
- no global class/member enumeration is executed.
- rename UI row to DLC Hook Safe Probe.
- inspect only the live PlayerInventoryComponent / WeaponsInventory instance.
- log _hookWeapon before/after InitHook.
- log get_Hook() before/after InitHook.
- if a WeaponId exists, log get_Category() and get_Slot().
- no GiveAllWeapons, AddWeapon or EquipWeapon is called by the probe.


### V2A31

V2A30 user log result:
- V2A30 completed without the previous deep-probe crash.
- V2A29 Base and DLC wheel behavior remained intact.
- DLC empty native wheel shell opened and closed repeatedly.
- shutdown remained clean through the V2A8 fence.
- safe Hook probe showed _hookWeapon=NULL and get_Hook()=NULL before InitHook().
- after native InitHook(), both values remained NULL.
- user then invoked Give All Weapons immediately after the probe.

Reason for V2A31:
- older logs showed get_Hook() becoming non-null after GiveAllWeapons.
- V2A30 did not yet snapshot hook state around the GiveAllWeapons call itself.
- instrumentation is therefore placed around the validated native call rather than adding another speculative activation path.

V2A31 instrumentation:
- preserve GiveAllWeapons behavior exactly.
- before call: targeted snapshot of WeaponsInventory._hookWeapon and get_Hook().
- after call: same snapshot.
- if a Hook WeaponId exists, log WeaponId.get_Category() and get_Slot().
- compare _hookWeapon pointer with get_Hook() return.
- no global metadata scan.
- no extra AddWeapon / EquipWeapon / InitHook call.


### V2A32

V2A31 result from user log:
- before GiveAllWeapons: _hookWeapon=NULL, get_Hook()=NULL
- after GiveAllWeapons: _hookWeapon remains NULL
- after GiveAllWeapons: get_Hook() becomes NON-NULL
- returned Hook WeaponId is category 0, slot 99
- therefore get_Hook() is not simply mirroring _hookWeapon

V2A32 hypothesis:
- the Hook is exposed when the special category0/slot99 WeaponId enters the collected inventory.
- native GiveAllWeapons likely adds that special WeaponId among its broader work.
- if true, adding only that one WeaponId should make get_Hook() non-null without granting the normal weapon set.

V2A32 implementation:
- preserve V2A29 wheels and V2A30 safe probe behavior.
- after InitHook still leaves Hook null, scan loaded WeaponIds.
- accept only a unique category0/slot99 candidate.
- inspect CollectedWeapons before mutation.
- call WeaponsInventory.AddWeapon(candidate) only if not already collected.
- never call EquipWeapon for the hook candidate.
- never call GiveAllWeapons from the Hook test.
- re-run get_Hook() and compare returned pointer to the exact candidate.
- refuse mutation on ambiguity or failed collected-state inspection.


### V2A33

User request:
- start from V2A32
- add a Weapon Catalog function to identify all loaded weapons by name rather than only category/slot

Implementation:
- branch: dev/v2a33-weapon-catalog
- V2A32 behavior preserved
- added synthetic menu command __WeaponCatalog / "Weapon Catalog"
- command lives in the Weapons tab
- command is read-only

Catalog data source:
- UnityEngine.Resources.FindObjectsOfTypeAll(Hyperstrange.PBD.WeaponId)

Per WeaponId logging:
- array index
- UnityEngine.Object.get_name()
- WeaponId.ToString() when resolvable
- WeaponId.get_Category()
- WeaponId.get_Slot()
- exact-object membership in PlayerInventoryComponent.get_CollectedWeapons()

Category labels:
- 0 = BASE / DLCCategory.NONE
- 1 = PTSD / These Sunny Daze
- any other value = OTHER

String handling:
- uses il2cpp_string_chars and il2cpp_string_length when exported
- both helpers are optional and do not become new startup requirements
- UTF-16 characters outside printable ASCII are replaced with '?' in the technical log only
- if Unity names are unavailable, catalog continues and logs <empty>

Safety:
- no AddWeapon
- no EquipWeapon
- no GiveAllWeapons
- no GiveAllDlcWeapons
- no InitHook
- no wheel mutation
- no global Assembly-CSharp class scan

Goal:
- identify the real names of the 25 known Base WeaponIds, the loaded PTSD/DLC WeaponIds, and special entries such as category0/slot99
- distinguish duplicate WeaponIds sharing the same slot


### V2A34

User V2A33 log diagnosis:
- V2A33 banner loaded correctly.
- READY banner confirmed non-destructive Weapon Catalog was present.
- three attempts produced "[ERROR] Requested native command is unresolved".
- no [WEAPON CATALOG] block was emitted.
- source audit found the exact wiring bug: the active ExecuteCommandIndex() is in source/PostalBorkenMenu_part2.inc, while the V2A33 dispatcher edit had been applied to the wrong source part.
- RunWeaponCatalog(), the menu row, and tab placement were otherwise present.

V2A34 fix:
- add __WeaponCatalog -> RunWeaponCatalog() to the active part2 ExecuteCommandIndex().
- no catalog algorithm changes.
- no wheel changes.
- no hook changes.
- no inventory mutation added by the catalog.

Important Hook result from same user log:
- InitHook alone still left _hookWeapon and get_Hook() null.
- exactly one loaded category0/slot99 WeaponId was found.
- native AddWeapon(category0/slot99) completed without EquipWeapon.
- get_Hook() then became NON-NULL.
- _hookWeapon remained NULL.
- get_Hook() returned the exact category0/slot99 candidate.
- this validates the slot99 AddWeapon isolation hypothesis from V2A32.


### V2A38

Bisect target:
- user specifically flagged the V2A35 exact-name selection system as suspicious.
- start directly from V2A34 known-good branch.
- transplant only the V2A35 exact-name resolver code.

Added to source/PostalBorkenMenu_part2.inc:
- DecodeNamedWeaponCommand()
- ExecuteNamedWeaponByUnityName()
- volatile function pointer g_dormantExactNameResolver to force linker retention under /OPT:REF

Not added:
- no __Arsenal_* or __Special_* command rows
- no named-weapon dispatcher branch
- no new tabs
- no config changes
- no runtime call to the resolver
- no automatic WeaponId enumeration
- no AddWeapon/EquipWeapon execution
- no recovery path

Startup:
- reads only the volatile resolver pointer and logs whether it is retained.
- then continues through the original V2A34 startup path unchanged.

Interpretation:
- if stable, resolver code presence is not the cause.
- if it crashes, bisect inside ExecuteNamedWeaponByUnityName next.


### V2A39

Base:
- V2A38 dormant exact-name resolver bisect, validated working by the user.

Single functional delta:
- ExecuteCommandIndex() now declares exactWeaponName/exactSpecial.
- DecodeNamedWeaponCommand() is called first.
- if it returns true, ExecuteNamedWeaponByUnityName() is called.
- otherwise execution falls through to the untouched V2A34 legacy dispatcher.

Important:
- command table remains V2A34/V2A38.
- there are zero __Arsenal_* rows.
- there are zero __Special_* rows.
- therefore the new branch should never reach ExecuteNamedWeaponByUnityName() in this test.

No other V2A35 functional changes:
- no five-tab UI
- no Arsenal/Special rows
- no INI expansion
- no startup scan
- no automatic inventory mutation

Interpretation:
- stable => dispatcher branch is innocent; isolate g_cmds row expansion next.
- crash => dispatcher integration itself is the regression.


### V2A40

Base:
- V2A39 named-dispatch no-rows bisect.

Observed V2A39 result:
- user reports crash.
- runtime log reaches:
  - IL2CPP ready
  - overlay creation
  - game window subclass
  - async keyboard polling
  - READY
- no command execution is logged after READY.
- therefore there is no evidence that DecodeNamedWeaponCommand or ExecuteNamedWeaponByUnityName ran before the crash.

Hypothesis:
- tiny code/timing changes may alter which foreground process window HookGameWindow() subclasses during Unity startup.
- previous V2A35 runs were inconsistent around the same overlay/subclass boundary.
- treat the immediate first-foreground-window subclass as a race candidate.

Single functional change:
- HookGameWindow() now requires the same valid foreground HWND from the current process to remain stable for 3 seconds.
- candidate must have a client area >= 320x200.
- candidate resets if foreground ownership/validity/size changes.
- only after 30 consecutive 100 ms samples is SetWindowLongPtrW called.

No weapon, UI, INI, IL2CPP or dispatcher changes versus V2A39.


### V2A41

Base:
- V2A40 stable-window subclass test, validated working by the user.

Single diagnostic change:
- add the 26 V2A35 __Arsenal_* rows.
- add the 8 V2A35 __Special_* rows.
- keep every V2A40/V2A34 legacy row, including all 18 Weapon List rows.
- CMD_COUNT becomes 76.

Intentionally unchanged:
- no five-tab UI.
- no Arsenal/Special tab routing.
- no INI additions.
- no new startup logic.
- no automatic named-weapon execution.
- V2A40 stable 3-second HWND qualification remains frozen.

Diagnostic note:
- because CommandMenuTab() remains V2A40, named rows default to Gameplay if the overlay is opened.
- user should test startup without F1 first.

Interpretation:
- stable => command-table expansion itself is innocent.
- crash => investigate LoadConfig/CMD_COUNT traversal and row/table layout before any UI work.


### V2A42

Base:
- V2A41 named-row branch.
- Preserves V2A40 stable-window subclass behavior validated by the user.

User-directed cleanup:
- remove every __Arsenal_* row whose WeaponId ends in _Akimbo.
- remove all four Dong special rows:
  - WEAPON_Dong
  - WEAPON_Dong_Confusion
  - WEAPON_Dong_Fire
  - WEAPON_Dong_Ice
- remove the old generic V2A34 WeaponListBase/WeaponListDlc rows from g_cmds and INI.

Final named lists in V2A42:
- Arsenal: 14 rows
  - 9 base single weapons
  - 5 These Sunny Daze/PTSD single weapons
- Special: 4 rows
  - Hook
  - CutsceneWeapon_DLC
  - NoWeapon
  - NoWeapon_DLC
- zero _Akimbo rows
- zero Dong rows
- zero generic Weapon List rows.

UI:
- restore five tabs: Gameplay / Weapons / Arsenal / Special / Visual.
- named weapon dispatcher routes __Arsenal_* to Arsenal and __Special_* to Special.
- Visual becomes tab index 4.
- V2A40 stable-window fix remains untouched.

Skip Intro Videos:
- synthetic __SkipIntroVideos checkbox row in Gameplay.
- [Settings] SkipIntroVideos defaults to 1.
- checkbox state persists to PostalBorkenMenu.ini.
- state changes take effect on the next launch.
- this first implementation does not modify SAVE_DATA.cfg and does not patch game binaries.
- keybd_event is imported from user32.dll.
- during HookGameWindow qualification, if the same valid foreground game HWND reaches 1 second of stability and skip is enabled, send one Escape key down/up pulse.
- g_introSkipPulseSent prevents repeats.
- WndProc subclass still waits for the full V2A40 3-second stability window.

Rationale:
- the retail game exposes a Skip Intro Video option in its General settings, but this ASI implementation deliberately avoids unknown save/config internals until the exact native storage path is verified.
- one startup-only Escape pulse is a reversible test that does not touch saves or game files.

Packaging:
- exactly PostalBorkenMenu.asi, PostalBorkenMenu.ini, README.txt.
- never include PostalBorkenMenu.log.


### V2A42-H3 / H4

H3 validation:
- exact `WEAPON_Hook` path validated by the user.
- required sequence is:
  1. resolve exact `WEAPON_Hook`
  2. `AddWeapon` only when required
  3. always call `WeaponsInventory.InitHook()` after the AddWeapon/collected state
  4. verify `get_Hook()`
- the previous early-return implementation was wrong because `AddWeapon` could make `get_Hook()` non-null before `InitHook()` ran.

H4 cleanup:
- renamed the menu entry to `Grappling Hook`.
- removed Weapon Catalog and the temporary DLC Hook Slot99 / InitHook menu rows.
- preserved the validated H3 internal Hook sequence.
- kept the standard minimal DXGI ASI loader.


### V2A42-H5 through H12

DLC Weapon Wheel research only. None of these builds became canonical.

H5:
- attempted temporary native `WeaponWheelButton` WeaponId remapping.
- crashed because the guessed setter/field write path was invalid.
- rejected.

H6:
- safe metadata-only probe.
- established the real `WeaponWheelButton` field:
  - `_weaponId : Hyperstrange.PBD.WeaponId`
- established that no public `set_WeaponId` method exists.

H7 / H8:
- used the exact `_weaponId` object field.
- native wheel became visible.
- H8 proved the five first button WeaponIds really changed to:
  - `WEAPON_UmDrill`
  - `WEAPON_PissGun`
  - `WEAPON_MeatShotgun`
  - `WEAPON_BubbleGumMachineGun`
  - `WEAPON_NuclearSyringe`
- however the wheel still behaved visually/logically like vanilla slots.

H9:
- attempted to consume `PlayerWeaponWheelComponent._selectedButton` at close.
- `_selectedButton` remained null.
- rejected.

H10:
- attempted the native `OnWheelDown/OnWheelUp` path.
- wheel no longer became visible.
- rejected.

H11:
- attempted native selection while preserving additional experimental state.
- wheel remained invisible although time slowdown activated.
- rejected.

H12:
- attempted to return to visible H7 behavior while explicitly activating the native controller.
- user reported regressions:
  - Base Weapon Wheel broken
  - No Crosshair regressed / failed in DLC
- rejected completely.

Rule established:
- do not build future work from H9-H12.
- Base Weapon Wheel must remain on the H4 path unless a new isolated test proves otherwise.


### V2A42-H13

Base:
- built directly from H4, explicitly excluding H9-H12 wheel experiments.

Recovery goals:
- restore Base Weapon Wheel to H4 behavior.
- recover/extend No Crosshair for DLC.
- remove the obsolete synthetic Escape Skip Intro behavior.
- audit the real internal NO_VIDEO / DebugManager path without mutating it.

No Crosshair change:
- H4 used a single `PlayerCrosshairController` returned by `FindObjectOfType`.
- H13 additionally resolves `Resources.FindObjectsOfTypeAll(PlayerCrosshairController)`.
- each loaded controller is inspected.
- only the controller's current Crosshair object is traversed.
- only child `CanvasRenderer.SetAlpha()` is changed.
- the native Crosshair object is never cleared or replaced.
- aim/target logic remains untouched.
- H4 single-controller behavior remains as fallback.

Skip Intro:
- all synthetic keyboard input was removed.
- the old Escape-based implementation is permanently rejected.
- H13 does not claim Skip Intro is fixed.
- a targeted metadata audit logs `DebugManager`, a `NO_VIDEO` class if present, and `PlayerCrosshairController` to identify the native path.

Validation:
- user explicitly reported H13 validated.
- H13 is promoted to the current canonical source/gameplay base.
- Base Weapon Wheel recovery validated.
- No Crosshair recovery / DLC behavior validated.
- validated Grappling Hook sequence preserved.

Build:
- GitHub Actions run: `36344321616`
- artifact: `POSTAL_Brain_Damaged_PostalBorkenMenu_V2A42_H13_RECOVERY_DLC_CROSSHAIR_AUDIT_TEST`
- PostalBorkenMenu.asi SHA-256:
  `7A34F32BDD282BDAFE2F6B8032D5ED0A46C7617BB3B1BDA850BAD0408959A052`
- dxgi.dll SHA-256:
  `B28B7DDAE1E37AA60DBF5246ECCC12403006A3A1115442B4B33EC1D664245AF1`
- artifact ZIP SHA-256:
  `9062aac35196afa7ed4f4c21585ab759e9cd366bc5cf288b275fbccb7acafb61`

Canonical rule:
- future gameplay/source changes start from V2A42-H13.
- Skip Intro remains an open item and must be solved without simulated keyboard/mouse input.


#### V2A42-H13 GitHub Release

Published release:
- tag: `V2A42-H13`
- release title: `PostalBorkenMenu V2A42-H13`
- asset: `POSTAL_Brain_Damaged_PostalBorkenMenu_V2A42_H13.zip`
- GitHub Actions publish run: `36346565155`

Release-build SHA-256:
- `PostalBorkenMenu.asi`
  `2eb2f49f0ef3a1823a9c5a739f321dbc6f129e5ebd2c2e614f5f9a015683c92c`
- `dxgi.dll`
  `a71a24368479620ea93e58b35a7291ee175ec894ad85ab3122510eff580d52ef`
- release ZIP
  `2b3c4f233a0a706714b7f198be5867506b62e7e21b4491f401079f7efb8513f0`

The release ZIP contains exactly:
- `dxgi.dll`
- `PostalBorkenMenu.asi`
- `PostalBorkenMenu.ini`
- `README.txt`

Note:
- these hashes are for the canonical GitHub Release rebuild.
- they intentionally supersede the earlier H13 validation-artifact hashes for public distribution.
- Skip Intro remains an open item and is not claimed fixed by this release.


## Logging policy - one launch, one log

From commit `1a97bd2d3ad8f43a2ff387a04cf3eb11cc7ce1c4`, the minimal DXGI ASI loader truncates `PostalBorkenMenu.log` exactly once at first DXGI initialization for each game process, before writing any loader or ASI diagnostic line. Subsequent writes in the same run remain append-only. This prevents diagnostics from multiple builds/runs being concatenated into one file.


## V2A42-H19 - rejected: immediate crash during early Unity observation

H19 resolved GameAssembly, the IL2CPP domain, Assembly-CSharp/CoreModule and all four target class metadata entries, then crashed before the first object-count result. The failure therefore occurred as the early watcher transitioned into Unity runtime object queries. H19 is rejected and must not be used as a future base.

## V2A42-H20 - static startup asset scan

Base: current main / validated H13 gameplay lineage, including one-launch-one-log reset. H20 removes the entire early Unity-observation concept. It performs only read-only Win32 file I/O against the game's Unity *_Data directory and scans selected Unity data files for printable ASCII strings related to startup logos, publishers, developers and intro/splash assets. No Unity or IL2CPP runtime method is called by the H20 diagnostic.
