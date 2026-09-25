PostalBorkenMenu V2A28 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A28 PURPOSE
Restore the exact V2A18 dual native weapon-wheel subsystem.

THIS IS NOT THE V2A27 REIMPLEMENTATION.

V2A18 WHEEL CODE RESTORED
The following pieces are transplanted from dev/v2a18-dual-native-wheel:

- one shared WM_APP_WHEELBANK message
- one shared g_activeWheelBank state
- one shared g_activeWheelVk state
- ShowNativeWeaponWheelBank(category)
- HideNativeWeaponWheelBank()
- ExecuteNativeWheelBank(bank, show)
- exact V2A18 HOLD / RELEASE hotkey handling

Base Weapon Wheel:
- bank 1
- filters native _weaponWheelButtons to category 0

DLC Weapon Wheel:
- bank 2
- filters the same native _weaponWheelButtons collection to category 1 / PTSD

On close:
- PlayerWeaponWheelComponent.EnableButtons() restores native button state

NO V2A22/V2A27 WHEEL FALLBACKS
The restored V2A18 wheel subsystem does not:
- call GiveAllWeapons automatically
- call the Hook probe automatically
- keep separate base/dlc wheel active-state variables
- use the later split WM_APP_BASEWHEEL / WM_APP_DLCWHEEL handlers

PRESERVED FROM CURRENT LINE
- V2A25 WEAPON LIST with visible Argument slots 1..9
- ALT behavior
- V2A26 Hook Deep Probe
- V2A8 shutdown/lifetime stability
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

TEST PRIORITY
1. Bind Base Weapon Wheel and DLC Weapon Wheel to two different keys.
2. Hold Base key, release.
3. Hold DLC key, release.
4. Confirm no crash.
5. Confirm F1/menu still works.
6. Confirm clean game exit.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
