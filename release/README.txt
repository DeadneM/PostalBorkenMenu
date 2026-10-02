POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H49 FIELD-AWARE INPUTMANAGER WHEEL TEST

BASE
----
H49 starts directly from validated stable V2A42-H43.

WHAT H48 PROVED
---------------
H48 found:
- live native wheel = 4 base + 5 DLC/PTSD buttons
- live Hyperstrange.PBD.InputManager object exists
- InputManager.OnWheelDown field exists and is non-null
- Player.IsWheelEnabled is 0 before the attempted event
- the object read from OnWheelDown was Rewired.KeyboardMap
- therefore our simple instance field read was wrong
- no delegate Invoke() was called
- Player.IsWheelEnabled remained 0

H49 FIX
-------
H49 no longer assumes InputManager.OnWheelDown / OnWheelUp are instance fields.

For the exact fields:
- OnWheelDown
- OnWheelUp
- _wheelId
- _wheelPreviousItemId
- _wheelNextItemId

H49 dynamically resolves:
- il2cpp_field_get_flags
- il2cpp_field_get_offset
- il2cpp_field_get_type
- il2cpp_field_static_get_value

For each field it logs:
- flags
- whether the field is static
- offset
- declared IL2CPP type

For OnWheelDown / OnWheelUp:
- if static, H49 reads the backing value through il2cpp_field_static_get_value
- if instance, H49 uses the live InputManager object
- H49 requires the declared field type to expose Invoke() with 0 parameters
- H49 also requires the runtime object type to expose Invoke() with 0 parameters
- only then is Invoke() executed

No broad metadata scan is used.

TEST
----
Inside the actual These Sunny Daze DLC level:

1. HOLD the mod's DLC Weapon Wheel shortcut.
2. Move through the wheel.
3. Release.
4. Send PostalBorkenMenu.log.

Most important lines:
[WHEEL H49 FIELD]
OnWheelDown
[WHEEL H49] field static: ...
[WHEEL H49] declared field type:
...
[WHEEL H49] declared zero-arg Invoke present: ...
[WHEEL H49] wheel event backing field read via STATIC/INSTANCE storage.
[WHEEL H49] runtime event object type:
...
[WHEEL H49] runtime zero-arg Invoke present: ...
[WHEEL H49] event Invoke() completed.
[WHEEL H49] Player.IsWheelEnabled after event: ...

If the real backing field is a delegate, this should finally fire the exact InputManager multicast path instead of reading unrelated Rewired state.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
