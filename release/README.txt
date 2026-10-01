POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H32 NATIVE RCLOGO SKIP TEST

BASE
----
H32 starts directly from the validated V2A42-H13 lineage.

PURPOSE
-------
Skip only the small startup/developer/publisher logos.

H32 does NOT replace or redirect scenes.
It does NOT touch level0, level1 or globalgamemanagers.
It does NOT call SceneManager.LoadScene.
It does NOT scan process memory.

METHOD
------
The game exposes:
  Hyperstrange.PBD.Ready.RCLogoComponent
  RCLogoComponent.OnPointerClick(...)

H32 resolves this native IL2CPP component early, installs a temporary
WH_CALLWNDPROC hook on the Unity/game thread, finds only the currently active
RCLogoComponent and invokes its own OnPointerClick handler.

The call is made at most three times, with a minimum delay between calls.
After three successful calls, or after the short startup window expires,
the hook is completely removed.

WM_NULL is used only to give the Unity main thread harmless callback
opportunities. No keyboard or mouse input is simulated.

NO GAME FILE MODIFICATION
-------------------------
H32 does not write or replace:
- POSTAL Brain Damaged.exe
- GameAssembly.dll
- UnityPlayer.dll
- globalgamemanagers
- globalgamemanagers.assets
- level0
- level1

CONFIG
------
[Settings]
SkipStartupLogos=1

Set to 0 to retain vanilla startup logos.

EXPECTED LOG
------------
Metadata ready:
[LOGO H32] RCLogoComponent class resolved: 1
[LOGO H32] OnPointerClick(instance,1 param) signature OK: 1
[LOGO H32] Unity FindObjectOfType(Type,bool) bridge: 1

Successful native advances:
[LOGO H32] Native RCLogoComponent.OnPointerClick calls completed: 1
...
[LOGO H32] Three native logo-skip calls completed; hook removed.

INSTALL
-------
Restore all original game files if any earlier static experiment changed them.

Copy only:
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

All validated H13 gameplay/stability functionality remains present.
