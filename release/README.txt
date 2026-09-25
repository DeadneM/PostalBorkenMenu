PostalBorkenMenu V2A31 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A31 PURPOSE
Preserve the validated V2A29 wheel behavior and the crash-safe V2A30 hook probe, then observe exactly what native GiveAllWeapons() does to the hook state.

RESULT FROM V2A30
- Base Weapon Wheel still opens/closes correctly.
- DLC Weapon Wheel still opens as an empty native shell when no DLC/PTSD wheel button exists.
- DLC Hook Safe Probe no longer crashes.
- _hookWeapon = null before InitHook().
- get_Hook() = null before InitHook().
- after InitHook(), both values remain null.
- therefore InitHook() alone does not create or assign the Hook WeaponId.

V2A31 CHANGE
Give All Weapons remains the same validated native PlayerInventoryComponent.GiveAllWeapons() command.

The only addition is targeted logging immediately before and immediately after that native call.

Each snapshot logs:
- WeaponsInventory._hookWeapon null / non-null
- WeaponsInventory.get_Hook() null / non-null
- whether both references are identical
- Hook WeaponId category if present
- Hook WeaponId slot if present

This is observation only.

NO NEW MUTATION
V2A31 does not:
- globally scan Assembly-CSharp
- add an extra weapon
- equip an extra weapon
- call InitHook automatically from Give All Weapons
- alter either Weapon Wheel

TEST
1. Start a session where Hook is not already initialized if possible.
2. Run DLC Hook Safe Probe once.
3. Run Give All Weapons once.
4. Send PostalBorkenMenu.log.
Look for:
[GIVEALL HOOK] BEFORE GiveAllWeapons
[GIVEALL HOOK] AFTER GiveAllWeapons

PRESERVED
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell
- V2A25 Weapon List / Arguments
- ALT behavior
- V2A30 safe Hook probe
- V2A8 clean shutdown
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is not included.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
