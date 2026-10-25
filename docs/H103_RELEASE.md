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
