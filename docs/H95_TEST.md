# H95 - exact normal silhouette alpha

Base: H90 validated wheel logic + H94 sprite retention, confirmed visually by
the user on 2026-10-14 after DLC -> normal within one process. H94 is retained
for this scenario; cold normal loading remains unresolved. H91-H93 rejected.

H94 log: all five native Rose sprite/texture pairs retained. In normal campaign
the five DLC icons were assigned repeatedly; user confirms icons and DLC backings
visible with stale normal silhouettes underneath. Timings 422 then 328 ms.

Objective: remove only those five stale normal silhouettes, preserving the DLC
icons/backings, native wheel selection, all shortcuts, INI, H90 cache/performance
and the validated H89 DLC -> base fix.

Change: within the existing local parent-Image traversal, match each slot's
EXACT normal silhouette name from the H94 runtime log. Require that its target
is the living H94 retained DLC sprite and that primary icon assignment succeeded.
Save the Image's RGBA color, set only its own Graphic.color alpha to zero, then
restore its original color on close. Do not disable Image/parent/GameObject,
modify CanvasGroup or clear a sprite. Descendant selected-image color is untouched.
An unknown sprite or unavailable API leaves the image unchanged. A game-driven
color update could overwrite this alpha while open; that remains a runtime risk.
The previously existing attached-worker-thread Unity-call limitation persists.

Slot -> exact normal silhouette:
- 1: Bez-nazwy-1_0009_Warstwa-9
- 2: Bez-nazwy-1_0014_Warstwa-4
- 3: Bez-nazwy-1_0011_Warstwa-7
- 4: Bez-nazwy-1_0013_Warstwa-5
- 8: Bez-nazwy-1_0017_Warstwa-1

Test: restart with H95, visit DLC and open native DLC wheel, return to normal
without exiting. Open DLC wheel: all five Rose icons/backings must remain, all
five normal silhouettes must disappear. Expect five H95 alpha=0 entries and
five color-restored entries on close. Confirm normal wheel afterwards; repeat
tap/hold, selection, transitions and opposite DLC -> normal wheel behavior.
Any missing DLC graphics or persistent transparency is rejection; restart and
reinstall H94 (preserve personal INI). No game asset files are changed.

Status: EXPERIMENTAL until user validation. Build/ZIP verification is separate.
TODO: user visual verdict and log; cold start remains a later asset-loading task.

Graphic.color is an individual Graphic's vertex color:
https://docs.unity3d.com/es/2017.4/ScriptReference/UI.Graphic-color.html
