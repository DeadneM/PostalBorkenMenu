POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H35 UNITY SPLASH STOP TEST

BASE
----
H35 starts directly from validated V2A42-H13.

WHY THIS APPROACH
-----------------
H29 proved that redirecting level0 does not remove the three startup logos.
H33/H34 proved RCLogoComponent is not the active startup-logo controller.
H34 also found no dedicated game-side startup-logo sequencer.

The three logos are therefore targeted as Unity's native Splash Screen.

H35 METHOD
----------
H35 resolves only:
  UnityEngine.Rendering.SplashScreen
  SplashScreen.Stop(StopBehavior)
  SplashScreen.get_isFinished()

If the exact static Stop method is present, H35 installs a one-shot hook on
the Unity window thread and invokes:
  SplashScreen.Stop(StopImmediate)

StopImmediate is enum value 0.

The hook is removed immediately after the call.

NO GAME-FILE PATCHING
---------------------
H35 does NOT:
- modify POSTAL Brain Damaged.exe
- modify GameAssembly.dll
- modify UnityPlayer.dll
- modify globalgamemanagers
- modify level0 / level1
- redirect scenes
- call SceneManager.LoadScene
- scan process memory
- simulate keyboard or mouse input

CONFIG
------
[Settings]
SkipStartupLogos=1

EXPECTED LOG
------------
[SPLASH H35] UnityEngine.Rendering.SplashScreen class: 1
[SPLASH H35] Stop(static,1 param) signature OK: 1
[SPLASH H35] One-shot Unity window-thread hook installed.
[SPLASH H35] SUCCESS: Unity SplashScreen.Stop(StopImmediate) invoked on Unity window thread.
[SPLASH H35] StopImmediate call succeeded: 1

INSTALL
-------
Restore original game files, then copy only:
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

All validated H13 gameplay and stability functionality remains present.
