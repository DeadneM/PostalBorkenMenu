# DLC / Base Weapon Wheel Status

## Validated checkpoint: V2A42-H77

H77 is the current validated development checkpoint. It preserves H74's native tap/hold weapon-wheel semantics and moves IL2CPP initialization out of process startup.

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

A second scene-native weapon silhouette can remain visible behind the corrected cross-mode weapon icon.

Runtime audits prove WeaponWheelButton._backgroundImage is not that silhouette. It uses the same generic sprite, weapon_wheel_part_light, in both normal and DLC scenes.

Do not rework H59, H61, H62 or H63 while fixing this remaining visual layer.

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

The remaining cross-mode issue is visual only: identify the exact secondary weapon-silhouette layer that can remain behind the corrected primary icon. Do not change the validated H59/H74 selection/input path while investigating it.

## Stability note

V2A42-H43 remains the stable public release.
V2A42-H77 is the current validated development checkpoint.