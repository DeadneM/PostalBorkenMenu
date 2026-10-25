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

## Verified release delivery
Source/tag c103137a3f92982fa0238bffa714b9b433f08c18. CI 40216923799 successful:
quit ABI/ordering, defaults consistency, timer boundaries and F3 fixture.
Normal public release V2A42-H104 verified draft=false, prerelease=false, latest=true.
Downloaded ZIP digest matched GitHub:
5b3d9f1eb0868228238a7be51f74898c2be5c0cad9986cdafd2da29424a7a931.
Four root entries / AMD64 verified: dxgi 9216; ASI 1006592; INI 2623; README 27507.
Release behavior unchanged from accepted H104 TEST. Release branch and tag published;
main remains unchanged pending separate authorization. Historical workflows unchanged.
Local game remains on accepted H104 TEST; no game launched or personal INI modified.

## Main promotion authorized
User explicitly approved promotion to main after the release delivery.
H104 release sources promoted by fast-forward, without force or rewriting history.
Historical H43 publisher now manual-only to prevent obsolete publication on main push.
H104 public tag/package remain unchanged; experimental branch and older release tags retained.
