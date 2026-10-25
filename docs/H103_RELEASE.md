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

## Verified delivery
CONSTRUIT / CONTROLE. Timer remains user-VALIDATED; new scope-only behavior awaits
in-game confirmation. Source/tag ae9d2c1a7783356e04f8badf12765963d1e9433f.
CI 40214782982 succeeded (build, native F3 fixture, timer format tests, package/release).
Normal GitHub release V2A42-H103 verified published, draft=false, prerelease=false.
Downloaded ZIP SHA256 matched GitHub digest:
f1c5671dabc49ff9d702702f1284e60365db1a1210bf0660c756e9ccb55d2d5a.
Four root files, AMD64 verified: dxgi.dll 9216; ASI 1005568; INI 2638; README 21324 bytes.
Timer implementation/format code unchanged except build labels; personal INI unchanged.
Main remains c1e4630767db8be3b6eab6d6c351e3a7fe23eff3; release is on its separate branch.
No game launched or installation modified. Next: scope/mask visible, dot absent, NoCrosshair ON.
