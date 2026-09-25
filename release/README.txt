PostalBorkenMenu V2A29 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A29 PURPOSE
Use the V2A20 weapon-wheel behavior as the reference.

BASE WEAPON WHEEL
The Base Weapon Wheel block is transplanted directly from V2A20:
- GetBaseWheelButtonsCollection()
- GetBaseWheelButtonFromCollection()
- ShowNativeBaseWeaponWheel()
- HideNativeBaseWeaponWheel()
- ExecuteNativeBaseWheel()

This is the wheel path that can still display its native wheel structure before the player has collected weapons, because the game's live wheel buttons already exist.

DLC WEAPON WHEEL
The DLC wheel keeps the same split state / HOLD interaction as the later branch, but follows the V2A20 display philosophy.

It:
1. restores the game's native EnableButtons() state
2. filters live buttons to category 1 / PTSD when possible
3. calls PlayerWheelView.Show() even when zero DLC buttons are currently present
4. therefore allows an EMPTY DLC wheel shell to be displayed for testing
5. restores the native button state on close

It does NOT:
- call GiveAllWeapons()
- call the Hook probe
- automatically add any weapon
- treat zero DLC buttons as a fatal error

PRESERVED
- V2A25 Weapon List with visible Argument slots 1..9
- ALT behavior
- V2A26 Hook Deep Probe
- V2A8 shutdown/lifetime model
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is not included.

TEST PRIORITY
1. Start with a save / situation where no weapon is collected if possible.
2. Hold Base Weapon Wheel and confirm it displays.
3. Hold DLC Weapon Wheel and confirm an empty wheel shell can display instead of failing.
4. Release each key and confirm the view closes cleanly.
5. Confirm F1 and normal weapon selection still work.
6. Confirm clean game exit.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
