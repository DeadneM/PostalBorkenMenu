PostalBorkenMenu V2A30 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A30 PURPOSE
Keep the V2A29 wheel behavior exactly as validated, and fix the crashing Hook probe.

V2A29 WHEELS PRESERVED
- Base Weapon Wheel uses the V2A20 wheel implementation.
- DLC Weapon Wheel can show an empty native wheel shell.
- no GiveAllWeapons fallback
- no automatic Hook call from either wheel

WHY THE OLD PROBE CRASHED
The V2A26/V2A29 Hook Deep Probe enumerated classes and members across all of Assembly-CSharp.
The user log shows that scan starting successfully, then terminating mid-class before the end marker.
The broad substring search also matched unrelated names such as Graphic / GatherProperties, causing a huge unsafe metadata walk.

V2A30 SAFE HOOK PROBE
The global Assembly-CSharp scan is completely disabled.

The probe now only touches known objects:
- PlayerInventoryComponent._weaponsController
- WeaponsInventory._hookWeapon
- WeaponsInventory.get_Hook()
- WeaponsInventory.InitHook()
- WeaponId.get_Category()
- WeaponId.get_Slot()

Sequence:
1. verify DLC ownership
2. locate the live WeaponsInventory
3. read _hookWeapon
4. call get_Hook()
5. log category/slot if a Hook WeaponId already exists
6. if no Hook exists, call InitHook() once
7. read _hookWeapon and get_Hook() again
8. log category/slot if available

It does NOT:
- enumerate Assembly-CSharp globally
- call GiveAllWeapons()
- add any weapon
- equip any weapon

IMPORTANT DISCOVERY FROM THE CRASH LOG
- WeaponsInventory.get_Hook() returns Hyperstrange.PBD.WeaponId
- WeaponsInventory contains a field named _hookWeapon

This is now the focus of the investigation.

PRESERVED
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell behavior
- V2A25 Weapon List with visible Argument slots 1..9
- ALT behavior
- V2A8 shutdown/lifetime model
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

TEST
1. Confirm both weapon wheels still behave exactly as in V2A29.
2. Run DLC Hook Safe Probe once.
3. Confirm no crash.
4. Send PostalBorkenMenu.log if _hookWeapon/get_Hook remain null.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
