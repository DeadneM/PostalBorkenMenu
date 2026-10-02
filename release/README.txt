POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H46 NATIVE DLC WHEEL ENTRY NO-AUDIT TEST

BASE
----
H46 starts directly from validated stable V2A42-H43.

H45 STATUS
----------
H45 is REJECTED because its broad Assembly-CSharp metadata audit crashed.

The uploaded H45 log ends during metadata enumeration of ItemWheelButton.Move,
before the actual DLC OnWheelDown test could be reached.

Therefore H45 did NOT disprove the native DLC entry hypothesis.

USEFUL H45 DISCOVERY
--------------------
Before crashing, H45 found on Hyperstrange.PBD.Player:
- get_IsWheelEnabled
- field OnWheelWeaponSelected
- field _playerWheelView

These reinforce that the wheel is routed through a real native/game input path.

H46 TEST
--------
H46 removes the entire global Wheel/DLC/PTSD metadata scan.

Inside a real DLC level, it only:

1. inspects the existing live wheel button categories;
2. if DLC/PTSD buttons already exist, leaves every button untouched;
3. calls PlayerWheelView.OnWheelDown() on press;
4. calls PlayerWheelView.OnWheelUp() on release.

No H8 remap.
No button mutation.
No SelectButton.
No UpdateSelection.
No global metadata scan.

EXPECTED DLC LAYOUT
-------------------
The real DLC level has already been observed with:
- 4 base-category buttons
- 5 DLC/PTSD-category buttons

That is the exact native 9-slot wheel shown in the user's screenshot.

TEST
----
In the actual These Sunny Daze DLC level:

1. Press and HOLD the mod's DLC Weapon Wheel shortcut.
2. Move through the wheel.
3. Release on another weapon.
4. Check whether the real native DLC wheel appears and whether selection equips.
5. Send PostalBorkenMenu.log.

Key lines:
[WHEEL H46] live native BASE buttons: 4
[WHEEL H46] live native DLC/PTSD buttons: 5
[WHEEL H46] TRUE native DLC layout detected. OnWheelDown() invoked with ZERO button mutation.
[WHEEL H46] TRUE native DLC OnWheelUp() completed. No button restoration needed.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
