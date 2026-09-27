PostalBorkenMenu V2A42-J1 REAL SKIPINTRO CMDLINE + DXGI
POSTAL: Brain-Damaged

BASE
Built directly from H4 clean grappling/menu base.

J1 GOAL
Make the menu option Skip Intro Videos reproduce the real game launch option instead of simulating user input.

IMPLEMENTATION
- dxgi.dll reads [Settings] SkipIntroVideos from PostalBorkenMenu.ini extremely early.
- If enabled, dxgi.dll appends -skipintro to the process command line before normal game startup.
- If -skipintro is already present, it is left unchanged.
- The ASI no longer emits Escape or any other keyboard/mouse input for Skip Intro.
- The Gameplay-tab checkbox and INI persistence remain unchanged.
- A change in the menu applies on the NEXT LAUNCH, exactly as before.

TEST
1. Enable Skip Intro Videos in the menu.
2. Exit the game completely.
3. Launch again with NO manual Steam -skipintro option.
4. Confirm the intro videos are skipped exactly like a manual -skipintro launch.
5. Disable Skip Intro Videos, restart, and confirm normal intro behavior returns.
6. Send PostalBorkenMenu.log if either direction fails.

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
