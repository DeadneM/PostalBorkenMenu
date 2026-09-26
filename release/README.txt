PostalBorkenMenu V2A46 INTERACTIVE OVERLAY WINDOW TEST
POSTAL: Brain-Damaged

BASE
Built directly from the exact V2A42 source branch that produced the user-validated stable V2A42 artifact.

SINGLE TEST GOAL
Change only the overlay-window interaction architecture.

OLD V2A42 WINDOW
WS_POPUP
WS_EX_TOOLWINDOW
WS_EX_TOPMOST
WS_EX_LAYERED
WS_EX_TRANSPARENT
WS_EX_NOACTIVATE

V2A46 WINDOW
WS_POPUP
WS_EX_TOOLWINDOW
WS_EX_TOPMOST
WS_EX_LAYERED

REMOVED
- WS_EX_TRANSPARENT
- WS_EX_NOACTIVATE

WHY
V2A42 never truly owns mouse focus.
Mouse clicks are intercepted indirectly through the game's WndProc and Unity keeps ownership of cursor state.

V2A46 BEHAVIOR
When F1 opens:
- overlay is shown with SW_SHOW
- overlay becomes foreground
- keyboard focus is assigned to overlay
- mouse wheel is received directly by OverlayWndProc
- left clicks are received directly by OverlayWndProc
- the window class uses the standard Windows arrow cursor

When F1 closes:
- overlay is hidden
- foreground/focus is returned to the game HWND

Click coordinates are now overlay-client coordinates.
The game WndProc no longer forwards overlay wheel or left-click interactions.

HOTKEY POLLING
Async key polling remains unchanged except that the overlay HWND is also considered active while the menu owns foreground.
This allows F1 to close the menu while the overlay has focus.

UNCHANGED FROM V2A42
- all IL2CPP bridges
- V2A40 stable 3-second game-window qualification
- V2A8 shutdown fencing
- five tabs
- Arsenal and Special lists
- exact-name weapon resolver
- Give All / Give All Weapons
- TimeScale
- No Crosshair
- V2A29 wheel behavior
- INI format and values
- synthetic V2A42 Skip Intro implementation is intentionally left byte-for-source unchanged for this isolation test
- no pause automation
- no Unity Cursor bridge
- no SceneManager bridge
- no auto-level logic

IMPORTANT
This build is only a window-architecture test.
Do not judge Skip Intro from V2A46. It is the same known-nonworking V2A42 Escape experiment.

TEST
1. Launch normally and confirm startup stability.
2. Enter gameplay.
3. Press F1 without opening the game's Pause menu.
4. Check whether a normal visible Windows mouse cursor appears.
5. Click tabs and RUN buttons directly.
6. Use the mouse wheel over the overlay.
7. Press F1 again and confirm focus returns to the game.
8. Exit normally.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not included.
