POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H53 POST-EVENT WHEEL SWAP TEST

BASE
----
H53 starts from H52.

H52 RESULT
----------
Normal campaign + DLC Weapon Wheel:
- the wheel opens,
- but no DLC weapons remain in it.

Cause:
the real InputManager.OnWheelDown path rebuilds the normal-campaign wheel AFTER
the H52 pre-event WeaponId swap, overwriting those five temporary DLC mappings.

H53 CHANGE
----------
H53 reverses the order.

For a cross-campaign wheel request:

1. Fire the validated native InputManager.OnWheelDown first.
2. Wait until Player.IsWheelEnabled == 1.
3. Only then apply the exact five-slot BASE<->DLC WeaponId swap.
4. Call WeaponWheelButton.Enable(true) on mapped buttons.
5. Call PlayerWeaponWheelComponent.EnableButtons() as a native refresh.
6. Keep the temporary mapping for the duration of the wheel.
7. On release, fire native OnWheelUp.
8. Restore the original five WeaponIds.

Exact pairs:
- WEAPON_Pistol <-> WEAPON_UmDrill
- WEAPON_Shovel <-> WEAPON_PissGun
- WEAPON_CatCanon <-> WEAPON_MeatShotgun
- WEAPON_Shotgun <-> WEAPON_BubbleGumMachineGun
- WEAPON_DildoBow <-> WEAPON_NuclearSyringe

UNCHANGED CASES
---------------
- Normal campaign + Base Weapon Wheel stays on the validated normal path.
- DLC + native DLC Weapon Wheel stays on the validated fast H51 path.

TARGETED METADATA
-----------------
H53 logs WeaponWheelButton method and field NAMES once, only after the first
post-event swap attempt.

No global metadata scan is used.

TEST
----
A. Normal campaign:
- Base Weapon Wheel should remain unchanged.
- DLC Weapon Wheel should open.
- Check whether the five DLC weapons now replace the five base counterparts.
- Note whether their real icons appear or placeholder silhouettes remain.

B. DLC:
- DLC Weapon Wheel should remain unchanged.
- Base Weapon Wheel should open.
- Check whether the five base counterparts replace the DLC-specific slots.

Send PostalBorkenMenu.log if the five mapped slots still have incorrect visuals.

Important lines:
[WHEEL H53] Wheel active. Applying BASE->DLC swap AFTER native setup.
[WHEEL H53] Wheel active. Applying DLC->BASE swap AFTER native setup.
[WHEEL H53] post-event mapped slots: 5
[WHEEL H53] EnableButtons refresh invoked after five-slot swap.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
