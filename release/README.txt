PostalBorkenMenu V2A42-H22 STARTUP CLASS INVENTORY TEST
POSTAL: Brain-Damaged

STATUS
Diagnostic candidate only.
Gameplay/source base: user-validated V2A42-H13.
H21 succeeded and revealed the current control-path candidates; H22 does not inherit H21 runtime changes because H21 had none beyond metadata logging.
H19 remains rejected.
Automatic one-launch-one-log reset is preserved in the DXGI loader.

WHY H22
H21 established:
- RCLogoComponent is Hyperstrange.PBD.Ready and exposes pointer-click/enter/exit behavior plus _cursorSprite. It does not expose startup-sequence playback methods.
- IntroController is Hyperstrange.PBD and controls the actual video path with PlayVideo, OnIntroVideoLoopPointReached, OnPrepareCompleted, skip UI/progress fields, and _videoPlayer/_videoClip.
- SkipIntroController is a preference controller with OnValueChanged/GetInitialValue.
- SceneManager exposes static LoadIntroCluster() and static LoadTitleCluster(), with _introCluster and _titleCluster fields.

H22 stays read-only and asks the next question before invoking anything:
What other startup-related managed classes exist, and what exactly is SceneCluster?

H22 INVENTORY KEYWORDS
- Intro
- Logo
- Ready
- Title
- Cluster
- Splash
- Startup
- Boot

H22 TARGETED CLASS
- SceneCluster

H22 SAFETY
- Assembly-CSharp metadata enumeration only.
- No Unity object enumeration.
- No field writes.
- No runtime_invoke.
- No LoadIntroCluster / LoadTitleCluster call.
- No startup/skip method call.
- No early IL2CPP worker.
- H13 2500 ms startup delay preserved.
- No simulated keyboard or mouse input.
- No PEB / process command-line modification.
- No game binary patching.
- H13 gameplay/menu behavior unchanged.

TEST
1. Install the four H22 files.
2. Launch normally and let the main menu appear.
3. Exit normally.
4. Send PostalBorkenMenu.log.

IMPORTANT MARKERS
[H22 INVENTORY] ===== Assembly-CSharp startup-related class inventory =====
[H22 INVENTORY CLASS]
[H22 META] ===== SceneCluster targeted metadata audit =====
[H22 META METHOD]
[H22 META FIELD]

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H21 DIAGNOSTIC HISTORY
======================================================================

PostalBorkenMenu V2A42-H21 PLATFORMINTRO METADATA AUDIT TEST
POSTAL: Brain-Damaged

STATUS
Diagnostic candidate only.
Gameplay/source base: user-validated V2A42-H13.
H20 is a successful diagnostic predecessor only; H21 does not inherit its static scanner.
H19 remains rejected.
Automatic one-launch-one-log reset is preserved in the DXGI loader.

WHY H21
H20's read-only asset scan separated the startup logo path from the actual opening cinematic:
- globalgamemanagers contains Assets/Scenes/PlatformIntro.unity and Assets/Scenes/Intro.unity
- resources.assets contains RCLogo / RCLogoImage
- sharedassets1.assets contains VIDEO_Intro and Assets/Videos/Cinematics/VIDEO_Intro.webm

This strongly suggests that the developer/publisher logo sequence belongs to PlatformIntro while VIDEO_Intro.webm is the separate opening cinematic.
H21 now audits the managed metadata around that startup path without observing live Unity objects.

H21 TARGET CLASSES
- RCLogoComponent
- IntroController
- SkipIntroController
- SceneManager

H21 LOGS
For every resolved target class H21 records:
- class namespace and class name
- every method name
- method parameter count
- instance/static flag
- return type namespace/name
- each parameter type namespace/name
- every field name

SAFETY / NON-INVASIVE RULES
- Metadata inspection only.
- No FindObjectsOfTypeAll or other object enumeration for H21.
- No field writes.
- No runtime_invoke from H21.
- No startup / skip method call.
- No early Unity or IL2CPP worker.
- Normal H13 2500 ms initialization timing is preserved.
- No simulated keyboard or mouse input.
- No PEB / process command-line modification.
- No game EXE, GameAssembly.dll or UnityPlayer.dll patching.
- H13 gameplay/menu behavior is unchanged.

TEST
1. Install the four H21 files.
2. Launch the game normally.
3. Let it reach the main menu.
4. Open F1 briefly to confirm the normal H13 menu still works.
5. Exit normally.
6. Send PostalBorkenMenu.log.

