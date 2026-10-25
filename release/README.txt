V2A42-H104 RELEASE

# V2A42-H104 release

Base: H104 TEST c63ccb5, installed and checked by the user. User feedback on
2026-10-25: "ca a lair ok, push github et build release". This accepts the tested
checkpoint for release; no claim of exhaustive exit-path testing.

## Delivered behavior
- Integrated ImGui menu, native DLC wheel sprites/cold loading and scope-only
  sniper handling retained from preceding builds.
- User-validated level game-time timer retained (on by default).
- ASI/INI defaults aligned: beta checks ON, No Crosshair ON, startup-logo skip ON,
  timer ON, weapon-camera post-processing OFF; other native Visual flags OFF.
- All command shortcuts unbound by default; F1 remains menu access. Automatic
  number-key base/DLC remapping OFF. Commands remain rebindable in the menu.
- Early GameManager/Unity quitting hooks fence timer/input/IL2CPP work before
  forwarding normal quit. Native exit is not replaced with forced termination.
- Personal INI retained during updater install; existing settings/bindings are
  not reset by updating. New defaults apply to fresh configs/missing entries.

## Validation
H104 TEST: build, defaults consistency, native F3 hook fixture, timer-format
boundaries, and static/instance quit-order/forwarding tests passed. User reports
installed test appears okay and requests release. Release rebuild reruns checks.
No game launched by agent. Scope implementation/timer/quit behavior unchanged
between test and release: only version identifiers and delivery documentation change.

## Install / rollback
ZIP has four files at root: dxgi.dll, PostalBorkenMenu.asi, PostalBorkenMenu.ini,
README.txt. Back up existing mod files, replace binaries/readme, keep personal INI.
F1 opens menu. For fresh chosen defaults, use the packaged INI after backing up yours.
Rollback: restore previous backed-up ASI/DXGI/INI/README (H103 lacks the new quit fix).
H104-TEST installation backup remains available locally. Stable main is promoted
only after release build/package verification; test branch/tag are kept separately.

Technical crash evidence and repair mechanism: docs/H104_TEST.md.
TODO: retain cumulative history; investigate any new crash with its own logs/dump.


--- Cumulative documentation and licenses ---
H104 TEST - DEFAULTS / EARLY QUIT FENCE

# H104 TEST - Unbound command defaults and early quit fence

Base: H103 scope-only release + user-validated H102 timer + accepted H101 functions.
Scope-only H103 lacks an explicit user visual verdict. User reports quit crash on
2026-10-25 and approves personal INI settings as future defaults, removing shortcuts.
Goal: zero command shortcuts by default, matching ASI/INI defaults; stop mod work
before engine teardown on Quit. F1 remains menu access, not a gameplay command binding.
Success: fresh INI and missing-key fallback agree; timer works, click Quit closes
normally repeatedly without crash. No claim of runtime success from offline tests.

## Defaults
Settings: MenuKey=112 (F1), UpdaterBeta=1, NoCrosshair=1, SkipStartupLogos=1,
LevelTimer=1. Visual: NoHud=0, NoHudWithCrossHair=0, NoModel=0, NoMenu=0,
WeaponPostProcessing=0. All Keys=0 (all command defaultVk=0). TimeScale argument=0.5,
WeaponKeysMode argument=OFF to disable automatic mod remapping of number keys 1-9;
other arguments empty. ASI missing-key fallbacks match.
Existing personal INI and bindings remain unchanged by the updater/install policy;
these defaults affect fresh configs/missing entries/Default key reset. No default
GodMode/Noclip activation. F3 remains available after native F3 hook; no default action.

## Quit evidence
Latest Unity Player.log and Windows Application events show 0xc0000005 on exit
(2026-10-25 18:17:22), ntdll.dll+0x1a12d. Unity stack includes ASI+0x91e5,
USER32.CallWindowProcW and UnityPlayer. Disassembly of the actual H103 package maps
0x91e5 to the return site of the il2cpp_thread_attach function pointer in the
WM_APP_LEVEL_TIMER handler (0x803e); subsequent handler is weapon post-processing
(0x803d). No mod shutdown marker precedes this. This demonstrates thread-attach
while processing a timer message at crash; teardown timing is the repair hypothesis,
not proof that every possible exit crash shares the same cause.
Evidence copied to ignored local out/quit-evidence-20261025 (not committed/uploaded).

