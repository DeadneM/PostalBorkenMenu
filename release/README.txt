PostalBorkenMenu V2A50 TEST
POSTAL: Brain-Damaged

BASE
Built directly from the user-validated V2A44 branch:
dev/v2a44-safe-win32-overlay-mouse

V2A43 remains rejected because of its crash.
The later V2A48 / V2A49 overlay experiments are NOT used as the gameplay base.

V2A50 deliberately changes only three areas:
1. restore a bundled DXGI ASI loader
2. test a real native DLC weapon wheel
3. keep the proven safe Grappling Hook path

The debug-command cleanup and visual redesign are intentionally postponed until
the DLC wheel and Grappling Hook are validated.

PRESERVED FROM V2A44
- V2A42/V2A44 stable IL2CPP lifetime and shutdown fencing
- five-tab menu
- Win32-only F1 cursor handling
- validated TimeManager pause/restore path
- Give All Weapons
- No Crosshair
- TimeScale
- Arsenal / Special exact-name WeaponId resolver
- base native weapon wheel
- existing hotkeys and INI persistence
- no EXE, GameAssembly.dll or UnityPlayer.dll patching

DXGI ASI LOADER
V2A50 includes dxgi.dll again.

The loader is intentionally minimal:
- forwards the real Windows System32\dxgi.dll
- loads PostalBorkenMenu.asi from the game directory
- initializes outside DllMain
- no Present hook
- no swap-chain vtable patching
- no renderer
- no second overlay
- no cursor implementation inside dxgi.dll

Expected log entries:
[DXGI LOADER] OK: System32 DXGI forwarding initialized.
[DXGI LOADER] OK: PostalBorkenMenu.asi loaded.

DLC WEAPON WHEEL TEST
The old V2A29 DLC wheel could display the game's native wheel shell, but the
live native button collection normally contained base-game WeaponIds rather
than the five gameplay DLC weapons.

V2A50 keeps the game's real PlayerWeaponWheelComponent and PlayerWheelView.

When the DLC wheel is opened, the mod:
1. verifies These Sunny Daze ownership
2. resolves only the five exact normal DLC WeaponIds
3. adds only DLC weapons that are not already collected
4. temporarily maps existing native WeaponWheelButton entries to those DLC WeaponIds
5. shows the game's own native wheel
6. restores every original WeaponId when the DLC wheel closes
7. calls the game's normal EnableButtons path after restoration

The five DLC wheel candidates are:
- WEAPON_UmDrill
- WEAPON_PissGun
- WEAPON_MeatShotgun
- WEAPON_BubbleGumMachineGun
- WEAPON_NuclearSyringe

Cutscene / NoWeapon special objects are deliberately excluded.

If the WeaponWheelButton class exposes a managed set_WeaponId method, V2A50
uses it. Otherwise it tries the corresponding managed IL2CPP field and verifies
the change again through get_WeaponId.

If no safe writable WeaponId target is found, V2A50 aborts the remap and restores
the native wheel instead of intentionally leaving a half-modified wheel.

GRAPPLING HOOK
The Special-menu entry is now displayed as:

Grappling Hook

Its underlying exact Unity WeaponId remains:
WEAPON_Hook

This preserves the V2A32-proven behavior:
- use the exact category 0 / slot 99 Hook WeaponId
- AddWeapon only when it is not already collected
- DO NOT call GiveAllWeapons as a Hook fallback
- DO NOT call EquipWeapon on WEAPON_Hook
- verify through WeaponsInventory.get_Hook()

The goal is to initialize the game's dedicated grappling-hook slot without
treating it as an ordinary gun.

TEST ORDER
1. Remove any other dxgi.dll / ASI loader from the game directory before this test.
2. Put the four V2A50 files next to the game EXE.
3. Launch the game normally without -debug.
4. Confirm the game reaches the menu and gameplay normally.
5. Open F1 and confirm V2A44 menu behavior is unchanged.
6. Run "Grappling Hook".
7. Confirm the grappling hook becomes usable without Give All Weapons and without
   forcing a normal weapon equip.
8. Bind "DLC Weapon Wheel" to a free key if it is not already bound.
9. Hold that key and inspect the native wheel.
10. Confirm DLC weapons appear in the wheel and can be selected normally.
11. Close/reopen the DLC wheel several times.
12. Open the normal Base Weapon Wheel afterward and confirm its original weapons
    were restored.
13. Change level once if practical and repeat the wheel/grappling tests.
14. Exit the game normally.
15. Keep PostalBorkenMenu.log if anything behaves unexpectedly.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not packaged.

NEXT PHASE AFTER VALIDATION
Once DLC wheel + Grappling Hook are stable:
- audit all exposed native/debug commands
- remove obsolete probes and test-only rows
- keep only useful player-facing features
- redesign the menu from a clean feature list instead of styling the current
  debug-heavy layout