IMPORTANT MARKERS
[H21 META] ===== PlatformIntro / startup-logo metadata audit =====
[H21 META CLASS QUERY]
[H21 META METHOD]
[H21 META FIELD]
[H21 META] ===== PlatformIntro / startup-logo metadata audit end =====

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H20 DIAGNOSTIC HISTORY
======================================================================

PostalBorkenMenu V2A42-H20 STATIC STARTUP ASSET SCAN TEST
POSTAL: Brain-Damaged

STATUS
Diagnostic candidate only.
Built from current main / validated H13 gameplay lineage.
Includes automatic one-launch-one-log reset.

WHY H20
H19 crashed immediately after resolving the four target classes and before its first FindObjectsOfTypeAll result.
That proves the failure is in the early Unity/IL2CPP runtime observation path, not in the log reset or H13 gameplay code.
H19 is rejected as an unsafe-timing diagnostic.

H20 METHOD
No early Unity or IL2CPP runtime calls.
H20 performs a read-only disk scan of Unity data files before normal H13 initialization.

H20 scans:
- globalgamemanagers
- globalgamemanagers.assets
- resources.assets
- data.unity3d
- level0
- sharedassets*.assets
- files whose names contain splash / logo / intro

Search keywords inside printable ASCII strings:
logo
splash
intro
startup
boot
hyperstrange
running with scissors
runningwithscissors
movie games / moviegames
creativeforge / creative forge
publisher
developer
pbd_ready
readylogo
rc_logo

SAFETY
- Read-only file access.
- No field writes.
- No Unity runtime invocation.
- No IL2CPP early thread attachment.
- No startup/skip method invocation.
- No synthetic input.
- No PEB or command-line modification.
- H13 gameplay paths remain unchanged.

LIMITS
- Up to 128 MiB scanned per candidate file.
- Up to 24 candidate files.
- Up to 120 matching strings logged per file.

TEST
1. Replace dxgi.dll and PostalBorkenMenu.asi with H20.
2. No need to delete PostalBorkenMenu.log.
3. Launch normally.
4. Let the game reach the main menu.
5. Exit and send the fresh PostalBorkenMenu.log.

IMPORTANT MARKERS
[H20 FILESCAN] ===== static startup asset scan begin =====
[H20 FILESCAN] HIT
[H20 FILESCAN] ===== static startup asset scan end =====

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H13 CANONICAL HISTORY
======================================================================

PostalBorkenMenu V2A42-H13 RECOVERY + DLC CROSSHAIR AUDIT
POSTAL: Brain-Damaged

STATUS
VALIDATED by user.
V2A42-H13 is the current canonical source/gameplay base.
Built directly from H4.
H9 / H10 / H11 / H12 wheel experiments are rejected and are NOT inherited.

PRIMARY GOAL
Restore the stable H4 behavior first, then isolate the two remaining issues:
- No Crosshair in DLC.
- Native Skip Intro / NO_VIDEO mechanism.

WEAPON WHEELS
- Base Weapon Wheel is exactly the H4 implementation again.
- No H7-H12 controller Show/Hide/UpdateSelection/SelectButton experiments are present.
- DLC Weapon Wheel is back to the H4 research implementation only.
- This build is NOT a new DLC-wheel experiment.

NO CROSSHAIR H13
H4 targeted only the first PlayerCrosshairController returned by FindObjectOfType.
H13 additionally resolves Resources.FindObjectsOfTypeAll(PlayerCrosshairController).

When No Crosshair is applied:
- every loaded PlayerCrosshairController is inspected,
- only each controller's current Crosshair object is used,
- only CanvasRenderer alpha is changed,
- aim logic / target position / current crosshair object are never cleared or replaced,
- the validated H4 single-controller path remains as fallback.

The log records:
- PlayerCrosshairController object count,
- how many expose a current Crosshair,
- how many CanvasRenderers were actually updated.

SKIP INTRO H13
- NO simulated keyboard or mouse input exists.
- The old Escape implementation is removed.
- H13 does NOT claim Skip Intro is fixed yet.
- The existing menu preference is still stored in the INI.
- H13 performs a targeted metadata audit of:
  - Hyperstrange.PBD.DebugManager
  - NO_VIDEO class if present
  - PlayerCrosshairController
This is intended to reveal the real internal NO_VIDEO path used by the game's working launch option.

TEST
1. Install all four files.
2. Enter normal gameplay.
3. Test Base Weapon Wheel first and confirm it behaves like H4.
4. Enable No Crosshair and confirm the base-game reticle disappears.
5. Enter the DLC / use DLC weapons and confirm whether the reticle remains hidden.
6. Switch several weapons in the DLC.
7. Exit normally.
8. Send PostalBorkenMenu.log so the NO_VIDEO and DLC crosshair metadata can be read.

