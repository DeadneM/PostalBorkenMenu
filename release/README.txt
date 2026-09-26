PostalBorkenMenu V2A37 UI-ONLY BISECT TEST
POSTAL: Brain-Damaged

BASE
Built directly from V2A34 Weapon Catalog Dispatch Fix.

PURPOSE
Test only the five-tab overlay structure.

TABS
Gameplay
Weapons
Arsenal
Special
Visual

IMPORTANT
- Arsenal is intentionally empty.
- Special is intentionally empty.
- No named WeaponId rows are present.
- The legacy V2A34 Weapon List rows remain under Weapons so no existing command behavior is removed.

NO NEW WEAPON LOGIC
V2A37 adds no:
- exact-name WeaponId lookup
- AddWeapon path
- EquipWeapon path
- automatic WeaponId scan
- automatic Hook recovery
- startup FindObjectOfType polling
- new DLC ownership logic
- new INI weapon entries

PRESERVED EXACTLY FROM V2A34
- Weapon Catalog
- V2A29 wheel behavior
- Weapon List slot rows and Arguments
- Weapon Keys Mode
- ALT behavior
- Hook probes already present in V2A34
- V2A8 shutdown/lifetime behavior
- No Crosshair
- TimeScale
- native developer commands

FUNCTIONAL DELTA
Only source/PostalBorkenMenu_part3.inc changes behavior:
- tab count 4 -> 5
- tab labels/layout
- click regions
- legacy Weapon List rows remapped from old tab 2 into Weapons tab
- Visual remapped from tab 3 to tab 4
- Arsenal tab 2 contains zero commands
- Special tab 3 contains zero commands

TEST
1. Start the game normally.
2. Do not run any command yet.
3. Confirm whether the game reaches the menu without crashing.
4. Press F1.
5. Click all five tabs.
6. Confirm Arsenal and Special are empty.
7. Exit normally.
8. Send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is not included.
