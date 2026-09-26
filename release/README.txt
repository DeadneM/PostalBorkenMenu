PostalBorkenMenu V2A39 BISECT TEST
POSTAL: Brain-Damaged

BASE
Built directly from V2A38, which the user validated as working.

PURPOSE
Test only the V2A35 named-weapon dispatcher integration.

V2A39 CHANGE
ExecuteCommandIndex() now contains the V2A35 branch:

DecodeNamedWeaponCommand(...)
-> ExecuteNamedWeaponByUnityName(...)

This branch runs before the legacy Weapon List decoder.

CRITICAL SAFETY OF THIS TEST
There are still ZERO command rows beginning with:
- __Arsenal_
- __Special_

Therefore DecodeNamedWeaponCommand() should return FALSE for every existing command.
ExecuteNamedWeaponByUnityName() should never be called.

UNCHANGED FROM V2A38
- original V2A34 command table
- original four tabs
- original INI
- no Arsenal rows
- no Special rows
- no automatic WeaponId scan
- no automatic AddWeapon
- no automatic EquipWeapon
- no recovery code
- V2A34/V2A38 startup path
- V2A8 shutdown/lifetime behavior

EXPECTED LOG
[BISECT] V2A35 exact-name resolver linked; dispatcher branch enabled; no Arsenal/Special rows exist.

TEST
1. Launch normally.
2. Do not press F1 at first.
3. Confirm whether the game reaches the menu.
4. If stable, open F1 briefly and exit normally.
5. Send PostalBorkenMenu.log.

INTERPRETATION
- Stable: the resolver AND dispatcher integration are both innocent. Next suspect becomes the new Arsenal/Special command rows / enlarged g_cmds table.
- Crash: the dispatcher branch itself is implicated.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is not included.
