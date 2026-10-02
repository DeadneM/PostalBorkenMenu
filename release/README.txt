POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H38 NATIVE UNITY SPLASH STATE TEST

BASE
----
H38 starts directly from validated V2A42-H13.

EXACT UNITYPLAYER AUDIT
-----------------------
The exact retail UnityPlayer.dll supplied by the tester was analyzed directly:

Size:
  29,068,712 bytes

SHA-256:
  27b88589c217589675c976bd303984286bb4b71516ab68efac1c178401a31e94

Unity build:
  2021.3.14f1

The native Unity scripting binding table contains:

  UnityEngine.Rendering.SplashScreen::get_isFinished
  UnityEngine.Rendering.SplashScreen::CancelSplashScreen
  UnityEngine.Rendering.SplashScreen::BeginSplashScreenFade
  UnityEngine.Rendering.SplashScreen::Begin
  UnityEngine.Rendering.SplashScreen::Draw
  UnityEngine.Rendering.SplashScreen::SetTime

Recovered native function RVAs:

  get_isFinished         0x003C8660
  CancelSplashScreen     0x003C2230
  BeginSplashScreenFade  0x003C15A0
  Begin                  0x003C15E0
  Draw                   0x003C5210

Recovered native Splash state global:

  UnityPlayer + 0x01A36340

The global points to the native Splash state object.

State field:
  +0x08

Observed semantics from native code:
  1 = Begin
  2 = Fading out
  3 = Done

Active byte:
  +0x70

CancelSplashScreen does exactly:
  state = 3
  active = 0

H38 METHOD
----------
H38 modifies no UnityPlayer code.

It uses two tightly bounded mechanisms:

1. DXGI checkpoints
   The proxy calls the native CancelSplashScreen path before and after each
   CreateDXGIFactory/CreateDXGIFactory1/CreateDXGIFactory2 forwarding point,
   but only if the exact audited UnityPlayer build and exact native function
   byte signatures match.

2. Startup-only native state watcher
   The H13 worker performs a bounded startup-only check of one exact known
   pointer:
     UnityPlayer + 0x01A36340

   If state becomes 1 or 2, H38 changes only:
     state  -> 3
     active -> 0

   Then the watcher exits immediately.

This is NOT a process-wide scan.
It does not search memory.
It does not patch executable code.
It does not call IL2CPP or SceneManager.

WHY H38 EXISTS
--------------
H34/H35 proved the three visible logos are the native Unity Splash system.
Static PlayerSettings analysis found exactly three configured splash logos:

  LOGO_RWS          2 seconds
  LOGO_Hyperstrange 3 seconds
  LOGOS_Rest        3 seconds

H36 proved the original serialized PlayerSettings read occurs before the normal
ASI worker hook.
H37 proved the serialized 120-byte PlayerSettings block is no longer present
in RAM when DXGI begins.

H38 therefore targets the final native runtime state machine itself.

NO GAME-FILE MODIFICATION
-------------------------
H38 does NOT modify:
- POSTAL Brain Damaged.exe
- UnityPlayer.dll on disk
- GameAssembly.dll
- globalgamemanagers
- globalgamemanagers.assets
- level0 / level1

H38 does NOT:
- redirect scenes
- call SceneManager.LoadScene
- simulate keyboard or mouse input
- scan process memory
- patch UnityPlayer executable code

CONFIG
------
[Settings]
SkipStartupLogos=1

Set SkipStartupLogos=0 to disable H38 completely.

EXPECTED LOG
------------
[SPLASH H38] UnityPlayer exact-build verification: 1
[SPLASH H38] Native CancelSplashScreen calls: N
[SPLASH H38] Direct native state stops: N
[SPLASH H38] Last observed native splash state: N

If either Native CancelSplashScreen calls or Direct native state stops is > 0,
H38 reached the real native splash state.
