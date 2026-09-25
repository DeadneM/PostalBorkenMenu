PostalBorkenMenu V2A34 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A34 PURPOSE
Fix the V2A33 Weapon Catalog command dispatch bug.

V2A33 RESULT
The Weapon Catalog row was present in the Weapons tab, but pressing RUN reached the generic native-command path and logged:
[ERROR] Requested native command is unresolved

Cause:
- RunWeaponCatalog() existed.
- the UI row existed.
- the Weapons-tab mapping existed.
- but the active ExecuteCommandIndex() implementation in source/PostalBorkenMenu_part2.inc did not contain the __WeaponCatalog branch.

V2A34 CHANGE
Adds exactly:
__WeaponCatalog -> RunWeaponCatalog()

to the active command dispatcher.

No Weapon Catalog logic itself is changed.

WEAPON CATALOG
RUN "Weapon Catalog" in F1 -> Weapons.

It enumerates loaded Hyperstrange.PBD.WeaponId objects and logs:
- index
- UnityEngine.Object.name
- WeaponId.ToString() when available
- category
- category label
- slot
- collected YES / NO / UNKNOWN

The catalog is read-only.

HOOK RESULT PRESERVED
The latest user log also validates the V2A32 Hook isolation:
- category0 / slot99 candidate count = 1
- candidate not collected initially
- native AddWeapon(slot99) completed
- get_Hook() became NON-NULL
- _hookWeapon remained NULL
- get_Hook() returned the exact slot99 candidate
- category 0 / slot 99

Therefore AddWeapon of the unique category0/slot99 WeaponId is sufficient to expose the native Hook WeaponId without EquipWeapon or GiveAllWeapons.

PRESERVED
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell
- V2A25 Weapon List / Arguments
- ALT behavior
- V2A30 safe Hook probe
- V2A31 GiveAll Hook snapshots
- V2A32 slot99 Hook isolation
- V2A33 non-destructive Weapon Catalog implementation
- V2A8 clean shutdown
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is not included.

TEST
1. Start the game.
2. F1 -> Weapons -> Weapon Catalog -> RUN.
3. Send PostalBorkenMenu.log.
4. The expected block begins with:
   [WEAPON CATALOG] ===== BEGIN =====
and ends with:
   [WEAPON CATALOG] ===== END =====

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
