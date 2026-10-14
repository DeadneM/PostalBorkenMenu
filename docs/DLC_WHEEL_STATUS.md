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

## H87 visual regression / H88 hypothesis, 14 October 2026

- H87 clears the immediate parent `WeaponImage` UI.Image sprite while the
  Image remains enabled. User screenshot shows 5 large white rectangles on
  the alternative wheel. H87 is rejected.
- The white rectangles are consistent with Unity UI.Image fallback rendering
  when its sprite is set to NULL.
- H88 starts strictly from H80. It never clears or disables wheel-sector
  imagery. A secondary Image is modified only if the source sprite is an
  exact known weapon icon or the same object as the foreground's original
  sprite, then it is swapped to the target weapon sprite and restored.
- If the parent sprite is generic, the guard leaves it untouched. A lack of
  visual change will indicate that the pink/violet silhouette is generated
  elsewhere (another image, a mesh effect such as Outline, or integrated
  into the weapon sprite). Do not misreport that as success.
- Acceptance: 9/9 wheel sectors intact, no white rectangles, correct
  secondary pink/violet icon silhouettes, original tap/hold and equip intact,
  close/reopen and scene transitions clean.


## H88 actual game result and H89 test design (14 October 2026)

Normal mode: base wheel works; DLC wheel remaps all five WeaponIds but
none of its five DLC `_Rose` icons are currently loaded. Initial Sprite
cache is 5/10. User observes mild wheel-opening freeze.

DLC mode: native DLC wheel perfect. Cross-mode BASE wheel icons are
correct, but five stale `_Empty` icon silhouettes remain. Exact names
confirmed by H88 runtime log:
NyanBrella, Piss Gun, Wiener-Grinder, UrAnusium and Bubble Gun
(`Weapon Icon_..._Empty`).

H89 test, still based on H80 validated behavior:
- suppress only these exact `_Empty` UI.Image components via
  `UnityEngine.Behaviour.set_enabled(false)` while BASE wheel is open
  in the DLC scene
- restore the original Image.enabled state on native wheel close
- keep the foreground Base sprite and all nine native sectors intact
- replace the two costly global Sprite scans with a single bounded scan
  and a one-time direct `Resources.Load(string,Type)` for five DLC icons
- do not claim the direct lookup solves scene-only unloaded assets

Pass criteria: no stale DLC backgrounds on BASE wheel in DLC scene; native
DLC wheel unchanged; normal wheel unchanged in normal scene; no added
latency; DLC icons if Resources.Load can access them.


## H89 in-game result and H90 performance test

User confirms H89 DLC scene: cross-mode BASE and native DLC wheels are both visually correct and functional. Minor BASE wheel freeze remains. H89 log proves all five `Weapon Icon_*_Empty` Image components disabled/restored correctly each opening.

H89 normal scene: native BASE wheel fine; cross-mode DLC wheel still displays no correct DLC icons. Source of missing art proved: cache 5/10; `Resources.Load(string,Type)` 0 DLC sprites. Minor freeze persists.

H90 isolates performance by caching canonical `WeaponId` targets scoped to native wheel instance (invalidate on instance change) and measuring remap milliseconds for both directions. Crosshair runtime alpha handling is unchanged, only duplicate log rows are coalesced. No change to DLC icon source. Runtime cache lifespan at level/reload still requires user confirmation, since the source reference to the current wheel is an unmanaged pointer and cannot substitute for in-game testing.
