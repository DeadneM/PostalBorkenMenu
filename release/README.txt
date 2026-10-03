POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H52 NATIVE PRE-EVENT WHEEL SWAP TEST

BASE
----
H52 starts from H51.

H51 RESULTS
-----------
User screenshots established three exact states:

1. Normal campaign + Base Weapon Wheel
   - full normal base wheel works.

2. DLC + Base Weapon Wheel
   - wheel opens through the native InputManager path
   - four weapons common to both campaigns are normal
   - the five DLC-replaced slots appear as disabled/pink silhouettes.

3. DLC + DLC Weapon Wheel
   - full native DLC wheel works correctly
   - no lag.

HISTORICAL H8 MAPPING
---------------------
Previous H8 diagnostics already proved the exact five substitutions:

WEAPON_Pistol        -> WEAPON_UmDrill
WEAPON_Shovel        -> WEAPON_PissGun
WEAPON_CatCanon      -> WEAPON_MeatShotgun
WEAPON_Shotgun       -> WEAPON_BubbleGumMachineGun
WEAPON_DildoBow      -> WEAPON_NuclearSyringe

H8 changed _weaponId and called PlayerWheelView.Show directly.
That produced placeholder silhouettes because the real native wheel setup path
was not known at that time.

H52 HYPOTHESIS
--------------
H49/H51 now proved the real wheel entry path:
InputManager.OnWheelDown / OnWheelUp.

Therefore H52 performs the exact five-slot WeaponId swap BEFORE the real native
OnWheelDown event, allowing the game itself to rebuild visuals/state/selection.

NORMAL CAMPAIGN + DLC WHEEL
---------------------------
H52:
- resolves the exact five DLC WeaponIds
- ensures those five weapons are collected
- remaps only the five known base slots to their DLC counterparts
- fires the real InputManager.OnWheelDown
- leaves the four common slots untouched
- on release fires OnWheelUp
- restores the five original base WeaponIds

DLC + BASE WHEEL
----------------
H52 performs the inverse:
- resolves the exact five base WeaponIds
- ensures those five base weapons are collected
- remaps the five DLC slots back to their base counterparts
- fires native OnWheelDown
- on release fires OnWheelUp
- restores the original DLC WeaponIds

DLC + DLC WHEEL
---------------
The already-correct native DLC configuration is left untouched.

NORMAL CAMPAIGN + BASE WHEEL
----------------------------
The validated H43/H51 normal base wheel path is left untouched.

NO HEAVY DIAGNOSTIC PATH
------------------------
H52 keeps the fast InputManager event bridge.
No global metadata scan is performed.

EXPECTED TEST
-------------
A. Normal campaign:
- Base Weapon Wheel should remain unchanged.
- DLC Weapon Wheel should now open and ideally display the real DLC weapon art,
  not silhouettes.

B. DLC:
- DLC Weapon Wheel should remain unchanged and lag-free.
- Base Weapon Wheel should now contain the full base-game replacements instead
  of the five pink disabled silhouettes.

Important lines:
[WHEEL H52] BASE->DLC slots remapped before native event: 5
[WHEEL H52] DLC->BASE slots remapped before native event: 5
[WHEEL H52] DLC wheel opened in NORMAL campaign via pre-event five-slot swap.
[WHEEL H52] Full BASE configuration injected before native OnWheelDown in DLC.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
