POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H40 MENU STARTUP LOGO TOGGLE TEST

BASE
----
H40 starts from H39, which is now validated by the tester:
the three native Unity startup logos are successfully skipped.

H39 VALIDATED NATIVE FIX
------------------------
Exact retail UnityPlayer.dll:

SHA-256:
27b88589c217589675c976bd303984286bb4b71516ab68efac1c178401a31e94

Unity:
2021.3.14f1

Native CancelSplashScreen:
RVA 0x003C2230

Native Splash state pointer:
UnityPlayer + 0x01A36340

State field:
+0x08
  1 = Begin
  2 = Fade
  3 = Done

Active byte:
+0x70

H39 watches this exact native state during startup. When Unity enters Begin/Fade,
H39 calls UnityPlayer's real native CancelSplashScreen(), including Unity's own
internal callback dispatch. This method is validated in-game.

H40 CHANGE
----------
H40 keeps the validated H39 native fix unchanged and connects it to the mod menu.

Gameplay tab now contains:

  Skip Intro Videos
  Skip Startup Logos

Skip Startup Logos uses the same checkbox presentation as Skip Intro Videos:

  [X] ENABLED   NEXT LAUNCH
  [ ] DISABLED  NEXT LAUNCH

Clicking the action area changes:

  [Settings]
  SkipStartupLogos=1

or:

  [Settings]
  SkipStartupLogos=0

The setting is flushed immediately to PostalBorkenMenu.ini.

IMPORTANT
---------
The startup-logo setting takes effect on the NEXT GAME LAUNCH.

This is intentional: by the time the F1 menu is available, Unity's startup splash
sequence has already happened.

When enabled on the next launch:
- H39's validated native CancelSplashScreen path is active.

When disabled on the next launch:
- H39 returns immediately and Unity's three native startup logos play normally.

The three confirmed native splash logos are:
- LOGO_RWS          2 seconds
- LOGO_Hyperstrange 3 seconds
- LOGOS_Rest        3 seconds

NO GAME-FILE MODIFICATION
-------------------------
H40 does not modify:
- POSTAL Brain Damaged.exe
- UnityPlayer.dll on disk
- GameAssembly.dll
- globalgamemanagers
- level0 / level1

No scene redirect.
No process-wide memory scan.
No input simulation.

PACKAGE
-------
The test ZIP contains exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
