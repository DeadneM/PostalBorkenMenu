POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H48 INPUTMANAGER WHEEL EVENT TEST

BASE
----
H48 starts directly from validated stable V2A42-H43.

WHAT H47 PROVED
---------------
The wheel call chain has a higher-level input event layer.

Hyperstrange.PBD.InputManager exposes:
- OnWheelDown
- OnWheelUp
- _wheelPreviousItemId
- _wheelNextItemId
- _wheelId

PlayerWheelView itself exposes:
- OnWheelDown
- OnWheelUp
- OnPlayerWheelViewShow
- OnPlayerWheelViewHide
- _wheelDown

PlayerWeaponWheelComponent exposes:
- OnPlayerWheelViewShow
- OnPlayerWheelViewHide
- OnWeaponDown
- UpdateSelection
- SelectButton
- SelectButtonInSlot

The live Player object also has:
- IsWheelEnabled
- OnWheelWeaponSelected
- _playerWheelView

H46 proved that directly calling PlayerWheelView.OnWheelDown/OnWheelUp is not enough:
the methods execute without exception but the wheel remains invisible.

H48 HYPOTHESIS
--------------
PlayerWheelView is only one subscriber to the real InputManager wheel event.

Calling only PlayerWheelView.OnWheelDown skips the other subscribers that may:
- arm Player.IsWheelEnabled
- start time scaling
- notify PlayerWeaponWheelComponent
- show the view
- enable selection handling

H48 therefore invokes the actual multicast delegate stored in:
- InputManager.OnWheelDown on press
- InputManager.OnWheelUp on release

This is much closer to the game's real input event path.

H48 SAFETY
----------
H48:
- starts from H43
- does not mutate any wheel button
- does not remap WeaponId
- does not call SelectButton
- does not call UpdateSelection
- does not call PlayerWheelView.Show directly
- does not scan global metadata
- only fires InputManager's already-wired multicast delegates

Before firing, H48 verifies the real DLC scene still has native DLC/PTSD buttons.

TEST
----
Inside the actual These Sunny Daze DLC level:

1. HOLD the mod's DLC Weapon Wheel hotkey.
2. Move through the wheel.
3. Release on another weapon.
4. Check:
   - whether the real DLC wheel becomes visible
   - whether selection works
   - whether the selected weapon equips
5. Send PostalBorkenMenu.log.

Important H48 lines:
[WHEEL H48] live native BASE buttons: 4
[WHEEL H48] live native DLC/PTSD buttons: 5
[WHEEL H48] live InputManager object found: 1
[WHEEL H48] OnWheelDown delegate non-null: 1
[WHEEL H48] Player.IsWheelEnabled before event: ...
[WHEEL H48] Delegate Invoke() completed.
[WHEEL H48] Player.IsWheelEnabled after event: ...
[WHEEL H48] InputManager.OnWheelDown multicast event fired on untouched native DLC layout.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
