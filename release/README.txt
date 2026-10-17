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

