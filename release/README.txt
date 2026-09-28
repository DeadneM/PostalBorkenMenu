PostalBorkenMenu V2A42-H17 STARTUP LOGO AUDIT TEST
POSTAL: Brain-Damaged

STATUS
Diagnostic candidate only.
Built directly from validated/canonical V2A42-H13.
H13 remains the canonical gameplay/source base.

CORRECTION OF TARGET
The game's DebugManager.NO_VIDEO / SkipIntroVideos path refers to an intro cinematic,
not the developer/publisher logos shown during application startup.
Therefore H14/H15/H16 targeted the wrong mechanism and are rejected as wrong-target experiments.

H17 GOAL
Identify the actual classes, methods, and fields responsible for startup logos / splash screens
without modifying any runtime state.

H17 AUDIT
Assembly-CSharp metadata is scanned for these terms in class names, namespaces, method names, and field names:
logo
splash
boot
startup
intro
video
movie
cinematic
cutscene
publisher
developer
sequence

For classes whose name/namespace matches, H17 logs all methods and fields.
For otherwise unrelated classes, H17 logs only members whose names match the startup terms.
Method parameter counts and return types are logged.
Field types are logged when the IL2CPP export is available.

SAFETY / PRESERVATION
- Observation only.
- No startup object is instantiated.
- No field is written.
- No runtime method is invoked by the H17 audit.
- No Escape, SendInput, keybd_event, mouse input, or PostMessage input simulation.
- No PEB or Windows command-line modification.
- No DebugManager.NO_VIDEO manipulation.
- Base Weapon Wheel remains the validated H13/H4 path.
- No Crosshair remains the validated H13 multi-controller path.
- Grappling Hook remains the validated H3 AddWeapon -> InitHook sequence.
- V2A8 shutdown/lifetime fencing is unchanged.
- Minimal DXGI ASI loader is unchanged.

TEST
1. Replace dxgi.dll and PostalBorkenMenu.asi with the H17 package.
2. Delete or rename the old PostalBorkenMenu.log.
3. Launch the game normally and let it reach the main menu.
4. Exit normally.
5. Send the fresh PostalBorkenMenu.log.

IMPORTANT LOG MARKERS
[H17 STARTUP] ===== startup/logo metadata audit begin =====
[H17 STARTUP] CLASS
[H17 STARTUP] METHOD
[H17 STARTUP] FIELD
[H17 STARTUP] ===== startup/logo metadata audit end =====

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
