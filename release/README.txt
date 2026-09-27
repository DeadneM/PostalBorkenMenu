PostalBorkenMenu V2A42-H11 DLC WHEEL NATIVE SELECTION + REAL SKIPINTRO
POSTAL: Brain-Damaged

BASE
Wheel work resumes from H7 because H10 removed the visible wheel.
H10 and H9 are rejected.

H11 WHEEL GOAL
Keep the H7 visible native wheel and force the game's own controller selection path only at close.

WHEEL IMPLEMENTATION
- H7 verified five exact DLC WeaponIds are mapped to the first five native WeaponWheelButton instances.
- The visible wheel still opens through PlayerWheelView.Show(), preserving the last working visual path.
- On close, while DLC WeaponIds are still active:
  1. PlayerWeaponWheelComponent.GetInputAngle()
  2. PlayerWeaponWheelComponent.UpdateSelection(angle)
  3. verify _selectedButton
  4. PlayerWeaponWheelComponent.SelectButton()
- Only after native SelectButton() is allowed to commit the choice does H11 hide the wheel and restore every original vanilla WeaponId.

DLC MAP
1 -> WEAPON_UmDrill
2 -> WEAPON_PissGun
3 -> WEAPON_MeatShotgun
4 -> WEAPON_BubbleGumMachineGun
5 -> WEAPON_NuclearSyringe

SKIP INTRO
H11 also contains the J1 loader-side real -skipintro command-line injection.
No Escape, SendInput, keybd_event, mouse event, or other simulated user input is used for Skip Intro.

TEST
1. Enable Skip Intro Videos, restart, and confirm the real intro bypass.
2. In gameplay HOLD DLC Weapon Wheel.
3. Move the wheel pointer to several of the first five sectors and release.
4. Check whether the corresponding DLC weapon is actually selected.
5. Open Base Weapon Wheel afterwards and confirm vanilla contents are restored.
6. Send PostalBorkenMenu.log.

UNCHANGED
- Grappling Hook validated AddWeapon -> InitHook fix
- H4 menu cleanup
- TimeScale / No Crosshair / hotkeys / shutdown stability
- minimal dxgi.dll ASI loader architecture

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H4 / PRIOR TECHNICAL HISTORY
======================================================================

PostalBorkenMenu V2A42-H7 DLC WHEEL OBJECT FIELD + DXGI
POSTAL: Brain-Damaged

BASE
Built directly from H4 clean gameplay base.
H5 rejected. H6 diagnostic proved the real WeaponWheelButton field.

H6 DISCOVERY
WeaponWheelButton exposes:
- get_WeaponId()
- no set_WeaponId()
- field _weaponId : Hyperstrange.PBD.WeaponId

H7 FIX
- Finds the five exact gameplay DLC WeaponIds.
- Ensures they are collected.
- Reuses native WeaponWheelButton instances.
- Writes ONLY WeaponWheelButton._weaponId.
- Uses il2cpp_field_set_value_object for the managed object reference.
- Verifies each write immediately with get_WeaponId().
- Opens the native wheel only after successful verified remaps.
- Restores original WeaponId object references on close using the same object-field API.
- Calls native EnableButtons() after restoration.

DLC WHEEL CONTENT
- Um Drill
- Piss Gun
- Meat Shotgun
- Bubble Gum Machine Gun
- Nuclear Syringe

UNCHANGED
- Grappling Hook validated AddWeapon -> InitHook sequence
- H4 menu cleanup
- Base Weapon Wheel logic
- Weapon Keys Mode
- DXGI ASI loader

TEST
1. Install all four files.
2. Enter gameplay.
3. HOLD DLC Weapon Wheel.
4. Check that five DLC entries appear.
5. Select at least two different DLC weapons.
6. Release the key.
7. Open Base Weapon Wheel and confirm normal weapons are restored.
8. Test Grappling Hook once.
9. Send PostalBorkenMenu.log if anything is wrong.

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H4 BASE README
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
