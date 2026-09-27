PostalBorkenMenu V2A42-H5 DLC WEAPON WHEEL REMAP + DXGI LOADER
POSTAL: Brain-Damaged

BASE
Built from V2A42-H4 clean grappling menu.
The validated Grappling Hook fix remains unchanged.

H5 GOAL
Make the DLC Weapon Wheel actually contain the five gameplay DLC weapons instead of
opening an empty native wheel shell.

DLC WHEEL WEAPONS
- WEAPON_UmDrill
- WEAPON_PissGun
- WEAPON_MeatShotgun
- WEAPON_BubbleGumMachineGun
- WEAPON_NuclearSyringe

IMPLEMENTATION
- Reuses the game's native PlayerWeaponWheelComponent and PlayerWheelView.
- Finds the five exact DLC WeaponId objects by Unity name.
- Ensures each selected DLC weapon is collected before exposing it on the wheel.
- Temporarily remaps native WeaponWheelButton WeaponIds to those DLC WeaponIds.
- Enables only the mapped DLC buttons while the DLC wheel is held.
- On wheel close, restores every original native WeaponId and calls EnableButtons().
- The base/native weapon wheel is therefore not permanently modified.
- No custom overlay wheel is introduced.

UNCHANGED
- Grappling Hook AddWeapon -> InitHook fix
- menu layout
- mouse/cursor handling
- Base Weapon Wheel
- Weapon Keys Mode
- TimeScale
- No Crosshair
- Skip Intro
- DXGI ASI loader architecture

TEST
1. Copy all four package files next to the game executable.
2. Launch and enter gameplay.
3. Bind DLC Weapon Wheel to a key if needed.
4. HOLD the DLC Weapon Wheel key.
5. Confirm the native wheel opens with DLC weapons instead of an empty shell.
6. Select several DLC weapons and verify they equip correctly.
7. Release/close the DLC wheel.
8. Open the normal Base Weapon Wheel and confirm its original weapons are restored.
9. Send PostalBorkenMenu.log if anything is wrong.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H4 BASE NOTES
======================================================================

PostalBorkenMenu V2A42-H4 CLEAN GRAPPLING MENU + DXGI LOADER
POSTAL: Brain-Damaged

BASE
Built from the user-validated V2A42-H3 grappling-hook fix.
H3 is the validated gameplay basis for this cleanup build.

VALIDATED GRAPPLING FIX PRESERVED
Special -> Grappling Hook uses the validated sequence:
1. Resolve exact WEAPON_Hook.
2. AddWeapon(WEAPON_Hook) only if required.
3. Never use ordinary EquipWeapon for the Hook.
4. Always call WeaponsInventory.InitHook() AFTER the AddWeapon/collected state.
5. Verify get_Hook().

MENU CLEANUP ONLY
- "Hook (Add Only)" renamed to "Grappling Hook".
- Weapon Catalog menu entry removed.
- DLC Hook Slot99 Test / InitHook menu entry removed.
- Their command dispatch/menu routing entries are removed.
- No other menu layout or behavior is changed.

DXGI ASI LOADER
dxgi.dll remains the standard ASI loader.
It forwards the real System32 DXGI exports and loads PostalBorkenMenu.asi.
It does not hook Present, render anything, patch swapchains/vtables, or alter gameplay.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
ORIGINAL V2A42 README
======================================================================

PostalBorkenMenu V2A42
POSTAL: Brain-Damaged

BASE
Built from V2A41 while preserving the V2A40 stable-window subclass fix that the user validated.

THIS BUILD
V2A42 reconstructs the desired five-tab overlay and cleans the named weapon lists.

TABS
- Gameplay
- Weapons
- Arsenal
- Special
- Visual

ARSENAL
Only single WeaponId variants are exposed.

Base:
- Shovel
- Pistol
- Shotgun
- Machine Gun
- Rocket Launcher
- Lightning Gun
- Gatling Gun
- Dildo Bow
- Cat Canon

These Sunny Daze / PTSD:
- Um Drill
- Piss Gun
- Meat Shotgun
- Bubble Gum Machine Gun
- Nuclear Syringe

All _Akimbo WeaponIds are intentionally removed from the overlay.

SPECIAL
Kept:
- Hook (Add Only)
- Cutscene Weapon DLC
- No Weapon
- No Weapon DLC

Removed:
- Dong
- Dong Confusion
- Dong Fire
- Dong Ice

The Hook path remains AddWeapon-only and is never passed through EquipWeapon.

SKIP INTRO VIDEOS
A new checkbox appears in the Gameplay tab.

Default:
SkipIntroVideos=1

The value is persisted in PostalBorkenMenu.ini under [Settings].
Changing the checkbox affects the next launch.

Implementation for this test:
- no game files are renamed or deleted
- SAVE_DATA.cfg is not modified
- no EXE/GameAssembly patching
- while V2A40 qualifies the foreground game HWND, after the same HWND has remained valid for 1 second, the ASI sends exactly one synthetic Escape key pulse if Skip Intro Videos is enabled
- the normal V2A40 WndProc subclass is still delayed until 3 seconds of HWND stability
- no repeated key injection occurs

This deliberately targets startup only and does not touch normal in-game cutscenes.

STABILITY PRESERVED
- V2A40: 3-second stable foreground HWND qualification before SetWindowLongPtrW
- V2A8: early WM_CLOSE shutdown fencing
- no permanent IL2CPP crosshair polling
- V2A29 weapon-wheel behavior
- exact-name WeaponId resolver remains targeted
- no broad Assembly-CSharp scans
- no startup Hook recovery

TEST
1. Launch normally with SkipIntroVideos=1.
2. Confirm whether the intro video is skipped.
3. Confirm the game reaches the menu and remains stable.
4. Open F1.
5. Check all five tabs.
6. Confirm Arsenal contains no Akimbo entries.
7. Confirm Special contains no Dong entries.
8. Toggle Skip Intro Videos off and on once to verify checkbox persistence.
9. Test one normal Arsenal weapon and, if owned, one DLC weapon.
10. Exit normally and send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not included.
