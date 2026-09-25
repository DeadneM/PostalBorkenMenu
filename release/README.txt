PostalBorkenMenu V2A36 RECOVERY TEST
POSTAL: Brain-Damaged

IMPORTANT
- This build is a recovery candidate, built from V2A34.
- Remove -debug from launch options.
- PostalBorkenMenu.log is generated automatically at runtime and is not included in the ZIP.

WHY V2A36 EXISTS
After the V2A32 Hook isolation test, the exact WeaponId:
WEAPON_Hook
category 0 / slot 99
was added to PlayerInventoryComponent.CollectedWeapons with native AddWeapon().

The following launch tests then crashed after the intro/profile transition even after rolling back from V2A35 to V2A34.

V2A36 tests the narrow hypothesis that WEAPON_Hook persisted in the player profile and is now being loaded at an unsafe point.

RECOVERY BEHAVIOR
V2A36 starts from V2A34 and adds one startup cleanup window.

Before creating the overlay or subclassing the game window, it waits up to 20 seconds for PlayerInventoryComponent to exist.

Once available it:
1. resolves the exact Unity WeaponId named WEAPON_Hook
2. requires exactly one matching object
3. reads PlayerInventoryComponent.get_CollectedWeapons()
4. checks whether the exact WEAPON_Hook object is in that collection
5. if absent, reports that the profile is already clean and changes nothing
6. if present, resolves the collection's native Remove(one argument) method
7. calls Remove(WEAPON_Hook) only
8. verifies the exact Hook object is no longer present

V2A36 DOES NOT
- call GiveAllWeapons
- call GiveAllDlcWeapons
- call AddWeapon
- call EquipWeapon
- call InitHook
- modify _hookWeapon
- modify Weapon Wheels
- modify save files directly
- enumerate Assembly-CSharp classes globally

EXPECTED LOG
[RECOVERY] Startup Hook cleanup window armed.
[RECOVERY] Exact WEAPON_Hook matches: 1

Then either:
[RECOVERY] WEAPON_Hook is not in CollectedWeapons. Profile is already clean.

or:
[RECOVERY] WEAPON_Hook found in CollectedWeapons. Invoking collection Remove(WEAPON_Hook) ONLY.
[RECOVERY] Collection Remove() returned TRUE.
[RECOVERY] SUCCESS: WEAPON_Hook removed from CollectedWeapons.

TEST
1. Replace the current ASI/INI with the files in this archive.
2. Start the game normally.
3. Do not press F1 and do not use Hook Probe.
4. Check whether the game gets past the intro/profile transition.
5. Send PostalBorkenMenu.log.

If recovery succeeds and the game reaches the menu, exit the game normally once before further testing.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
