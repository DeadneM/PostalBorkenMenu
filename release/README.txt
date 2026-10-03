POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H51 POST-EVENT BASE FILTER TEST

BASE
----
H51 starts from H49, not from rejected H50.

H50 STATUS
----------
Rejected.

User result:
- both mod wheel hotkeys opened the full DLC wheel
- noticeable lag

Cause:
- Base Weapon Wheel filtered DLC buttons BEFORE firing the real InputManager.OnWheelDown event
- the native wheel setup then rebuilt/re-enabled the DLC layout and erased the filter
- the H49 diagnostic metadata/log path was still being executed on every press/release

H51 FIX
-------
DLC Weapon Wheel:
- uses the validated H49 static System.Action path
- OnWheelDown / OnWheelUp are read through static IL2CPP storage
- hot path is now fast and quiet
- no repeated metadata audit/log spam

Base Weapon Wheel in the DLC:
1. fire the real InputManager.OnWheelDown first
2. do NOT filter immediately
3. poll Player.IsWheelEnabled
4. when the native wheel is genuinely active, apply the base-only filter
5. disable DLC-category buttons only after the game's setup is complete
6. on release, fire InputManager.OnWheelUp
7. restore all native buttons

Base Weapon Wheel outside the DLC:
- preserves the validated H43 path unchanged

EXPECTED RESULT
---------------
Inside the DLC:

DLC Weapon Wheel:
- full native DLC wheel
- no extra diagnostic stutter

Base Weapon Wheel:
- native wheel opens normally
- after the internal hold threshold, DLC buttons are disabled
- only base-category buttons remain selectable/visible

IMPORTANT LOG LINES
-------------------
[WHEEL H51] Base wheel native input fired; waiting for active wheel before filtering DLC buttons.
[WHEEL H51] Delayed BASE buttons enabled: N
[WHEEL H51] Delayed DLC buttons disabled: N
[WHEEL H51] Base-only filter applied AFTER native wheel became active.
[WHEEL H51] DLC wheel opened through fast validated InputManager.OnWheelDown.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
