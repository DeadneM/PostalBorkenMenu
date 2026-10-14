# DLC / Base Weapon Wheel Status

## Validated checkpoint: V2A42-H80

H80 is the current validated development checkpoint. It preserves H74's native tap/hold weapon-wheel semantics and H77's lazy IL2CPP startup architecture. User validation confirms the current functions behave correctly; only the alternative wheel icon background remains visually wrong.

It combines only the parts validated in game:

- H59: logical weapon pairing by native WeaponId slots 1, 2, 3, 4 and 8
- H61: primary cross-mode weapon-icon replacement
- H62: cached one-scan sprite lookup for fast wheel opening
- H63: focus-independent F1/window initialization

Broad secondary/background visual experiments from H62/H63/H64 are not part of H65.

## Variable slot pairs

| Native slot | Base game | These Sunny Daze |
| --- | --- | --- |
| 1 | Shovel | UmDrill |
| 2 | Pistol | PissGun |
| 3 | Shotgun | MeatShotgun |
| 4 | MachineGun | BubbleGumMachineGun |
| 8 | DildoBow | NuclearSyringe |

Slots 5, 6, 7 and 9 are common/native and are never remapped.

## User-validated wheel behavior

- normal wheel in normal mode: correct
- DLC wheel in DLC mode: correct
- cross-mode population: 9/9
- cross-mode weapon calls: correct
- cross-mode primary weapon icon: correct
- wheel shortcut performance: fixed by H62 cache
- F1/menu window initialization no longer depends on POSTAL being foreground at startup
- H74 restores native short-tap previous-weapon swap and hold-to-open semantics for both wheel shortcuts

## Remaining visual issue

The remaining defect is the weapon-specific background/silhouette behind the corrected icon on the alternative cross-mode wheel.

Runtime audits prove `WeaponWheelButton._backgroundImage` is not that layer. It uses the same generic `weapon_wheel_part_light` sprite in both normal and DLC scenes.

Do not rework H59, H74 or H77 while fixing this remaining visual layer. H80 is now the preferred validated checkpoint.

## H68 detector

H68 experimentally validates a reliable mode detector from the live native wheel composition:

- normal game: 9 base-category buttons / 0 DLC-category buttons
- These Sunny Daze: 4 base-category buttons / 5 DLC-category buttons

This is preferred over the rejected H67 SceneManager name detector.

## Startup stability

H77 changes startup architecture without changing validated H74 weapon-wheel behavior:

- no automatic IL2CPP initialization during process startup
- H39 startup-logo skip remains active
- F1 or the first bound gameplay hotkey triggers lazy IL2CPP initialization on the mod worker thread
- the H13 metadata audit is skipped during lazy init
- the H62 sprite prewarm is not performed at startup

This is the validated fix for the pre-menu freeze observed with earlier automatic-startup initialization.

## Current visual target

The remaining cross-mode issue is visual only: identify the exact icon-background / secondary silhouette layer behind the corrected foreground icon. Do not change the validated H59/H74 selection/input path or the H77 lazy-startup architecture while investigating it.

## Stability note

V2A42-H43 remains the stable public release.
V2A42-H80 is the current validated development checkpoint.

## H90 validated source checkpoint, 2026-10-14

User reports H90: These Sunny Daze DLC and alternative normal wheels work and
display the right icons and weapon-specific backgrounds. Native normal-game
wheel remains correct. Opening freezes are reduced. The ONLY reported visual
issue is normal campaign -> alternative DLC wheel: base-weapon silhouettes
are still visible behind the otherwise correct DLC icons.

H90 is now the source checkpoint on main. H80 stays the last public stable
release until the final cosmetic defect is resolved. H90 prerelease tag and
test ZIP: V2A42-H90-TEST.

H91 experiment is developed and tested separately on
`dev/v2a42-h91-symmetric-base-underlay-hide`; it turns off only the
`WeaponImage` parent secondary Image in the normal -> DLC direction,
strictly preserving H89's reverse fix, H90 performance caches and H74/H77
input/init.


## H93 user verdict: rejected / resource origin required

User tested H93 and reported DLC wheel still has no correct DLC icons. Thus H91, H92 and H93 are all rejected for resolving the normal-campaign DLC-wheel visuals. Previous H90 user's screenshot showed correctly rendered DLC foreground icons and rose backings with stale normal silhouettes; later tests of H91–H93 did not reproduce this. That discrepancy suggests scene / asset availability differences but is NOT proved without the newest H93 log.

The current H89 observed cold/normal sprite cache was 5/10; direct Resources.Load yielded 0/5 DLC sprites. H93 can only cache objects already created by Unity. Correct solution requires direct identification of serialized DLC Sprite / atlas / texture data and reliable scene-independent loading from user-owned game assets; or an export supplied by the user. Never hide parent `WeaponImage` in normal->DLC: H91 demonstrated that this removed legitimate DLC graphics. Do not touch the verified DLC->BASE H89 logic or H90 performance cache.

## H94 experiment (2026-10-14)

H90 remains the validated base; H91-H93 remain rejected. H94 retains five exact
native DLC sprites and their textures with strong IL2CPP handles and Unity
DontUnloadUnusedAsset, then checks native lifetime before reuse. No new masking,
no cold loading. Runtime validation pending. See [H94 test protocol](H94_TEST.md).

## H95 experiment - 2026-10-14

User confirms H94 DLC sprites/backings survive DLC -> normal. Retain H94 cache.
H95 tests exact normal silhouette Image.color alpha=0 and restoration on close.
No parent disabling, sprite clearing, INI or selection changes. Cold normal
loading unresolved. See docs/H95_TEST.md (or H95_TEST.md from docs).

## H96 - H95 rejected, slot mapping correction
User reports H95 unchanged. Log: only shotgun alpha changed; four parents preserved.
H95 confused button traversal backup index with canonical weapon-slot index.
H96 passes SlotIndexH59(slot) separately for silhouette/retained sprite checks.
H94 retention remains confirmed for DLC -> normal; cold loading unresolved.
See H96_TEST.md in docs. Runtime visual result pending.

## H97 - H96 validated, cold prefab loading experiment
User validates H96 icons/backings and normal underlay removal after DLC -> normal.
Read-only ResourceManager audit identifies postalplayer_dlc -> resources.assets
GameObject 2369; its WeaponSelectedImage descendants reference all five Rose sprites.
H97 loads only this prefab asset without Instantiate, captures exact sprites into
H94 retained cache, preserves H96 rendering. Runtime cold loading pending.
See H97_TEST.md in docs for evidence, limits, test and rollback.

## H97 stable promotion - 2026-10-14
User validates cold normal-campaign DLC sprites and requests stable release.
H97 source unchanged; main promoted. See H97_RELEASE.md in docs.
H95 rejected remains historical; H94/H96 retained features now cumulative H97.


## H98 ImGui experiment
User selects actual integrated ImGui, matching Darksiders dark/blue style.
H97 remains stable. See docs/H98_TEST.md for controls, acceptance and rollback.
