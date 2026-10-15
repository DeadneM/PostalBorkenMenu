# H100 TEST - Visual startup and keyboard names

Base: H99D experimental interface on user-validated H97 gameplay. Main/stable H97 remains separate.
Goal: readable keyboard labels and persistent checkboxes for every Visual entry.
Success: OEM key labels match the active game keyboard layout; selected native Visual
states restore after a restart without opening the mod menu; wheel/cursor/updater behavior retained.

- Windows keyboard layout names/symbols replace VK hex labels where Windows recognizes a key.
  Key codes and existing bindings remain unchanged. Unknown codes keep the fallback.
- No HUD, No HUD With Crosshair, No Model and No Menu now use checkboxes.
  Native DebugManager getters read the real state before invoking a command and verify
  it afterwards. Successful changes, including hotkeys, are saved in [Visual].
- These four defaults are 0. NoCrosshair=1 remains under [Settings]. Existing INIs
  are retained; missing keys use compiled defaults. No Menu hides native game menus.
- If any native Visual startup setting is enabled, initialization is requested once,
  after DX11 renderer readiness and a 10-second delay, then applied on the game window
  thread. This explicitly selected path is experimental; the default all-off path
  keeps H77 initialization on user demand. Failed reads never trigger blind toggles.
- Native F3 investigation: local Assembly-CSharp method PlayerWeaponCamera.Update
  (RVA 0x80e440) calls input with Unity KeyCode.F3 (284), then flips a byte at +0x44
  of a component obtained through the camera reference. This establishes a native
  camera-rendering toggle, not the exact light/shadow algorithm. No F3 hook or patch
  was added. F3 is not assigned in the supplied current mod INI.

Status before CI: EXPERIMENTAL, not validated in game. No game launched.
Test: inspect OEM keys in all tabs; check one Visual option, restart normally and wait
10 seconds after renderer readiness; verify actual effect and saved checkbox, then
uncheck/restart. Test each separately, including No Menu and recovery via mod key.
Check H97 DLC wheel at cold start and normal wheel, mouse capture, updater.
Rollback: reinstall H99D files, retaining a backup of the personal INI; [Visual] is
ignored by H99D. H97 remains the stable gameplay fallback.

TODO: user runtime validation; identify exact native F3 component type before exposing
or changing it. No assertion that build/static checks establish visual behavior.

### H100 delivery verification
CONSTRUIT / CONTROLE, EXPERIMENTAL; no live-game validation.
Source/tag 615ada81332e4aaba636269e4ba9bd840af4a54e.
CI 38726888611: all steps successful. Downloaded published ZIP, verified exact four
root files and AMD64 DLL/ASI. SHA256:
25691b687747e0379afefd44ece08b650eaf64f78359856a76df53a7c6f6e72a.
ZIP: dxgi.dll 9216; ASI 993280; INI 2489; README 11241 bytes.
Main verified unchanged at c1e4630767db8be3b6eab6d6c351e3a7fe23eff3.
Runtime TODO remains as above; no game files changed or game launched.
