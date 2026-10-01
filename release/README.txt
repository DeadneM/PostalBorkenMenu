POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H37 EARLY PLAYERSETTINGS RAM SPLASH TEST

BASE
----
H37 starts directly from validated V2A42-H13.
H29, H30, H32-H36 remain diagnostic/rejected branches and are not used as the gameplay base.

WHAT WE NOW KNOW
----------------
The exact retail globalgamemanagers PlayerSettings contains:

m_ShowUnitySplashScreen = 1
m_ShowUnitySplashLogo   = 0
m_SplashScreenLogos     = 3

Logo #1:
  Sprite: LOGO_RWS
  PathID: 5944
  Duration: 2.0 seconds

Logo #2:
  Sprite: LOGO_Hyperstrange
  PathID: 5943
  Duration: 3.0 seconds

Logo #3:
  Sprite: LOGOS_Rest
  PathID: 5942
  Duration: 3.0 seconds

Those are the three visible startup logos.

WHAT H36 PROVED
---------------
H36 successfully hooked UnityPlayer ReadFile and later observed 626 reads from a file
with the exact retail globalgamemanagers size, but never observed the original
PlayerSettings offset containing m_ShowUnitySplashScreen.

Conclusion:
the critical PlayerSettings data is read before the normal ASI worker hook is active.

H37 METHOD
----------
H37 does not hook the file system.

The DXGI proxy loads PostalBorkenMenu.asi and, immediately after LoadLibraryW returns,
calls the exported PostalEarlySplashPatch() synchronously BEFORE forwarding the real
CreateDXGIFactory call.

The ASI then scans only committed readable MEM_PRIVATE / MEM_MAPPED regions for the
exact 120-byte retail PlayerSettings sequence beginning at m_ShowUnitySplashScreen.

That exact sequence includes the complete three-logo vector above, making the
signature highly specific.

Only an exact match is eligible. For each exact match:

  m_ShowUnitySplashScreen: 1 -> 0

Only the RAM copy changes.
No disk file changes.
The scan stops after at most 8 exact matches.

H37 logs:
[SPLASH H37] committed readable regions scanned before DXGI: N
[SPLASH H37] exact 120-byte PlayerSettings matches: N
[SPLASH H37] m_ShowUnitySplashScreen RAM patches: N

The DXGI loader also logs one of:
[DXGI LOADER] H37: exact PlayerSettings splash block patched in RAM before DXGI factory.
[DXGI LOADER] H37: exact PlayerSettings splash block not found before DXGI factory.

SAFETY / NON-GOALS
------------------
H37 does NOT:
- alter POSTAL Brain Damaged.exe
- alter UnityPlayer.dll on disk
- alter GameAssembly.dll
- alter globalgamemanagers on disk
- alter level0 / level1
- redirect scenes
- call SceneManager.LoadScene
- simulate input
- continuously scan memory
- repeat the H30 byte-by-byte runtime scan

The H37 scan is one-shot, synchronous, and runs before the first forwarded DXGI factory call.

CONFIG
------
[Settings]
SkipStartupLogos=1

Set it to 0 to disable H37's early RAM patch completely.
