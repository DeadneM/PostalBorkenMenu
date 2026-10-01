POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H30 ASI-ONLY MEMORY SCENE-MAP TEST

BASE
----
H30 starts from the validated V2A42-H13 source lineage.

REJECTED H29 RESULT
-------------------
H29 redirected Unity's level0 file directly to level1. In-game testing reached
a black screen after the startup logo sequence. H29 is rejected.

H30 APPROACH
------------
H30 remains ASI-only and modifies no game files on disk.

When SkipStartupLogos=1:
1. PostalBorkenMenu installs a temporary CreateFileW hook in UnityPlayer.dll.
2. If Unity asks for level0, the ASI first searches process memory for the
   exact 72-byte serialized adjacent build-scene entries:
     Assets/Scenes/PlatformIntro.unity
     Assets/Scenes/Intro.unity
3. Only if that exact block is found, it is swapped in memory.
4. Only after that successful in-memory swap is level0 redirected to level1.
5. The CreateFileW hook is removed immediately after a successful redirect.

FAIL-CLOSED
-----------
If the exact scene table is not found, H30 does NOT redirect level0.
Vanilla startup is preserved instead of risking the H29 black screen.

NO DISK PATCHING
----------------
H30 does NOT modify or replace:
- POSTAL Brain Damaged.exe
- GameAssembly.dll
- UnityPlayer.dll
- globalgamemanagers
- globalgamemanagers.assets
- level0
- level1

No SceneManager.LoadScene, WH_CALLWNDPROC startup transition, or synthetic
keyboard/mouse input is used.

CONFIG
------
[Settings]
SkipStartupLogos=1

Set to 0 to disable this experiment.

EXPECTED LOG
------------
Successful bypass:
[STARTUP H30] Fail-closed UnityPlayer CreateFileW hook installed.
[STARTUP H30] SUCCESS: scene map swapped in memory and level0 redirected to level1.

Safe fallback:
[STARTUP H30] No safe redirect committed; vanilla startup path retained.

INSTALL
-------
Restore original Steam game files first if any previous static test modified
globalgamemanagers, level0 or level1.

Then copy only these four files to the game directory:
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt
