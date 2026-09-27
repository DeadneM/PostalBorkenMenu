PostalBorkenMenu V2A52 HOOK-ONLY TEST
POSTAL: Brain-Damaged

IMPORTANT
This candidate is built DIRECTLY from the user-validated V2A44 branch.
It does not inherit V2A50 or V2A51.

SCOPE
Only the exact WEAPON_Hook execution path is changed.

Unchanged from V2A44:
- F1 overlay/menu and all mouse behavior
- Weapon Catalog
- Gameplay / Weapons / Arsenal / Special / Visual tabs
- Arsenal and Special lists
- weapon wheels
- hotkeys and bindings
- No Crosshair
- TimeScale
- Give All Weapons
- shutdown handling
- all other IL2CPP bridges

HOOK TEST CHANGE
V2A44 proved that AddWeapon(WEAPON_Hook) alone could add the Hook WeaponId but
user testing shows the grappling hook is still not actually available until
native GiveAllWeapons() is used.

V2A52 tests the smallest missing native sequence:
1. resolve exact Unity WeaponId name WEAPON_Hook
2. AddWeapon only if it is not already collected
3. DO NOT call EquipWeapon
4. call native WeaponsInventory.InitHook()
5. verify WeaponsInventory.get_Hook() returns the exact WEAPON_Hook
6. never call GiveAllWeapons as fallback

TEST
1. Install this package exactly like V2A44.
2. Launch and confirm the familiar V2A44 menu/Weapon Catalog is unchanged.
3. Do NOT press Give All Weapons.
4. Run Hook (Add Only) from Special.
5. Try using the grappling hook.
6. If it does not work, send PostalBorkenMenu.log.
7. Then, only as a comparison, use Give All Weapons and tell us whether the hook becomes available.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
VALIDATED V2A44 BASE NOTES
======================================================================

PostalBorkenMenu V2A44
POSTAL: Brain-Damaged

BASE
Built directly from V2A42, which the user validated as working.

WHY V2A44 EXISTS
V2A43 crashed after adding new Unity Cursor and SceneManager IL2CPP bridges.
V2A43 is rejected.

V2A44 deliberately does NOT inherit the V2A43 branch.
It returns to V2A42 and adds only a much smaller overlay-input fix.

PRESERVED FROM V2A42
- V2A40 stable 3-second game-window qualification
- five tabs: Gameplay / Weapons / Arsenal / Special / Visual
- 14 single Arsenal weapons
- no Akimbo Arsenal rows
- 4 non-Dong Special rows
- V2A29 wheel behavior
- V2A8 shutdown fencing
- exact-name WeaponId resolver
- no broad Assembly-CSharp scans
- no startup Hook recovery

F1 OVERLAY MOUSE
V2A44 uses Win32 cursor APIs only:
- GetCursor
- LoadCursorW(IDC_ARROW)
- ShowCursor
- SetCursor

No Unity Cursor metadata bridge is added.

When F1 opens:
- the game window thread forces a visible arrow cursor
- raw/movement/right/middle mouse messages are swallowed while the overlay is open
- left click and wheel remain available to the overlay
- the current game timescale is saved
- the already-validated TimeManager.SetGameTimeScale bridge sets timescale to 0.0

When F1 closes:
- the previous timescale is restored
- only the ShowCursor count adjustments made by the mod are unwound
- the previous cursor handle is restored

SKIP INTRO
The V2A42 synthetic Escape implementation has been REMOVED.
Escape does not skip this game's intro and will not be sent anymore.

The game itself has a documented native -novideo launch option.
A proper overlay checkbox will only be restored once the exact native saved-setting representation is mapped from SAVE_DATA.cfg.

AUTO LEVEL ACTIONS
Not included in V2A44.
The V2A43 SceneManager experiment is removed completely because that build crashed.

The next level-start implementation will be rebuilt independently after V2A44 overlay stability is confirmed.

TEST
1. Launch normally.
2. Confirm startup remains as stable as V2A42.
3. Enter gameplay.
4. Press F1 without opening the game's Pause menu.
5. Confirm a visible mouse cursor appears.
6. Confirm gameplay pauses.
7. Click tabs and RUN controls.
8. Close F1 and confirm controls/timescale return normally.
9. Exit normally and send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not included.
