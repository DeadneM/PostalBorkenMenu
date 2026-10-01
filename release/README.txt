POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H29 ASI-ONLY STARTUP LOGO BYPASS TEST

TEST BASIS
----------
This candidate starts from the validated V2A42-H13 gameplay/stability source lineage.

H29 changes only startup-logo handling:
- No game EXE patching.
- No GameAssembly.dll patching.
- No UnityPlayer.dll file patching.
- No globalgamemanagers / level0 / level1 file replacement.
- No SceneManager.LoadScene call.
- No WH_CALLWNDPROC startup-scene hook.
- No simulated keyboard or mouse input.

ASI-ONLY STARTUP BYPASS
-----------------------
When SkipStartupLogos=1, PostalBorkenMenu temporarily patches UnityPlayer.dll's
in-process CreateFileW import. The first Unity request for the startup scene
file "level0" is redirected to its sibling "level1". The original game files
on disk remain untouched.

After the redirected file opens successfully, the IAT entry is restored
immediately. The hook is therefore one-shot.

For this retail build:
- level0 corresponds to Assets/Scenes/PlatformIntro.unity
- level1 corresponds to Assets/Scenes/Intro.unity

The goal is to bypass only the startup/platform logos while preserving the
separate opening Intro scene/cinematic.

CONFIG
------
[Settings]
SkipStartupLogos=1

Set to 0 to disable the H29 startup redirect.

EXPECTED LOG
------------
Success should include:
[STARTUP H29] UnityPlayer CreateFileW one-shot hook installed.
[STARTUP H29] SUCCESS: Unity level0 open redirected to level1; hook already removed.

If the second line is not present, send PostalBorkenMenu.log. Do not modify
any game data files for this test.

INSTALLATION
------------
Copy these four files into the POSTAL: Brain-Damaged game directory:

dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

Remove any modified globalgamemanagers, level0 or level1 from prior static
tests and restore the original Steam game files before testing H29.

All validated H13 gameplay features remain present.
