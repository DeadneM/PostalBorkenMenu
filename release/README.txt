POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H42 SAFE MENU CLEANUP TEST

BASE
----
H42 starts directly from H40.

H40 STATUS
----------
H40 is the last known-good base before the H41 cleanup regression.
It preserves:
- validated H39 native startup-logo skip
- Skip Startup Logos menu toggle + INI persistence
- Skip Intro Videos
- No Crosshair
- Time Scale
- Give All Weapons
- DLC weapon support
- Arsenal / Special tabs
- Base / DLC weapon wheels
- H13 gameplay/stability baseline

H41 REGRESSION
--------------
H41 physically removed the Help and Log Time entries from g_cmds.
That changed the indices of every later command record.

The user reported a crash with H41.

H42 SAFE FIX
------------
H42 does NOT remove those records from g_cmds.

Instead:
- Help remains in the internal command table
- Log Time remains in the internal command table
- their original positions and indices are preserved
- both are simply hidden from the overlay menu

This keeps all downstream command indices exactly aligned with H40 while still
cleaning the visible menu as requested.

No other option is hidden or changed.

PACKAGE
-------
The ZIP contains exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
