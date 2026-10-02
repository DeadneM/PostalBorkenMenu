POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H47 TARGETED WHEEL CALLER AUDIT

BASE
----
H47 starts directly from validated stable V2A42-H43.

WHY H47
-------
H46 confirmed:
- the live DLC level contains 4 base + 5 DLC/PTSD buttons;
- PlayerWheelView.OnWheelDown() executes without exception;
- PlayerWheelView.OnWheelUp() executes without exception;
- the wheel still does not become visible.

Therefore OnWheelDown/OnWheelUp are not the top-level wheel opener.
They are downstream callbacks in an already-armed wheel path.

H45 also exposed useful Player members before its broad metadata audit crashed:
- get_IsWheelEnabled
- OnWheelWeaponSelected
- _playerWheelView

H47 is a safe targeted metadata build.

WHAT H47 DOES
-------------
When the mod's DLC Weapon Wheel hotkey is pressed:

1. It does NOT show any wheel.
2. It does NOT mutate any button.
3. It does NOT call OnWheelDown / OnWheelUp.
4. It logs complete method and field names for exactly:
   - Hyperstrange.PBD.Player
   - Hyperstrange.PBD.PlayerView.PlayerWheelView
   - Hyperstrange.PBD.PlayerComponents.PlayerWeaponWheelComponent
5. It logs the live Player state:
   - Player object found
   - Player.IsWheelEnabled
   - _playerWheelView non-null
   - OnWheelWeaponSelected non-null
6. It scans only Hyperstrange.* classes for member names containing:
   - Wheel
   - WeaponSelected

IMPORTANT SAFETY DIFFERENCE FROM H45
------------------------------------
H47 does NOT resolve parameter types or return types.
It does NOT scan Rewired classes.
It does NOT invoke any discovered method.

This avoids the exact metadata path that crashed H45.

TEST
----
Inside the real DLC level:

1. Press the mod's DLC Weapon Wheel shortcut once.
2. Nothing visual is expected.
3. Close the game normally.
4. Send PostalBorkenMenu.log.

The log should reveal the actual top-level class/method that owns wheel input and selection.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