## Repair
Metadata-resolved GameManager.ExitGame and UnityEngine.Application.Internal_ApplicationQuit
get forwarding hooks. Before forwarding, call the existing early shutdown fence,
restore original WndProc, hide menu, stop timer/wheel/input tasks and worker. GameManager
static/instance ABI selected via method_is_instance; original called exactly once.
Unity quitting event provides fallback for other quit paths; WM_CLOSE/destruction
fallbacks retained. Hook mask logged (1 game manager, 2 Unity; expected 3).
EnsureIl2CppThread refuses entry once stopping. Present bypasses overlay without backend
cleanup during teardown; Resize also bypasses overlay. Process exit reclaims resources.
No forced process termination, no changes to the game's exit/Steam cleanup behavior.
No game file patches or game launch. Installation unchanged during this development.

## Checks / test / rollback
Tests: defaults consistency, isolated quit hook ordering/forwarding/ABI (both static
and instance), prior F3 hook fixture and timer-format boundaries. Runtime quit still
EXPERIMENTAL until user validates; no game launched by agent.
Test with current personal config first: open menu, play with timer, return title,
click Quit; verify [QUIT H104] marker and normal exit several times. Also test Quit
from startup/title and WM_CLOSE; opening menu only should preserve cursor behavior.
Fresh default test: back up personal INI, temporarily use packaged defaults, confirm
all command bindings Unbound and F1 menu; timer/post preference restore after 10s.
Rollback: H103 files and backed-up personal INI. Existing INI is not silently rewritten.
TODO: user quit verdict and H103 scope-only verdict; only then promote quit fix.


--- Cumulative documentation and licenses ---
H103 RELEASE - DLC SNIPER SCOPE WITHOUT DOT

# H103 release - DLC sniper scope without dot

User requested correction and GitHub release on 2026-10-25.
Base: accepted H101 functions plus H102 level timer explicitly VALIDATED by user.
H102's broad sniper exemption was rejected because it also kept the reticle dot.
Goal/success: with No Crosshair ON, keep only native DLC sniper scope/mask; hide the
reticle dot and other crosshair elements. Other weapons stay hidden as before.

## Change
Replace whole-CrosshairNuclearSyringe exemption with per-CanvasRenderer ancestry.
Resolve the exact runtime class/namespace and its serialized fields _dot,
_zoomedInVisual and _maskImage; get each GameObject/Component Transform through
Unity metadata methods. Dot subtree exclusion wins even if nested inside scope.
Only scope/mask descendants get alpha=1; all other renderers keep requested alpha=0.
No Crosshair OFF restores normal alpha=1 behavior. Both multi-controller and fallback
paths share the same selection. Incomplete metadata never enables a broad exemption.
No parent deactivation, image/sprite replacement, aim/zoom logic or game file edit.

## Retained / status
Level timer code/format/defaults unchanged from user-validated H102.
H101 native F3 block/rebind, post-processing preference, mouse/input/updater, H97
native DLC wheel assets/cache and other commands retained. Personal INI/defaults
unchanged. Updater build identifier now V2A42-H103 (normal release, requested by user).
Release branch separate from main. No claim that main was merged/promoted.
CONSTRUIT / CONTROLE after CI/package checks; scope-only rendering remains unverified
in game until user test. Timer is VALIDATED by user. No game launched.

## Test and rollback
Restart after install, retain personal INI. With No Crosshair ON: equip DLC sniper,
verify no dot when unzoomed; zoom and verify scope/mask visible with no dot; zoom out,
swap weapons and verify other crosshairs hidden. Turn No Crosshair OFF and verify native
reticle restored. Check timer still runs with menu closed, and F3/cursor/wheels.
Rollback: restore H102 ASI/DXGI and personal INI backup (H102 retains the dot exception).
TODO: user scope-only verdict; do not treat compilation as visual validation.


--- Cumulative documentation and licenses ---
H102 TEST - LEVEL TIMER / DLC SNIPER SCOPE

# H102 TEST - Level timer and DLC sniper scope

Base: H101 explicitly accepted by user ("oui cest super") on 2026-10-17.
Goal: add persistent on/off native level timer; preserve only DLC sniper reticle/scope
when No Crosshair is enabled. H101 accepted behavior is retained.
Success: timer displays with menu closed, matches native level time and follows native
restart/end behavior; option persists. NuclearSyringe zoom scope visible with NoCrosshair=1,
all other weapon crosshairs still hidden. F3 rebind/wheels/cursor/updater intact.

