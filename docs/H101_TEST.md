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
