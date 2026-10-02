POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H43 SAFE MENU CLEANUP TEST

BASE
----
H43 starts directly from H40, the last known-good menu build.

H41 REGRESSION
--------------
H41 physically removed Help and Log Time from the internal g_cmds table.
That shifted all later command indices and the user reported a crash.

H43 SAFE CLEANUP
----------------
H43 does NOT remove any internal command records.

The following rows are hidden from the overlay only:
- Help
- Log Time
- Skip Intro Videos

Their original internal positions remain intact, preserving all command indices.

SKIP INTRO VIDEOS
-----------------
The game already provides its own Skip Intro Videos option.

Therefore H43:
- hides the redundant mod-menu row,
- removes SkipIntroVideos from the bundled PostalBorkenMenu.ini,
- forces the mod-side g_skipIntroVideos state to 0,
- does not attempt to manage or emulate the game's native setting.

SKIP STARTUP LOGOS
------------------
This remains fully active and unchanged from validated H39/H40.

The Gameplay tab still contains:
- Skip Startup Logos

It persists:
[Settings]
SkipStartupLogos=1 or 0

and applies on the next launch.

PRESERVED FEATURES
------------------
- validated H39 native startup-logo skip
- Skip Startup Logos menu toggle
- No Crosshair
- Time Scale
- Give All Weapons
- DLC weapon support
- Arsenal / Special tabs
- Base / DLC weapon wheels
- H13 gameplay/stability baseline

PACKAGE
-------
The ZIP contains exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