## Timer
Visual -> Level Timer (game time), default OFF; [Settings] LevelTimer=0/1. Optional
[Keys] LevelTimer=0 default unbound, existing Change/Default controls available.
Top-right read-only HUD, resolution-scaled, MM:SS.cc. No cursor capture or mouse input.
Reads LevelController.get_TimeController -> TimeController.get_CompletionTime through
metadata on the game-window thread at up to 20Hz. A single queued refresh prevents
message backlog. Single atomic display snapshot; rendering never calls IL2CPP.
No game clock writes, no independent stopwatch, no resetting on toggle; enabling
mid-level shows elapsed native time. Hide when no active level / invalid getter.
Native static audit: TimeController.OnUpdate RVA e88130 accumulates float delta into
CompletionTime only when _count is true; ResetState e88150 sets count true/time zero;
EndLevel e88070 sets count false; getter 7ca620 reads the same time. Therefore this
is in-game level time, subject to the game's pause/time-scale/save-load semantics,
not real-time leaderboard timing or LiveSplit integration. Verify those in game.
Saved timer ON requests delayed startup initialization after renderer readiness +10s
using the existing opt-in Visual startup path; default OFF retains lazy init.

## Scope exception
Exact runtime class+namespace CrosshairNuclearSyringe in
Hyperstrange.PBD.PlayerComponents.Crosshairs owns _zoomedInVisual, _maskImage and
OnSniperZoomIn/Out. Entire crosshair kept at CanvasRenderer alpha=1 (user requested
this one weapon to retain its reticle); original requested alpha applies to all
other crosshairs. Both multi-controller and fallback paths use this exception.
No prefab/image replacement, no disabling parents, no changes to aim/zoom behavior.
Existing native crosshair reapply events retained. Log exemption once when encountered.

## Status / validation / rollback
EXPERIMENTAL until user runtime test. Build + isolated F3 hook regression fixture and
timer formatting boundary test required before packaging. No game launch or install edits.
H101 accepted F3 hook, post-processing, wheels, native sprites and input/updater retained.
Personal INI retained; missing timer keys use compiled OFF/unbound defaults.

Test: toggle timer in Visual; close mod menu, check timing during play/pause/time-scale,
restart level/change scene/complete level/load save; compare to native results time.
Restart with timer saved ON. Enable No Crosshair, equip DLC NuclearSyringe, zoom in/out,
swap to other weapons and verify only sniper retains crosshair. Recheck F3 and wheel.
Rollback: restore H101 ASI + backed-up personal INI; H101 ignores LevelTimer keys.
TODO: user timer/pause/end/load and sniper scope verdict, then decide next stable release.


--- Cumulative documentation and licenses ---
H101 TEST - WEAPON CAMERA POST-PROCESSING / NATIVE F3

# H101 TEST - Weapon camera post-processing / release native F3

Base: H100 experimental interface (on H99D + user-validated H97 gameplay).
H100 has no explicit user runtime verdict; H97 remains main/stable.
Goal: expose the exact native F3 effect as a rebindable Visual command and stop
its built-in F3 shortcut, so F3 can be assigned to any mod command.
Success: F3 alone no longer changes the native effect; a mod binding on F3 works
once; post-processing row toggles the same effect by click or another binding,
saves state, and restores it across restart/scene change. Wheels/cursor unchanged.

## Exact static evidence (local game, no runtime visual claim)
Assembly-CSharp PlayerWeaponCamera.Update at GameAssembly RVA 0x80e440 has only:
Input.GetKeyDownInt(284 = Unity F3), inherited PlayerCamera._camera, CameraExtensions
.GetUniversalAdditionalCameraData(camera), then inversion of the byte at +0x44.
Module-specific pointer tables identify GetUniversalAdditionalCameraData at
0x5aae560 in Unity.RenderPipelines.Universal.Runtime. That module's property
get_renderPostProcessing at 0x8500d0 reads +0x44 and set_renderPostProcessing
at 0x40b1da0 writes +0x44. The effect is weapon-camera URP post-processing.
It is not a general shadow/light toggle. Field offsets are evidence only; the
mod command uses metadata-resolved getter/setter methods rather than raw fields.

## Changes and defaults
- Visual: Weapon Camera Post-processing, checkbox and existing Change/Default
  key controls. [Keys] WeaponPostProcessing=0 (unbound). Assign F3 there or to
  another command; no existing binding is overwritten.
- [Visual] WeaponPostProcessing=-1 preserves scene defaults until first change.
  On successful change, 0/1 is saved and native readback verified. A saved choice
  requests delayed startup initialization after renderer readiness and 10 seconds.
- Game-window-thread refresh at most once/second resolves an active weapon camera,
  never a prefab/inactive object. Reads the real property; reapplies a saved choice
  when necessary (including scene changes). No scene object held across scenes.
  No playable camera means 'Load a level' and no setting change.
