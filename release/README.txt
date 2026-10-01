POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H36 UNITY SPLASH FLAG VIRTUALIZATION TEST

BASE
----
H36 starts directly from validated V2A42-H13.

WHAT H34/H35 PROVED
-------------------
The three startup logos are not PlatformIntro scene logic and are not controlled
by RCLogoComponent.

Exact retail globalgamemanagers PlayerSettings analysis found:
- m_ShowUnitySplashScreen = 1
- m_ShowUnitySplashLogo = 0
- m_SplashScreenLogos count = 3
- logo #1: LOGO_RWS, duration 2 seconds
- logo #2: LOGO_Hyperstrange, duration 3 seconds
- logo #3: LOGOS_Rest, duration 3 seconds

This exactly matches the three startup logos we want to remove.

H35 also proved the managed UnityEngine.Rendering.SplashScreen class is stripped
from this IL2CPP player, so calling SplashScreen.Stop through IL2CPP is not possible.

H36 METHOD
----------
H36 is ASI-only and modifies no game file.

During early startup it temporarily hooks only UnityPlayer.dll's imported ReadFile.
For a file with the exact validated globalgamemanagers size (4,518,416 bytes), if
the returned read contains the exact PlayerSettings splash location, H36 verifies
the expected retail bytes and changes only the returned RAM buffer:

  absolute file offset 0x1070:
  m_ShowUnitySplashScreen 1 -> 0

The on-disk globalgamemanagers stays byte-for-byte untouched.

After a successful virtualization, the ReadFile IAT is restored immediately.
If the exact expected bytes are not present, H36 leaves the buffer untouched.

NO SCENE OR GAMEPLAY HACKS
--------------------------
H36 does NOT:
- redirect level0 or level1
- call SceneManager.LoadScene
- modify POSTAL Brain Damaged.exe
- modify GameAssembly.dll
- modify UnityPlayer.dll on disk
- modify globalgamemanagers on disk
- scan all process memory
- simulate keyboard or mouse input

CONFIG
------
[Settings]
SkipStartupLogos=1

TEST
----
Launch normally.

Expected successful log:
[SPLASH H36] SUCCESS: virtualized m_ShowUnitySplashScreen 1 -> 0 in Unity read buffer.
[SPLASH H36] Disk globalgamemanagers was not modified.

If Unity does not use ReadFile for this early PlayerSettings read, H36 will fail
closed and log that the target flag was not observed. No game file is altered.
