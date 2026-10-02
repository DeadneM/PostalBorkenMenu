POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H45 TRUE NATIVE DLC WHEEL ENTRY TEST

BASE
----
H45 starts directly from validated stable V2A42-H43.

WHAT H44 PROVED
---------------
The H8-style remap can place the five exact DLC WeaponIds into the visible wheel:
- WEAPON_UmDrill
- WEAPON_PissGun
- WEAPON_MeatShotgun
- WEAPON_BubbleGumMachineGun
- WEAPON_NuclearSyringe

But the resulting wheel is not the true DLC wheel:
- DLC entries use placeholder/silhouette visuals instead of the game's full native artwork
- _selectedButton remains NULL throughout the held wheel
- H44 observed up to 84 passive probe ticks with no native selection

The supplied screenshots also show the difference clearly:
- native DLC wheel: 9 complete weapon visuals
- H8/H44 remap: a hybrid wheel with injected placeholder silhouettes

CRITICAL DLC-SCENE FINDING
--------------------------
Inside the real DLC level, the live PlayerWeaponWheelComponent already contains:
- 4 base-category buttons
- 5 DLC/PTSD-category buttons

Therefore the DLC scene itself has already configured the correct 9-button wheel.
H45 does NOT mutate those buttons.

H45 TEST
--------
When the DLC Weapon Wheel hotkey is pressed:

1. Inspect the live native wheel categories.
2. If at least one DLC/PTSD button already exists:
   - treat this as the real DLC-configured layout
   - call PlayerWheelView.OnWheelDown()
   - do not call Show()
   - do not alter any WeaponWheelButton
   - do not filter categories
3. On hotkey release:
   - call PlayerWheelView.OnWheelUp()
   - do not restore anything because nothing was modified

This is intended to reproduce the game's own wheel input entry/exit path.

OUTSIDE THE DLC SCENE
---------------------
If the live wheel contains zero DLC/PTSD buttons, H45 keeps the harmless H43 fallback shell.
No H8 remap is performed in that case.

METADATA AUDIT
--------------
On the first H45 DLC-wheel activation, the mod also performs a metadata-only audit of
Assembly-CSharp classes/methods containing Wheel / DLC / PTSD.

For matching methods it logs:
- class / namespace
- method name
- parameter count
- parameter types
- return type

This is to identify the exact DLC wheel configurator/setup path if OnWheelDown/OnWheelUp
alone are not enough.

BASE WHEEL
----------
The validated Base Weapon Wheel path is unchanged from H43.

NO FORCED SELECTION
-------------------
H45 does NOT call:
- SelectButton()
- UpdateSelection()
- OnPlayerWheelViewShow()
- OnPlayerWheelViewHide()

No direct WeaponId remap is used for the real DLC-scene path.

TEST
----
Inside the actual These Sunny Daze DLC level:

1. Bind the mod's DLC Weapon Wheel to a key.
2. HOLD it.
3. Move the mouse/stick around the wheel.
4. Release on a different weapon.
5. Check whether the real native DLC wheel appears and whether the selected weapon equips.
6. Send PostalBorkenMenu.log.

Important log lines:
[WHEEL H45] live native BASE buttons: 4
[WHEEL H45] live native DLC/PTSD buttons: 5
[WHEEL H45] TRUE native DLC layout detected. OnWheelDown() invoked with ZERO button mutation.
[WHEEL H45] TRUE native DLC OnWheelUp() completed. No button restoration needed.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
