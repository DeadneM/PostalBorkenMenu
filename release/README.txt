PostalBorkenMenu V2A45
POSTAL: Brain-Damaged

BASE
Built directly from V2A42, the last build the user confirmed working.

PURPOSE
Pure cleanup rollback after V2A43 and V2A44 both crashed.

V2A45 REMOVES ONLY THE NON-WORKING SYNTHETIC INTRO FEATURE
- removes the Skip Intro Videos overlay row
- removes the SkipIntroVideos INI setting
- removes keybd_event import
- removes the one-shot Escape pulse
- removes all synthetic intro logs/state

NO MOUSE / PAUSE EXPERIMENTS
V2A43 and V2A44 are rejected.
V2A45 contains none of their cursor, pause or SceneManager experiments.

PRESERVED EXACTLY FROM V2A42
- V2A40 stable 3-second game-window qualification
- five tabs: Gameplay / Weapons / Arsenal / Special / Visual
- Arsenal: 14 single WeaponIds, no Akimbo entries
- Special: Hook / Cutscene Weapon DLC / No Weapon / No Weapon DLC
- no Dong entries
- V2A29 wheel behavior
- V2A8 early shutdown fencing
- exact-name WeaponId resolver
- existing Give All / Give All Weapons behavior
- No Crosshair behavior
- TimeScale behavior
- no broad Assembly-CSharp scans
- no startup Hook recovery

NATIVE SAVE FINDING
The user's SAVE_DATA.cfg contains:
GeneralSettingsData:
- WatchedIntro = true
- SkipIntro = true
- SkipCutscenes = false

This confirms the game has a native saved SkipIntro field.
V2A45 deliberately does not edit SAVE_DATA.cfg.

Important:
the user's SkipIntro is already true, so the startup videos the user still sees may be controlled by a separate startup-video path rather than this saved field.

TEST
1. Launch normally.
2. Confirm startup stability matches V2A42.
3. Open F1.
4. Confirm all five tabs behave as V2A42.
5. Do not expect a Skip Intro row in V2A45.
6. Exit normally and send PostalBorkenMenu.log if anything abnormal occurs.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is runtime-generated and is intentionally not included.
