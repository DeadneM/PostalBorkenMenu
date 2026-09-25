PostalBorkenMenu V2A33 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A33 PURPOSE
Start strictly from V2A32 and add one new read-only function:
Weapon Catalog

WEAPON CATALOG
The new Weapon Catalog row is in the Weapons tab.

When RUN is pressed, it enumerates loaded Hyperstrange.PBD.WeaponId objects with:
UnityEngine.Resources.FindObjectsOfTypeAll(WeaponId)

For every loaded WeaponId it logs:
- scan index
- UnityEngine.Object.name
- WeaponId.ToString() when available
- numeric category
- category label:
  0 = BASE
  1 = PTSD / These Sunny Daze
  other = OTHER
- native slot
- whether that exact WeaponId is already in PlayerInventoryComponent.CollectedWeapons:
  YES / NO / UNKNOWN

Example log shape:
[WEAPON CATALOG] #12 | name=<Unity name> | toString=<managed text> | category=0(BASE) | slot=5 | collected=YES

SAFETY
Weapon Catalog is observation-only.

It does NOT:
- AddWeapon
- EquipWeapon
- GiveAllWeapons
- GiveAllDlcWeapons
- InitHook
- modify _hookWeapon
- modify the Weapon Wheels

The IL2CPP string helper exports used only for catalog text are optional.
If unavailable, startup behavior remains unchanged and names may appear as <empty>.

PRESERVED FROM V2A32
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell
- V2A25 Weapon List / Arguments
- ALT behavior
- V2A30 safe Hook probe
- V2A31 GiveAll Hook snapshots
- V2A32 category0 / slot99 Hook isolation test
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
1. Start the game normally.
2. Open F1 -> Weapons.
3. RUN "Weapon Catalog".
4. Send PostalBorkenMenu.log.
5. Search for:
   [WEAPON CATALOG] ===== BEGIN =====
   ...
   [WEAPON CATALOG] ===== END =====

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
