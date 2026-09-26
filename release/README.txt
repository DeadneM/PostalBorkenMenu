PostalBorkenMenu V2A43
POSTAL: Brain-Damaged

BASE
Built directly from V2A42, validated working by the user.

V2A42 PRESERVED
- V2A40 stable 3-second game-window qualification before WndProc subclass
- five tabs: Gameplay / Weapons / Arsenal / Special / Visual
- 14 single Arsenal weapons, no Akimbo rows
- 4 Special rows, no Dong rows
- Skip Intro Videos checkbox, enabled by default
- V2A29 wheel behavior
- V2A8 early shutdown fencing
- no broad Assembly-CSharp scans
- no startup Hook recovery
- no EXE/GameAssembly patching

V2A43: OVERLAY MOUSE + AUTO PAUSE
The user previously needed to open the game's Pause menu at the same time as F1 because the gameplay cursor stayed hidden/locked.

V2A43 changes F1 behavior:
1. Open PostalBorkenMenu.
2. Save the current game timescale.
3. Set game timescale to 0.0.
4. Save Unity Cursor.lockState and Cursor.visible.
5. Set Cursor.lockState = None.
6. Set Cursor.visible = true.
7. While the overlay is open, refresh only the cursor unlock/visibility state about four times per second.
8. Suppress gameplay raw/mouse traffic behind the overlay.
9. When F1 closes, restore the exact saved cursor lock/visibility state and previous game timescale.

This is temporary overlay state only. It is not written to the INI.

V2A43: AUTO ON LEVEL START
New persistent checkboxes:

Weapons tab:
- Auto Give All on Level Start
- Auto Give All Weapons on Level Start

Visual tab:
- Auto No HUD on Level Start
- Auto No HUD + Crosshair on Level Start

All four default to OFF.

The two automatic HUD modes are mutually exclusive.

No Crosshair:
- the existing No Crosshair setting remains unchanged
- if No Crosshair is already enabled, V2A43 re-applies alpha=0 after a playable scene change

LEVEL DETECTION
V2A43 resolves UnityEngine.SceneManagement.SceneManager.GetActiveScene.
The worker samples the active scene handle once per second only when at least one auto-level behavior is enabled.

When the scene handle changes:
- wait 2 seconds
- look only for PlayerInventoryComponent using the already targeted FindObjectOfType bridge
- retry once per second for up to 15 seconds
- when PlayerInventoryComponent exists, treat the scene as playable and run the enabled actions once
- no global class scan
- no per-frame IL2CPP scan

If an auto checkbox is enabled in the middle of a level, the current scene is deliberately re-armed and can receive the selected action after the same readiness gate.

AUTO ACTION ORDER
1. Give All, if enabled
2. Give All Weapons, if enabled
3. one HUD mode, if enabled
4. No Crosshair re-application, if the persistent NoCrosshair setting is enabled

GIVE ALL WEAPONS
Uses the existing validated PlayerInventoryComponent.GiveAllWeapons path.
Quest items remain untouched by that specific function.

SKIP INTRO
Unchanged from V2A42:
- default ON
- one startup-only Escape pulse
- no SAVE_DATA.cfg modification
- no renamed/deleted video files
- no repeated key injection

TEST
1. Launch the game normally.
2. Confirm Skip Intro still behaves as V2A42.
3. Press F1 during gameplay without opening the game's Pause menu.
4. Confirm the game pauses and a visible free mouse cursor appears.
5. Click several overlay tabs/options.
6. Close F1 and confirm gameplay, previous timescale and mouse lock resume correctly.
7. Enable one Auto Give option.
8. Load/restart a level and confirm it fires once after the level becomes playable.
9. Test one Auto HUD option.
10. Exit normally and send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is intentionally not included.
