PostalBorkenMenu V2A35 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in this archive.

V2A35 PURPOSE
Replace the old ambiguous slot-only Weapon List with exact named WeaponId entries and split them into two dedicated tabs.

NEW OVERLAY TABS
Gameplay
Weapons
Arsenal
Special
Visual

ARSENAL
The old generic rows:
1 Base Weapon / 1 DLC Weapon ... 9 Base Weapon / 9 DLC Weapon

are removed from the UI.

The Arsenal tab now exposes exact Unity WeaponId names:

BASE
- Shovel
- Pistol
- Pistol Akimbo
- Shotgun
- Shotgun Akimbo
- Machine Gun
- Machine Gun Akimbo
- Rocket Launcher
- Rocket Launcher Akimbo
- Lightning Gun
- Lightning Gun Akimbo
- Gatling Gun
- Gatling Gun Akimbo
- Dildo Bow
- Dildo Bow Akimbo
- Cat Canon
- Cat Canon Akimbo

PTSD / THESE SUNNY DAZE
- Um Drill
- Piss Gun
- Piss Gun Akimbo
- Meat Shotgun
- Meat Shotgun Akimbo
- Bubble Gum MG
- Bubble Gum MG Akimbo
- Nuclear Syringe
- Nuclear Syringe Akimbo

Each row resolves the exact UnityEngine.Object.name at runtime.
It no longer selects the first WeaponId sharing a category/slot pair.

For normal weapons:
- if not collected, native AddWeapon(exact WeaponId) is used
- native EquipWeapon(exact WeaponId) is then used
- duplicate AddWeapon is avoided
- DLC ownership remains enforced for category 1 WeaponIds

SPECIAL
The new Special tab exposes the internal/special WeaponIds discovered by Weapon Catalog:
- Dong
- Dong Confusion
- Dong Fire
- Dong Ice
- Hook (Add Only)
- Cutscene Weapon DLC
- No Weapon
- No Weapon DLC

HOOK SPECIAL CASE
V2A32 and the subsequent user log proved:
- WEAPON_Hook is category 0 / slot 99
- adding that exact WeaponId to CollectedWeapons is enough for get_Hook() to expose it
- EquipWeapon is not required
- _hookWeapon may remain NULL

Therefore the Special -> Hook command:
- resolves exact WEAPON_Hook by Unity name
- adds it only if missing
- deliberately does NOT call EquipWeapon
- verifies get_Hook() afterwards

OTHER SPECIAL OBJECTS
Other Special rows use exact-name AddWeapon/EquipWeapon for testing.
They are internal objects and may have unusual gameplay behavior.

WEAPON CATALOG
Still available in the Weapons tab and remains read-only.

PRESERVED
- V2A29 V2A20-style Base Weapon Wheel
- V2A29 empty DLC wheel shell
- V2A30 safe Hook probe
- V2A31 GiveAll Hook snapshots
- V2A32 slot99 Hook isolation
- V2A33/V2A34 Weapon Catalog
- ALT slot key behavior
- Weapon Keys Mode
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
2. Open F1.
3. Check the new Arsenal and Special tabs.
4. Try individual named weapons.
5. For unusual entries such as Dong variants, Cutscene Weapon, No Weapon, or No Weapon DLC, test one at a time.
6. Send PostalBorkenMenu.log with the results.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
