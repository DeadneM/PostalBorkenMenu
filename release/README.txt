PostalBorkenMenu V2A51 TEST
POSTAL: Brain-Damaged

BASE
V2A51 is built directly from V2A50, which itself was built directly from the
user-validated V2A44 line.

PURPOSE
The user reported that opening the menu in gameplay left the mouse centered and
the game crashed.

For this build, the menu mouse experiment is removed completely.
We are NOT trying to solve mouse capture yet.

V2A51 keeps:
- dxgi.dll as the minimal ASI loader
- PostalBorkenMenu.asi
- F1 menu visibility
- menu pause through the already-existing TimeManager bridge
- Grappling Hook exact WEAPON_Hook AddWeapon-only path
- V2A50 DLC weapon-wheel remap experiment
- all previously preserved gameplay features

V2A51 removes from the menu path:
- ShowCursor
- SetCursor
- GetCursor
- LoadCursorW
- forced arrow cursor
- cursor display-count balancing
- WM_SETCURSOR handling
- WM_INPUT suppression
- WM_MOUSEMOVE suppression
- left/right/middle mouse interception
- menu mouse-wheel scrolling
- menu mouse clicks

IMPORTANT
The F1 menu is temporarily VIEW-ONLY with respect to the mouse.

The game receives its mouse messages normally.
The game may therefore keep its usual FPS cursor lock/recentering behavior while
the F1 panel is visible. That is EXPECTED for this test.

The goal is only to confirm whether removing our menu mouse/cursor layer removes
the crash.

FOCUS OF CURRENT DEVELOPMENT
1. DLC Weapon Wheel
2. Grappling Hook
3. menu/debug cleanup later
4. redesign later
5. proper mouse interaction later, as a separate task

DLC WEAPON WHEEL
Unchanged from V2A50:
- exact five gameplay DLC WeaponIds
- temporary native WeaponWheelButton remap
- original native WeaponIds restored when the DLC wheel closes

GRAPPLING HOOK
Unchanged from V2A50 / V2A32-proven path:
- exact WEAPON_Hook
- AddWeapon only when required
- no EquipWeapon
- no GiveAllWeapons fallback
- verify through WeaponsInventory.get_Hook()

TEST
1. Remove any older dxgi.dll / ASI loader from the game directory.
2. Install the four V2A51 files.
3. Launch normally without -debug.
4. Enter gameplay.
5. Press F1.
6. Do NOT expect a usable mouse cursor in the menu.
7. Confirm whether simply opening/closing F1 is now stable and no longer crashes.
8. Test Grappling Hook.
9. Test DLC Weapon Wheel with its assigned hotkey.
10. Exit normally.
11. If anything crashes, send PostalBorkenMenu.log.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt
