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

## H95 delivery check - 2026-10-14
CONSTRUIT / CONTROLE / EXPERIMENTAL; user visual validation pending.
Release/source commit 07195ba9ffdd6e9ec9a13e57679ff4161e1913f3.
Successful CI https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38594659274
Release https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H95-TEST
Downloaded ZIP exactly four root files. ASI 133120 bytes, DXGI 9216 bytes,
INI 2296 bytes, README 2325 bytes. Both binaries MZ/PE AMD64 checked.
SHA256 1ad6b5eda47b1a4fe44335c2eb823861af675d4d94304b4736acdbb9f0910c08
matches GitHub asset digest. Main remains 7d660eb. INI/loader/H94 rooting unchanged.
Initial publish run encountered HTTP 403; corrected source run published successfully.
A subsequent workflow-only commit adds ZIP artifact preservation; it does not
change the released source and its release-create step may report existing tag.
Next: user confirms removal of normal underlays without loss of DLC graphics.
