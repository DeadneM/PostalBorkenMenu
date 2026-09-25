PostalBorkenMenu V2A26 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not shipped in this archive.

V2A26 PURPOSE
V2A25 proved that:
- the pure Hook probe no longer grants weapons
- WeaponsInventory.get_Hook() is null before the probe
- WeaponsInventory.InitHook() completes without exception
- get_Hook() is still null afterwards

Earlier V2A24 evidence also showed that after native GiveAllWeapons(), get_Hook() was already non-null.
Therefore InitHook() alone is not the creator of the usable hook. Some state prepared by the native GiveAllWeapons path is missing.

V2A26 DEEP HOOK DISCOVERY
The DLC Hook Deep Probe remains side-effect free with respect to weapon grants.

It now logs:
- exact return-type metadata for WeaponsInventory.get_Hook()
- every field name on PlayerInventoryComponent
- every field name on WeaponsInventory
- all Assembly-CSharp classes whose class/namespace contains:
  Hook / Grap / Rope / Cable / Tether
- all methods and fields on those matching classes
- methods/fields with those terms on other classes
- parameter counts
- method return types

Then it performs the same pure:
get_Hook() -> InitHook() -> get_Hook()
sequence.

It still does NOT call GiveAllWeapons.

TEST
1. Launch the game normally.
2. Open F1 -> WEAPONS.
3. Run DLC Hook Deep Probe once.
4. Do not run Give All Weapons first.
5. Send the generated PostalBorkenMenu.log.

The useful section will begin with:
[HOOK DEEP] ===== Assembly-CSharp Hook/Grap discovery begin =====

PRESERVED
- V2A25 Weapon List labels and Argument-based slots
- ALT behavior
- Base/DLC wheel experiment
- V2A8 shutdown/lifetime model
- No Crosshair
- TimeScale
- DLC ownership checks

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is not included.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
