PostalBorkenMenu V2A27 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not shipped in this archive.

V2A27 PURPOSE
Restore the older dual-wheel behavior.

WEAPONS TAB
- Base Weapon Wheel: native base-game weapon wheel.
- DLC Weapon Wheel: classic native wheel filtered to DLC/PTSD category.

RESTORED CLASSIC DLC WHEEL RULE
The DLC wheel no longer performs any fallback action.

It does NOT:
- call GiveAllWeapons()
- call InitHook()
- add weapons automatically
- modify the inventory if no DLC buttons exist

It only:
1. restores the game's native wheel-button state
2. scans the live _weaponWheelButtons collection
3. enables only category 1 / PTSD buttons
4. opens the native PlayerWheelView if at least one DLC button exists

If no DLC button exists in the live native collection, the command fails cleanly and restores the native button state.

PRESERVED
- V2A25 Weapon List with visible Argument slots 1..9
- ALT behavior from V2A23
- V2A26 DLC Hook Deep Probe
- V2A8 shutdown/lifetime model
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