VALIDATION RESULT
- Base Weapon Wheel restored and validated.
- No Crosshair behavior validated, including the DLC path tested by the user.
- Grappling Hook validated behavior preserved.
- H13 promoted to canonical base.

KNOWN OPEN ITEM
Skip Intro is NOT fixed in H13.
The old Escape/input approach is permanently rejected.
Future work must reproduce the game's native NO_VIDEO behavior without simulated user input.

PACKAGE
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
H4 BASE TECHNICAL HISTORY
======================================================================

PostalBorkenMenu V2A42-H4 CLEAN GRAPPLING MENU + DXGI LOADER
POSTAL: Brain-Damaged

BASE
Built from the user-validated V2A42-H3 grappling-hook fix.
H3 is the validated gameplay basis for this cleanup build.

VALIDATED GRAPPLING FIX PRESERVED
Special -> Grappling Hook uses the validated sequence:
1. Resolve exact WEAPON_Hook.
2. AddWeapon(WEAPON_Hook) only if required.
3. Never use ordinary EquipWeapon for the Hook.
4. Always call WeaponsInventory.InitHook() AFTER the AddWeapon/collected state.
5. Verify get_Hook().

MENU CLEANUP ONLY
- "Hook (Add Only)" renamed to "Grappling Hook".
- Weapon Catalog menu entry removed.
- DLC Hook Slot99 Test / InitHook menu entry removed.
- Their command dispatch/menu routing entries are removed.
- No other menu layout or behavior is changed.

DXGI ASI LOADER
dxgi.dll remains the standard ASI loader.
It forwards the real System32 DXGI exports and loads PostalBorkenMenu.asi.
It does not hook Present, render anything, patch swapchains/vtables, or alter gameplay.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
ORIGINAL V2A42 README
======================================================================

PostalBorkenMenu V2A42
POSTAL: Brain-Damaged

BASE
Built from V2A41 while preserving the V2A40 stable-window subclass fix that the user validated.

THIS BUILD
V2A42 reconstructs the desired five-tab overlay and cleans the named weapon lists.

TABS
- Gameplay
- Weapons
- Arsenal
- Special
- Visual

ARSENAL
Only single WeaponId variants are exposed.

Base:
- Shovel
- Pistol
- Shotgun
- Machine Gun
- Rocket Launcher
- Lightning Gun
- Gatling Gun
- Dildo Bow
- Cat Canon

These Sunny Daze / PTSD:
- Um Drill
- Piss Gun
- Meat Shotgun
- Bubble Gum Machine Gun
- Nuclear Syringe

All _Akimbo WeaponIds are intentionally removed from the overlay.

SPECIAL
Kept:
- Hook (Add Only)
- Cutscene Weapon DLC
- No Weapon
- No Weapon DLC

Removed:
- Dong
- Dong Confusion
- Dong Fire
- Dong Ice

The Hook path remains AddWeapon-only and is never passed through EquipWeapon.

SKIP INTRO VIDEOS
A new checkbox appears in the Gameplay tab.

Default:
SkipIntroVideos=1

The value is persisted in PostalBorkenMenu.ini under [Settings].
Changing the checkbox affects the next launch.

Implementation for this test:
- no game files are renamed or deleted
- SAVE_DATA.cfg is not modified
- no EXE/GameAssembly patching
- while V2A40 qualifies the foreground game HWND, after the same HWND has remained valid for 1 second, the ASI sends exactly one synthetic Escape key pulse if Skip Intro Videos is enabled
- the normal V2A40 WndProc subclass is still delayed until 3 seconds of HWND stability
- no repeated key injection occurs

This deliberately targets startup only and does not touch normal in-game cutscenes.

STABILITY PRESERVED
- V2A40: 3-second stable foreground HWND qualification before SetWindowLongPtrW
- V2A8: early WM_CLOSE shutdown fencing
- no permanent IL2CPP crosshair polling
- V2A29 weapon-wheel behavior
- exact-name WeaponId resolver remains targeted
- no broad Assembly-CSharp scans
- no startup Hook recovery

TEST
1. Launch normally with SkipIntroVideos=1.
2. Confirm whether the intro video is skipped.
3. Confirm the game reaches the menu and remains stable.
4. Open F1.
5. Check all five tabs.
6. Confirm Arsenal contains no Akimbo entries.
7. Confirm Special contains no Dong entries.
8. Toggle Skip Intro Videos off and on once to verify checkbox persistence.
9. Test one normal Arsenal weapon and, if owned, one DLC weapon.
10. Exit normally and send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not included.
