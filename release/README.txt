PostalBorkenMenu V2A48 - GORI-STYLE DX11 IN-SWAPCHAIN OVERLAY TEST
POSTAL: Brain-Damaged

BASE
V2A42 is the last user-confirmed stable PostalBorkenMenu base.
V2A48 keeps its command/weapon/IL2CPP logic and replaces the old persistent
Postal overlay window with a Gori-style in-game renderer.

WHY THIS BUILD
V2A43 / V2A44 / V2A46 / V2A47A / V2A47B were rejected during mouse/window
experiments. V2A47C was stable, but ShowCursor from its worker-thread cave did
not produce a visible cursor.

POSTAL RENDERER
Player.log confirms:
Direct3D 11.0, feature level 11.1.

GORI-STYLE ARCHITECTURE
Gori rendered its menu inside the game's swapchain instead of maintaining a
second interactive desktop window.

V2A48 applies the same separation to POSTAL:
- no persistent PostalBorkenMenu HWND is created
- the real game HWND keeps the validated V2A40 WndProc subclass
- a temporary hidden D3D11 probe obtains the DXGI swapchain Present vtable
- the Present vtable is patched, then the temporary probe windows are destroyed
- the menu is rendered into the game's D3D11 backbuffer immediately before Present
- the old V2A42 visual layout is painted to an in-memory GDI bitmap and uploaded
  to a D3D11 texture
- F1 visibility is marshalled to the real game-window thread
- mouse clicks / wheel / raw movement are consumed by the real game WndProc while
  the overlay is open
- cursor visibility is requested on the real game-window thread, not the worker

OPTIONS TRANSFERRED
The V2A42 five-tab command model is preserved:
- Gameplay
- Weapons
- Arsenal
- Special
- Visual

Preserved features include native developer commands, Give All, Give All Weapons,
No Crosshair, TimeScale, exact-name Arsenal, Special weapons, weapon-key mode and
the validated V2A29 wheel paths.

KNOWN INTRO ITEM
The old V2A42 Escape-based "Skip Intro Videos" experiment is known not to skip
the game's startup videos. V2A48 keeps its dormant internal record only for
history/compatibility, but hides the control from the new menu.

NO AUTO-LEVEL WORK YET
V2A48 intentionally does not reintroduce the rejected V2A43 SceneManager /
automatic level-start actions. First validate the new overlay architecture.

TEST
1. Launch normally and reach the main menu.
2. Enter gameplay.
3. Press F1.
4. Confirm the menu is drawn inside the game image.
5. Confirm a usable cursor is visible.
6. Click tabs and RUN buttons.
7. Test mouse-wheel scrolling.
8. Press F1 again and confirm normal gameplay input returns.
9. Exit normally.
10. If anything fails, send PostalBorkenMenu.log and Player.log.

PACKAGE
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally NOT included.
