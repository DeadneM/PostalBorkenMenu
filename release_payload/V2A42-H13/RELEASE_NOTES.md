# PostalBorkenMenu V2A42-H13

Validated cumulative release for **POSTAL: Brain-Damaged**.

> **Borken is intentional.** It is a nod to *POSTAL: No Regerts*.

## Highlights

- Five-tab in-game developer menu
- Reliable configurable hotkeys and persistent INI settings
- **Grappling Hook** support through the validated native sequence:
  - resolve exact `WEAPON_Hook`
  - `AddWeapon` only when required
  - always call `WeaponsInventory.InitHook()`
  - verify `get_Hook()`
- **Base Weapon Wheel** restored to the validated H4 path
- **No Crosshair** remains visual-only and now covers loaded DLC crosshair controllers without clearing or replacing the native crosshair object
- **Give All Weapons** without quest-item injection
- Named Arsenal and Special weapon entries
- **TimeScale** toggle behavior preserved
- V2A8 early-shutdown / IL2CPP lifetime stability preserved
- Includes the project-standard **x64 DXGI ASI loader**

## Rejected research branches

The experimental DLC Weapon Wheel branches **H9, H10, H11 and H12 are not part of this release**.

Future gameplay/source work starts from **V2A42-H13**.

## Known open item

**Skip Intro is not fixed in this release.**

The old synthetic Escape/input approach is permanently rejected. A future implementation must reproduce the game's native intro-skip / `NO_VIDEO` behavior without simulating keyboard or mouse input.

## Installation

Copy all four files from the ZIP into the POSTAL: Brain-Damaged game directory:

```text
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt
```

The included `dxgi.dll` is the minimal x64 ASI loader used by the project. It forwards the real System32 DXGI exports and loads `PostalBorkenMenu.asi`.

## Important

- Remove `-debug` from the game's launch options before using PostalBorkenMenu.
- No on-disk patching of the game EXE, `GameAssembly.dll`, or `UnityPlayer.dll` is performed.
- `PostalBorkenMenu.log` is generated at runtime and is not included in the release ZIP.

## Validation

V2A42-H13 was explicitly validated in-game before publication.

The release build is compiled from the canonical `main` source and the workflow appends its exact SHA-256 hashes below.
