PostalBorkenMenu V2A24 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not shipped in this archive.

V2A24 CHANGES

1. CLEAN WEAPONS TAB
The following legacy rows were removed from the menu:
- Equip DLC Weapon
- Equip Base Weapon
- Base Weapon 1
- Base Weapon 2
- DLC Weapon 1
- DLC Weapon 2

The WEAPONS tab now keeps the global weapon systems such as:
- Give All
- Give All Weapons
- Give All DLC Weapons
- Give / Init DLC Hook
- Weapon Keys Mode
- Base Weapon Wheel
- DLC Weapon Wheel

2. NEW WEAPON LIST TAB
A fourth tab named WEAPON LIST was added.

Rows are fixed in this order:
1 Base Weapon 1
1 DLC Weapon 1
2 Base Weapon 2
2 DLC Weapon 2
3 Base Weapon 3
3 DLC Weapon 3
4 Base Weapon 4
4 DLC Weapon 4
5 Base Weapon 5
5 DLC Weapon 5
6 Base Weapon 6
6 DLC Weapon 6
7 Base Weapon 7
7 DLC Weapon 7
8 Base Weapon 8
8 DLC Weapon 8
9 Base Weapon 9
9 DLC Weapon 9

Every row has its own RUN action and its own assignable KEY.
There is no editable slot argument anymore for these rows.

Execution uses the same safe native WeaponId path:
- if the target is not collected: AddWeapon then EquipWeapon
- if already collected: skip duplicate AddWeapon, but EquipWeapon is still allowed
- DLC ownership checks remain enforced

3. DLC HOOK AUDIT
The previous InitHook-only attempt did not make the DLC hook usable.

V2A24 keeps the native attempt but now:
- runs the validated PlayerInventoryComponent.GiveAllWeapons() path first
- scans PlayerInventoryComponent methods for names containing Hook or Grap
- scans WeaponsInventory methods for names containing Hook or Grap
- logs each matching method and its parameter count
- calls the known WeaponsInventory.InitHook() path
- checks get_Hook() again

This is intentionally a diagnostic step. If the hook still does not work, run Give / Init DLC Hook once and send the newly generated PostalBorkenMenu.log. The log should expose the actual game-side Hook/Grap API so the next build can target it directly.

PRESERVED
- V2A8 shutdown/lifetime stability model
- No Crosshair visual-only behavior
- TimeScale toggle behavior
- async hotkeys
- ALT weapon mode from V2A23
- Base and DLC weapon wheels from V2A22
- normal These Sunny Daze ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
