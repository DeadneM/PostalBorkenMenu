# H98 - integrated ImGui overlay, experimental from H97 stable

User requested the same integrated ImGui menu as Q Protocol/Darksiders, then
explicitly chose integrated rendering rather than repainting the old GDI window.
Reference read-only: Darksiders work-debug OverlayUi.cpp and DarksidersGenesisMod.cpp.
StyleColorsDark, rounding 7/5/4 pixels, padding 12/12, frame padding 8/5, tabs and
keyboard/action tables replicated. No edits to either reference project.

Base stable H97 (validated cold DLC art + H96 underlay fix), main c1e4630.
H98 isolates rendering/input/build infrastructure. IL2CPP/gameplay part2 unchanged.
Commands still posted to the existing game-window bridge, never run on Present.
INI, shortcut defaults, native tap/hold, sprite retention/load and wheel mapping
preserved in source. GDI fallback if DX11 hook/init unavailable.

ImGui 1.91.9b f5befd2, MinHook 1.3.4 c3fcafd, vendored with licenses.
DX11 Present/Resize hooks from a hidden dummy swapchain; filter by game HWND.
Five tabs, resizable/movable native-rendered window, argument combos, toggles,
current binding / Change / Default columns. Existing Escape/Backspace capture.
Existing F1 lazy IL2CPP trigger preserved. Render backend guarded by recursive
critical section across Present/Resize/input. No new WndProc subclass; reuse H97.
Win32 keyboard/mouse/raw-input messages withheld from game while menu visible;
game may also poll input independently, so runtime mouse/camera behavior unproven.
Early H97 shutdown fence preserved. Rendering cleaned on post-fence Present;
trampolines stay loaded until process exit (no runtime DLL unloading supported).

Build changes: static MSVC CRT + default CRT DLL entry initialization for ImGui;
old native core recompiles with own types, custom _fltused disabled for this build.
Thread notifications retained for static CRT. Loader unchanged. No external
runtime DLL added. Header/ABI bridge uses POD and integer action messages.

Known risks: DX11 hook compatibility, Unity cursor warping, input capture,
alt-tab/fullscreen/resize/device loss, threading, shutdown. Build-only success
cannot prove them. No game launched/tested by assistant; no visual proof claimed.

Acceptance: fresh launch with H98, F1 shows actual dark/blue ImGui in game,
move/resize/scroll all five tabs; Run / argument / toggles / Change / Escape /
Backspace / Default persist correctly; F1 close restores game control. Check
normal+DLC wheels, direct cold-normal DLC graphics, crosshair/menu/startup options.
Alt-tab, window/fullscreen changes and clean exit must remain stable. Send log
and screenshot/verdict. Regression -> exit, reinstall H97 stable, preserve INI.

Status: EXPERIMENTAL until user validation. Four-file package and PE integrity
are independent of runtime result. Stable/main remains H97; future UI fixes here.

Upstream backend reference:
https://github.com/ocornut/imgui/blob/v1.91.9b/examples/example_win32_directx11/main.cpp