- Native F3 suppression: exact 86-byte normal-body signature of the audited method,
  AMD64 image bounds and page checks, then targeted MinHook no-op for this F3-only
  Update. Installed by worker before lazy IL2CPP initialization; no global Input
  hook, no key simulation, no disk/GameAssembly file patch. Unsupported code or
  hook failure leaves native F3 untouched with visible/logged status.
- MinHook initialization accepts already-initialized state for both native and
  DX11 hooks; fixes initialization-order conflict introduced by sharing hooks.
- H100 keyboard labels, prior native Visual checkboxes, H99D updater/input/cursor,
  H97 native wheel sprites/cache/alpha paths and personal-INI policy retained.

## Validation / risk
EXPERIMENTAL. Isolated native hook fixture compiled/executed in CI before packaging:
reject a changed signature, accept exact fixture with MinHook already initialized,
invoke hook 100 times without entering the original Input/camera body, disable hook
and verify original bytes restored. This does not prove behavior in the game.
No game launched, no game installation modified. Exact signature intentionally
rejects other game builds and conflicting patches. New entry needs a playable level.

## User test / rollback / TODO
Back up personal INI; replace ASI/DXGI from ZIP, retain existing INI (new keys use
compiled defaults). Restart game. Open Visual and verify 'Native F3 shortcut blocked'.
Load a level, press unbound F3: no post-processing change. Bind new row to F3,
press it and compare to checkbox; rebind to another key, then bind F3 to a different
mod command and confirm no post-processing double action. Test saved state after
restart and change of level; verify base/DLC wheel cold start and mouse restoration.
No Menu can hide game menus; mod menu remains available.
Rollback to H100 files and backed-up INI; removing H101 ASI on restart restores native
F3. Stable fallback H97. TODO: user runtime verdict; do not promote stable from CI.


--- Cumulative documentation and licenses ---
H100 TEST - KEYBOARD NAMES / PERSISTENT VISUAL OPTIONS

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


--- Cumulative documentation and licenses ---
H99D EXPERIMENTAL - menu rebind / supplied defaults / Unity cursor / updater
GodMode ID conflict addressed; Hotkeys tab rebinds menu key (F1 default).
Supplied INI is the new package and compiled default configuration.
Menu requests unlocked visible cursor without auto-pause, restores on close.
Updates: stable/beta check, confirm install after exit, backup and rollback.
INI preserved by updater; result file beside game. Runtime validation pending.
Rollback: exit and reinstall H98B or H97, preserve personal INI.

H98B EXPERIMENTAL - integrated ImGui DX11 menu from H97 stable
F1 open/close, movable/resizable window, five tabs, arguments, key table.
No runtime validation yet; test input, alt-tab/resize, native wheels and exit.
Rollback: exit and reinstall H97 stable, preserve personal INI.

POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H97
STATUS: VALIDATED BY USER - 2026-10-14
Stable release rebuilt from unchanged user-tested H97 source.

Cumulative validated menu/gameplay, Base/DLC wheels, native tap/hold,
lazy startup, target cache, startup-logo skip and cross-mode selection preserved.
DLC icons/backings directly in normal campaign, no DLC level visit required.
Stale normal silhouettes removed; original colors restored on close.
Native sprite/texture retention across scene transitions.

INSTALLATION
Exit game. Copy four files beside POSTAL Brain Damaged.exe.
Preserve your personal PostalBorkenMenu.ini. Remove -debug from launch options.
No original game assets or saves modified.

PACKAGE
Exactly dxgi.dll, PostalBorkenMenu.asi, PostalBorkenMenu.ini, README.txt at root.

NOTES
First cold prefab resource load may hitch; no player is instantiated.
Other game builds not independently validated. Failed load keeps DLC-visit fallback.
H97 TEST log banner identifies unchanged validated code.
Rollback: exit and reinstall an earlier preserved package; keep personal INI.
THIRD-PARTY LICENSES
The MIT License (MIT)

Copyright (c) 2014-2025 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

MinHook - The Minimalistic API Hooking Library for x64/x86
Copyright (C) 2009-2017 Tsuda Kageyu.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

================================================================================
Portions of this software are Copyright (c) 2008-2009, Vyacheslav Patkov.
================================================================================
Hacker Disassembler Engine 32 C
Copyright (c) 2008-2009, Vyacheslav Patkov.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-------------------------------------------------------------------------------
Hacker Disassembler Engine 64 C
Copyright (c) 2008-2009, Vyacheslav Patkov.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

JSON for Modern C++ / nlohmann 3.11.3
MIT License 

Copyright (c) 2013-2022 Niels Lohmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

