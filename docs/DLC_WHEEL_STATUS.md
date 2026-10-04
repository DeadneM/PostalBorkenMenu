# DLC / Base Weapon Wheel Status

## Validated checkpoint: V2A42-H59

H59 is the current validated development checkpoint for weapon-wheel logic.

It is built from the H57 9/9-wheel baseline and replaces the old guessed name-order mapping with native `WeaponId.get_Slot()` pairing.

### Variable slot pairs

| Native slot | Base game | These Sunny Daze |
| --- | --- | --- |
| 1 | melee / Shovel | UmDrill |
| 2 | Pistol | PissGun |
| 3 | Shotgun | MeatShotgun |
| 4 | MachineGun | BubbleGumMachineGun |
| 8 | DildoBow | NuclearSyringe |

Slots **5, 6, 7 and 9** are common/native and are not remapped.

This specifically fixes the H57-era logical errors where:
- melee and pistol could be inverted,
- CatCanon could be treated as a variable weapon twice,
- the slot-4 machine gun was absent from the variable mapping,
- shotgun/cat-gun behavior could be shifted.

## User-validated H59 behavior

- Normal wheel in normal mode: correct logical weapon set.
- DLC wheel in DLC mode: correct logical weapon set.
- Both wheel contexts populate all 9 sectors.

## Known remaining cross-mode limitation

The wheel identity/presentation is still scene-native:

- DLC wheel requested in normal mode currently presents the normal-mode wheel.
- Normal wheel requested in DLC mode currently presents the DLC-mode wheel.

This means the remaining problem is **not** the H59 weapon-slot pairing.

Do not rework the validated H59 slot map unless a later test disproves it.

## Next target

The next build should preserve H59 exactly and investigate how the game chooses:
- the active wheel layout,
- per-sector button identity,
- per-sector icon/sprite source,
- scene/mode-specific wheel presentation.

Icon correction should follow the wheel-identity fix rather than be developed independently against the wrong wheel presentation.

## Stability note

V2A42-H43 remains the stable public release.
H59 is a validated development checkpoint and is published as a prerelease/test build.
