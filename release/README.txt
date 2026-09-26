PostalBorkenMenu V2A38 BISECT TEST
POSTAL: Brain-Damaged

BASE
Built directly from V2A34 Weapon Catalog Dispatch Fix, the last version with a complete known-good run.

PURPOSE
Isolate the V2A35 exact-name weapon resolver from every other V2A35 change.

V2A38 ADDS
The V2A35 functions:
- DecodeNamedWeaponCommand()
- ExecuteNamedWeaponByUnityName()

are compiled and retained in the ASI.

The resolver contains the same V2A35 exact-name path:
- Resources.FindObjectsOfTypeAll(WeaponId)
- UnityEngine.Object.get_name()
- exact Unity name matching
- category/slot logging
- ownership check for category 1
- duplicate-safe AddWeapon + EquipWeapon
- special Hook AddWeapon-only path

BUT THEY ARE DORMANT.

V2A38 DOES NOT ADD
- any __Arsenal_* command rows
- any __Special_* command rows
- any new tab
- any new INI weapon entries
- any ExecuteCommandIndex() branch for exact-name weapons
- any automatic WeaponId scan
- any automatic AddWeapon
- any automatic EquipWeapon
- any automatic Hook action
- any startup recovery

A volatile function pointer is read only to ensure the linker keeps the resolver in the binary.
The resolver itself is never called.

EXPECTED LOG
[BISECT] V2A35 exact-name resolver is linked but dormant; no named-weapon dispatch exists.

TEST
1. Launch the game normally.
2. Do not press F1 initially.
3. Check whether it reaches the menu.
4. If stable, open F1 and verify the original V2A34 four-tab layout.
5. Exit normally.
6. Send PostalBorkenMenu.log.

INTERPRETATION
- Stable: the exact-name resolver code itself is innocent; the crash is caused by its integration, command rows, or later V2A35 UI/dispatch changes.
- Crash: the mere linked presence of the resolver changes the binary enough to reproduce the fault, so we narrow further inside this block.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is not included.
