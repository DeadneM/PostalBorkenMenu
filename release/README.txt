POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H44 DLC WHEEL SELECTION OBSERVER TEST

BASE
----
H44 starts from validated stable V2A42-H43.

PRESERVED
---------
- H39 native Unity startup-logo skip
- Skip Startup Logos menu toggle
- H43 safe menu cleanup
- Base Weapon Wheel path unchanged
- No Crosshair unchanged
- Grappling Hook unchanged
- Arsenal / Special unchanged
- shutdown/lifetime behavior unchanged
- one-launch-one-log diagnostics

HISTORICAL DLC WHEEL FACTS
--------------------------
H7/H8 proved:
- exact five gameplay DLC WeaponIds can be loaded:
  WEAPON_UmDrill
  WEAPON_PissGun
  WEAPON_MeatShotgun
  WEAPON_BubbleGumMachineGun
  WEAPON_NuclearSyringe
- WeaponWheelButton._weaponId is the correct object field
- il2cpp_field_set_value_object can remap that field safely
- get_WeaponId verifies each write
- a visible native wheel can be opened with those temporary DLC WeaponIds

H9 proved:
- _selectedButton was NULL when sampled only at wheel close

H10-H12 are rejected:
- OnWheelDown/OnWheelUp or forced controller state caused visibility/regression problems
- forced UpdateSelection/SelectButton did not provide a safe final architecture
- H12 regressed Base Weapon Wheel / No Crosshair

H44 GOAL
--------
H44 does NOT force selection.

It restores only the visually proven H8 temporary button remap on top of H43,
then passively watches the native PlayerWeaponWheelComponent._selectedButton
while the DLC wheel key is HELD.

The observer runs on the game thread approximately every 50 ms.

It logs only when _selectedButton changes.

If a selected button appears, H44 logs:
- mapped DLC button index
- exact selected WeaponId name

At close it logs:
- total observer ticks
- number of _selectedButton changes

H44 also dumps PlayerWeaponWheelComponent / PlayerWheelView method and field names
once, on the first DLC wheel open.

NO FORCED SELECTION
-------------------
H44 does NOT call:
- SelectButton()
- UpdateSelection()
- OnWheelDown()
- OnWheelUp()
- OnPlayerWheelViewShow()
- OnPlayerWheelViewHide()

It does not equip a weapon on close.

The permanent base wheel mapping is restored after the DLC wheel closes.

TEST
----
1. Bind DLC Weapon Wheel to a key if needed.
2. HOLD the DLC wheel key.
3. While holding, move the mouse / stick clearly through several wheel sectors.
4. Keep it open for a few seconds.
5. Release the key.
6. Repeat once if useful.
7. Send PostalBorkenMenu.log.

Key H44 lines:
[WHEEL H44] Native buttons remapped to DLC weapons: N
[WHEEL H44] _selectedButton changed -> mapped index: N
[WHEEL H44] selected WeaponId:
[WHEEL H44] probe ticks while held: N
[WHEEL H44] _selectedButton changes observed: N

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
