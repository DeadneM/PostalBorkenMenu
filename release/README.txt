PostalBorkenMenu V2A25 TEST
POSTAL: Brain-Damaged

IMPORTANT
- Remove -debug from the game's launch options.
- F1 opens/closes the PostalBorkenMenu overlay.
- PostalBorkenMenu.log is generated automatically at runtime and is not shipped in this archive.

V2A25 CHANGES

1. WEAPON LIST ARGUMENTS
The WEAPON LIST tab keeps the paired order:
1 Base Weapon | Argument 1
1 DLC Weapon  | Argument 1
2 Base Weapon | Argument 2
2 DLC Weapon  | Argument 2
...
9 Base Weapon | Argument 9
9 DLC Weapon  | Argument 9

The last number is now the actual Argument value shown in the Argument column.
It is no longer part of the command name.

Each row still has:
- RUN
- independent KEY binding

The Argument value is the slot actually sent to the weapon lookup.
Click the Argument cell to cycle slots 1 through 9.

2. DLC HOOK PROBE
User result from V2A24:
- the Hook command did not make the hook usable
- its GiveAllWeapons fallback only gave the weapon set

V2A25 removes GiveAllWeapons from the Hook path completely.

DLC Hook Probe now only:
- checks These Sunny Daze ownership
- audits PlayerInventoryComponent for Hook/Grap methods
- audits WeaponsInventory for Hook/Grap methods
- checks WeaponsInventory.get_Hook()
- if null, calls WeaponsInventory.InitHook()
- checks get_Hook() again

It does NOT call GiveAllWeapons and should not grant the normal weapon set.

If the hook is still unusable, run DLC Hook Probe once and send the generated PostalBorkenMenu.log. The new audit lines will be used to identify the actual native hook activation path.

PRESERVED
- V2A8 shutdown/lifetime stability model
- ALT behavior from V2A23
- Base and DLC weapon wheels
- DLC ownership checks
- No Crosshair
- TimeScale
- async hotkeys

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is not included.

PROJECT
https://github.com/DeadneM/PostalBorkenMenu
