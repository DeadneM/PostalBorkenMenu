PostalBorkenMenu V2A40 STABLE WINDOW SUBCLASS TEST
POSTAL: Brain-Damaged

BASE
Built directly from V2A39.

WHY THIS TEST EXISTS
V2A39 reaches full READY and then crashes without any command execution being logged.
The exact-name resolver is linked and the named dispatcher branch is present, but no Arsenal/Special rows exist.

This points away from named-weapon execution and toward a startup timing race around the game-window subclass.

V2A40 CHANGE
HookGameWindow() no longer subclasses the first foreground window belonging to the game process immediately.

It now requires:
- foreground window belongs to the current process
- HWND is valid
- client area is at least 320x200
- the same foreground HWND remains stable for 30 consecutive 100 ms samples

That is 3 seconds of stability before SetWindowLongPtrW installs GameWndProc.

If the candidate changes or becomes invalid, the stability counter resets.

LOG MARKERS
[WINDOW] Waiting for one stable foreground game window before subclassing.
[WINDOW] New foreground candidate detected; stability timer restarted.
[WINDOW] Candidate stable for 1 second.
[WINDOW] Candidate stable for 2 seconds.
[WINDOW] Candidate stable for 3 seconds; installing WndProc subclass now.
[OK] Stable game window subclass installed - F1 overlay ready

UNCHANGED FROM V2A39
- exact-name resolver code
- named dispatcher branch
- original V2A34 command table
- zero Arsenal/Special rows
- original four-tab UI
- original INI
- no automatic WeaponId scan
- no automatic AddWeapon/EquipWeapon
- no recovery code
- V2A8 shutdown fencing

TEST
1. Launch normally.
2. Do not press any mod hotkey during startup.
3. Check whether the game passes the intro/menu transition.
4. If stable, open F1 once.
5. Exit normally.
6. Send PostalBorkenMenu.log.

INTERPRETATION
- Stable: window subclass timing race confirmed.
- Crash after READY: continue isolating post-subclass activity.
- Crash before subclass: the failure is earlier than GameWndProc installation.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is not included.
