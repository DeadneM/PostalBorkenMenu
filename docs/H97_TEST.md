# H97 - cold DLC sprites from verified Resources prefab

Base: H96 source/tag 55e7114, documentation head 767ddcf. User explicitly
validated H96: DLC Rose icons/backings retained and normal underlays removed
after visiting DLC. Remaining issue: cold normal campaign requires DLC warmup.
H95 rejected; H94 retention and H96 slot-correct alpha remain accepted features.

Read-only asset audit of user-owned installation on 2026-10-14:
- All five expected Sprite objects are in resources.assets: NyanBrella 226,
  Piss Gun 265, Wiener-Grinder 218, Bubble Gun2 243, UrAnusium2 251 (path IDs).
- globalgamemanagers ResourceManager.m_Container contains postalplayer_dlc ->
  file ID 5 / path ID 2369. External file 5 is resources.assets.
- Those five Sprites do not have their own ResourceManager container entry;
  the old H89 Resources.Load(sprite-name, Sprite) attempts used non-addresses.
- Prefab traversal covers 232 GameObjects. Descendant WeaponSelectedImage
  components serialize local PPtrs at byte offset 88 to all five exact sprites.
  There is also an UrAnusium1 variant, deliberately ignored by exact-name checks.
  Scripted type layouts are stripped: raw component references are evidence of
  prefab dependencies, not an independent proof of managed field layout.
- Reproducible read-only audit: docs/audit_h97_resources.py, UnityPy 1.25.4.
  No textures or game assets are copied into the mod or published.

Goal: from a completely fresh process, go directly to normal campaign and
open DLC wheel with correct five icons/backings and no stale normal underlays.

Implementation: after existing DLC ownership/live normal wheel checks, one
bounded Resources.Load("postalplayer_dlc", GameObject) at first normal DLC wheel
request. Never Instantiate, activate, spawn a player or load a DLC level. Traverse
only this asset's child Images (includeInactive, max 512), accept the five exact
Rose names, then use unchanged H94 strong sprite/texture retention. Temporary
prefab and Image-array GC roots released after capture; no Destroy/Unload calls.
No global rescan added. H96 matching/restoration and native wheel logic unchanged.

Limitations: Unity Resources.Load and prefab traversal on the existing attached
worker still need runtime proof; thread attachment does not prove main-thread
safety. Loading prefab dependencies may create a one-time hitch/memory increase.
The prefab may differ on another game build. Failed APIs/load/partial capture
leave H96 fallback (visit DLC) available; no retry loop in the process. No cold
success claimed from serialized files or compilation alone.

Test: fully exit/restart H97. Load normal campaign directly, never visit DLC.
Open DLC wheel. Expect cold retained DLC sprites (of 5): 5, then five existing
H96 alpha changes and corresponding color restores. Check all nine sectors,
icons/backings, tap/hold/selection and normal wheel after close. Repeat open:
there must be no repeated H97 prefab load. Repeat normal-level transition.
If cold capture fails, visit DLC and verify the H96 fallback is still intact.
Send log plus visual verdict. Failure/crash/regression -> exit and reinstall H96,
keeping personal INI. Original game files and saves are not modified.

Status: EXPERIMENTAL; user validation pending. TODO: cold result, initial load
timing and repeated transitions. H96 remains validated until user approves H97.

Unity Resources.Load requires the Resources-relative path and type:
https://docs.unity3d.com/ja/2021.2/ScriptReference/Resources.Load.html

## H97 delivery verification - 2026-10-14
CONSTRUIT / CONTROLE / EXPERIMENTAL; cold gameplay result not yet tested.
Source/release commit c92c73ef3bb212daf95c587b0ef38db37c208bb8.
CI success: https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38606798582
Release: https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H97-TEST
Published ZIP downloaded: four root files, dxgi.dll 9216, ASI 135680,
INI 2296, README 3134 bytes. Both binaries MZ/PE AMD64 verified.
ZIP SHA256 0a49e10e8ecd35a2fba14948f0265f011933c6d600c898cb55b654e70a5ff19c
matches GitHub asset digest. Remote tag and branch verified; main still 7d660eb.
INI/loader/H96 rendering and H94 retention functions unchanged.
Next: fresh normal process, cold capture 5/5, visual result, initial load timing.

## H97 stable promotion - 2026-10-14
User validates cold normal-campaign DLC sprites and requests stable release.
H97 source unchanged; main promoted. See H97_RELEASE.md in docs.
H95 rejected remains historical; H94/H96 retained features now cumulative H97.

