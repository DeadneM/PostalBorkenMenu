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
