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
