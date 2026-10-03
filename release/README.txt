POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H50 BOTH WHEEL HOTKEYS DLC TEST

Base: H49.

H49 validated the real DLC wheel input path:
- InputManager.OnWheelDown is a static System.Action
- InputManager.OnWheelUp is a static System.Action
- both Invoke() successfully
- the native DLC wheel becomes visible
- Player.IsWheelEnabled becomes active while held and returns to 0 on release

H50 keeps the H49 DLC Weapon Wheel path unchanged.

H50 also fixes Base Weapon Wheel inside the DLC:
- non-base buttons are temporarily disabled as before
- in a DLC-configured scene, opening now uses InputManager.OnWheelDown
- closing uses InputManager.OnWheelUp
- native buttons are then restored
- outside the DLC, the H43 Base Weapon Wheel path is unchanged

This build does not yet recreate the true 4+5 DLC wheel in the normal campaign.

Test inside These Sunny Daze:
1. Test DLC Weapon Wheel.
2. Test Base Weapon Wheel.
3. Move to a weapon and release for each wheel.
4. Send PostalBorkenMenu.log.

Expected H50 lines:
[WHEEL H50] BASE-only wheel opened in DLC via validated InputManager.OnWheelDown.
[WHEEL H50] BASE-only DLC wheel closed via validated InputManager.OnWheelUp.

Package:
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt
