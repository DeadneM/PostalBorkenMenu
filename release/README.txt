POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H54 DLC WHEEL NORMAL-ONLY TEST

BASE
----
H54 starts from H51.

WHY H54
-------
H53 is rejected because it regressed Base Weapon Wheel inside the DLC.

H54 therefore freezes the entire H51 Base Weapon Wheel implementation.

Verification:
- the Show/Hide/Delayed Base Weapon Wheel block is byte-identical to H51
- no H53 inverse DLC->BASE swap code is present in the Base Weapon Wheel path

PRESERVED H51 BEHAVIOR
----------------------
Normal campaign + Base Weapon Wheel:
- unchanged

DLC + Base Weapon Wheel:
- unchanged from H51
- opens through the validated H51 path
- known limitation remains: DLC-replaced slots may appear as silhouettes

DLC + DLC Weapon Wheel:
- unchanged from H51
- full native DLC wheel
- fast InputManager path
- no diagnostic lag

H54 EXPERIMENT ONLY
-------------------
Only this case is modified:

Normal campaign + DLC Weapon Wheel.

H54 sequence:

1. Fire the validated native InputManager.OnWheelDown.
2. Wait until Player.IsWheelEnabled == 1.
3. Resolve the exact five DLC WeaponIds:
   - WEAPON_UmDrill
   - WEAPON_PissGun
   - WEAPON_MeatShotgun
   - WEAPON_BubbleGumMachineGun
   - WEAPON_NuclearSyringe
4. Ensure the five DLC weapons are collected.
5. Replace only these five normal-campaign slots:
   - WEAPON_Pistol
   - WEAPON_Shovel
   - WEAPON_CatCanon
   - WEAPON_Shotgun
   - WEAPON_DildoBow
6. Enable the five mapped buttons.
7. Call PlayerWeaponWheelComponent.EnableButtons().
8. On release, call native OnWheelUp and restore the original five base WeaponIds.

TARGETED METADATA
-----------------
If the post-open swap succeeds, H54 logs WeaponWheelButton method and field names once.

No global metadata scan is used.

TEST
----
First verify regression protection:
1. DLC + Base Weapon Wheel must behave exactly like H51.
2. DLC + DLC Weapon Wheel must remain full and lag-free.

Then test:
3. Normal campaign + DLC Weapon Wheel.

If the five DLC weapons appear but icons are wrong, send PostalBorkenMenu.log.
If they do not appear, send the log as well.

Important lines:
[WHEEL H54] Normal campaign wheel opened; DLC-only post-event swap pending.
[WHEEL H54] exact DLC targets found: 5
[WHEEL H54] DLC targets collected/ready: 5
[WHEEL H54] BASE->DLC slots swapped AFTER native open: 5
[WHEEL H54] DLC-only normal-campaign swap applied. Base-wheel code untouched.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
