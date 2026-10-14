# H96 - correct silhouette matching by weapon slot

Base: H90 + H94 retention confirmed by user for same-process DLC -> normal.
H95 user verdict REJECTED: visually unchanged. H94 remains the retained feature.

H95 runtime log shows only pair 2 (shotgun) alpha=0 and restored, twice.
Four normal silhouette parents were explicitly preserved. Source error:
visualIndex is the order of the remapped buttons (pistol, shovel, shotgun,
bow, machinegun), not the canonical weapon-slot order (1,2,3,4,8).
Both exact silhouette matching and retained sprite pointer comparison incorrectly
used visualIndex. Only the shotgun index coincided. The assistant introduced
this mapping error in H95; H96 separates canonical nativePair from backup index.

Change: pass SlotIndexH59(slot) to the local image replacement routine for exact
normal silhouette and H94 sprite-root checks. Keep visualIndex only for backup
and restoration, as required by H90 callers. Native weapon logic is unchanged.

Goal: five exact normal silhouette Graphic colors become transparent while
five native DLC icons and backings remain. No Image.enabled, parent/GameObject,
CanvasGroup or sprite changes added; original Image color restored on close.
H95 alpha behavior is experimental, not validated even for the shotgun. Native
game code may overwrite alpha while open; this correction does not prove it won't.
Same-process DLC warmup is required; cold normal loading remains unresolved.

Test: restart H96, open native DLC wheel in DLC, return to normal without exiting.
Open DLC wheel: expect FIVE H96 alpha=0 entries and FIVE restores on close.
Confirm all normal underlays disappear, all DLC graphics remain, and normal
wheel afterwards is intact. Repeat selection/tap/hold and reverse wheel behavior.
If five changes log but visuals remain unchanged, investigate rendering/update
behavior rather than claim another mapping repair solved the feature.

Status: EXPERIMENTAL, user validation pending. Build/ZIP controls separate.
Rollback: exit and reinstall H94 package, preserve personal INI.
TODO: user visual verdict and H96 log, then cold loading only after this succeeds.

## H96 delivery verification - 2026-10-14
CONSTRUIT / CONTROLE / EXPERIMENTAL, not user-validated.
Release/source: 55e7114394e3940fc752d5da71062290935ef0b9.
CI success: https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38597020252
Release: https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H96-TEST
Published ZIP downloaded: exactly dxgi.dll (9216), PostalBorkenMenu.asi (133120),
PostalBorkenMenu.ini (2296), README.txt (2712), all at root. AMD64 PE verified.
SHA256 b10e7eca6e7cca3a01c7b84ab06fe7b7b14ca9d43924d5d4c458a6faf362b45d
matches GitHub digest. Main unchanged at 7d660eb; INI/loader/H94 rooting preserved.
Next: five H96 alpha changes/restores plus user's visual verdict. H95 rejected;
H94 retention remains the confirmed feature for warmed same-process transitions.

## H97 - H96 validated, cold prefab loading experiment
User validates H96 icons/backings and normal underlay removal after DLC -> normal.
Read-only ResourceManager audit identifies postalplayer_dlc -> resources.assets
GameObject 2369; its WeaponSelectedImage descendants reference all five Rose sprites.
H97 loads only this prefab asset without Instantiate, captures exact sprites into
H94 retained cache, preserves H96 rendering. Runtime cold loading pending.
See H97_TEST.md in docs for evidence, limits, test and rollback.

