POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H56 WHEEL BUTTON MIRROR AUDIT

BASE
----
H56 starts from H55.

BEHAVIOR
--------
No wheel behavior change from H55/H54.

Known current state:
- Normal campaign + Base Weapon Wheel: works.
- DLC + DLC Weapon Wheel: works and is lag-free.
- Normal campaign + DLC Weapon Wheel: the five weapons that differ from base are still missing visually.
- DLC + Base Weapon Wheel: the five weapons that differ from DLC are still missing visually.

This symmetrical result strongly suggests that the two game modes use different native configuration for exactly five wheel buttons.

H56 PURPOSE
-----------
Capture both native 9-button layouts in a directly comparable form.

For each button H56 logs:
- collection index
- Unity object name
- _id declared type
- _id numeric value when safely readable
- WeaponId name
- WeaponId category
- WeaponId slot
- Enabled state
- _weaponSelectedImage sprite name
- _backgroundImage sprite name

H56 records snapshots only for the two known native signatures:
- Normal layout: 9 base / 0 DLC
- DLC layout: 4 base / 5 DLC

Each signature is logged once per launch.

WHY THIS MATTERS
----------------
The next functional fix should no longer guess.

The mirror snapshots will tell us whether the five mode-specific weapons are represented by:
- the same button indices but different _id values,
- different sprites on the same buttons,
- different WeaponId slots,
- or a different button configuration altogether.

TEST
----
Please do two short runs if possible.

RUN A - Normal campaign:
1. Open Base Weapon Wheel once.
2. Open DLC Weapon Wheel once if desired.
3. Close game.

RUN B - DLC:
1. Open DLC Weapon Wheel once.
2. Open Base Weapon Wheel once if desired.
3. Close game.

Send both PostalBorkenMenu.log files, or at minimum the log from each mode.

Key section:
[WHEEL H56] NATIVE WHEEL MIRROR SNAPSHOT

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
