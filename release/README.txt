PostalBorkenMenu V2A41 NAMED ROWS ONLY BISECT
POSTAL: Brain-Damaged

BASE
Built directly from V2A40, validated working by the user.

PURPOSE
Isolate the effect of adding the V2A35 named Arsenal/Special command rows to g_cmds.

V2A41 CHANGE
Adds exactly:
- 26 __Arsenal_* command rows
- 8 __Special_* command rows

Total new named rows: 34.

The original 18 V2A34 Weapon List rows are intentionally still present.
This makes CMD_COUNT 76 for this diagnostic build.

IMPORTANT
The UI is still the original four-tab V2A40 layout.
No five-tab UI changes are included.
The INI is unchanged from V2A40.
The V2A40 three-second stable-window subclass fix is preserved.

Because CommandMenuTab() has not yet been taught about Arsenal/Special,
the new rows would fall through to the Gameplay tab if F1 is opened.
For this diagnostic test, DO NOT open F1 during startup.

PRESERVED
- V2A40 stable game-window subclass
- V2A39 exact-name dispatcher branch
- V2A38 exact-name resolver
- V2A34 legacy Weapon List
- V2A29 wheel behavior
- V2A8 shutdown/lifetime fencing

NO OTHER NEW BEHAVIOR
- no new tab mapping
- no INI additions
- no automatic WeaponId scan
- no automatic AddWeapon/EquipWeapon
- no recovery code

TEST
1. Launch the game.
2. Do not press F1 or any mod hotkey.
3. Confirm whether the game reaches the menu and stays stable.
4. If stable, exit normally.
5. Send PostalBorkenMenu.log.

INTERPRETATION
- Stable: the extra g_cmds rows themselves are not the crash source. Next test is the five-tab routing/UI.
- Crash: the command-table expansion / startup handling of those rows is implicated.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is not included.
