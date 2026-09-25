PostalBorkenMenu V2A32 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A32 PURPOSE
Use the V2A31 discovery to isolate the Hook without GiveAllWeapons.

V2A31 DISCOVERY
Immediately before native GiveAllWeapons():
- WeaponsInventory._hookWeapon = NULL
- WeaponsInventory.get_Hook() = NULL

Immediately after native GiveAllWeapons():
- WeaponsInventory._hookWeapon still = NULL
- WeaponsInventory.get_Hook() = NON-NULL
- returned WeaponId category = 0
- returned WeaponId slot = 99

This strongly indicates that get_Hook() resolves a special collected WeaponId rather than simply returning _hookWeapon.

V2A32 TEST
The menu row is now:
DLC Hook Slot99 Test

Sequence:
1. enforce These Sunny Daze ownership
2. run the safe get_Hook / InitHook observation from V2A30
3. if Hook remains null, scan loaded WeaponId objects
4. require exactly ONE candidate with:
   category = 0
   slot = 99
5. check whether that exact candidate is already collected
6. if not collected, invoke native WeaponsInventory.AddWeapon(candidate)
7. DO NOT call EquipWeapon
8. DO NOT call GiveAllWeapons
9. re-check get_Hook()
10. compare get_Hook() pointer against the exact slot99 candidate

SAFETY
- if zero or multiple category0/slot99 candidates are found, no mutation occurs
- if CollectedWeapons state cannot be inspected, no mutation occurs
- DLC ownership remains enforced
- no global Assembly-CSharp scan
- no automatic weapon equip
- no GiveAllWeapons
- no Weapon Wheel changes

PRESERVED
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell
- V2A25 Weapon List / Arguments
- ALT behavior
- V2A31 GiveAll snapshot logging
- V2A8 clean shutdown
- No Crosshair
- TimeScale

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

TEST
1. Prefer a fresh session before using Give All Weapons.
2. Run DLC Hook Slot99 Test once.
3. If the command reports SUCCESS, test the actual hook in gameplay.
4. Send PostalBorkenMenu.log.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
