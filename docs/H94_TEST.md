# H94 - Persistent native DLC sprites

Base: user-validated H90 code, promoted by commit 14619d9. Main documentation
records H91/H92/H93 rejected. This experiment carries no masking from those builds.

Goal: open the native DLC wheel in These Sunny Daze, capture its five exact Rose
sprites by native WeaponId category/slot, and reuse those same objects after
returning to the normal campaign in the SAME game process.

Capture occurs after successful native OnWheelDown invocation, before any mod
remapping. Exact names must match H56 observations. A successful invocation is
not proof of correct rendered graphics; the user's visual check is required.

Each accepted Sprite and its Texture2D receives a strong, non-pinned IL2CPP GC
handle and the existing hideFlags OR DontUnloadUnusedAsset (32). Existing flags
are preserved. If retention fails, handles are freed and sprite flags restored.
Only five pairs may be retained, for the lifetime of the process. Textures may
be shared atlases, so retained memory can exceed the five visible icons.

Before reuse, Unity Object.op_Implicit checks both native objects. Destroyed
objects are never returned by the retained-cache path. Explicit Destroy or
AssetBundle.Unload(true) can still invalidate assets; this build does not block
those calls or recapture invalid slots. Restart resets the experiment.

No scene, wheel, button, Image or parent GameObject is kept alive. DontDestroyOnLoad
is not applied to sprites. No images/backgrounds are hidden by H94. H89's already
validated reverse underlay fix and all H90 wheel logic/cache/timing are preserved.
No additional global sprite scans, disk asset edits, INI or shortcut changes.

Limitations: no cold loading, extraction, disk persistence or guaranteed graphics
repair. Start directly in normal campaign and this new cache has no source.
The remaining normal silhouettes are deliberately outside this test's scope.
Calls use the existing H90 attached worker thread; thread attachment alone does
not prove Unity main-thread safety. Runtime exceptions cause retention to fail.
No in-game execution has been performed by the build system.

Test / acceptance:
1. Fully restart with H94; cold normal campaign is a control, not expected success.
2. Visit DLC, open its native DLC wheel, confirm all five correct icons and backings.
3. Expect five H94 retained-pair log entries (indices 0-4) and capture count 5.
4. Return to normal campaign without exiting; open DLC wheel, verify those five
   Rose icons persist. Repeat scene transitions and both wheel shortcuts.
5. Check tap/hold, selection, normal wheel, DLC wheel and reduced hitch remain H90.
6. Supply the session log and verdict. Missing capture versus lost graphics after
   capture separates asset availability from lifetime/atlas issues.

Status: EXPERIMENTAL; build/package integrity is established only by successful CI
and ZIP checks. User validation remains pending. H90 is still the validated base.
Rollback: exit the game and reinstall the H90 four-file package; keep personal INI.
Next: use the runtime result before altering any underlay or cold-loading path.

## H94 delivery verification - 2026-10-14

CONSTRUIT / CONTROLE / EXPERIMENTAL. In-game validation pending.
Source/release commit: bccf71cce8be8e83342a62877f9412ea458f3bbe.
Build: https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38592335981
CI completed successfully, ASI and DXGI build/package/publication passed.
Release: https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H94-TEST
Downloaded published ZIP: 57795 bytes, exactly four root entries:
dxgi.dll (9216), PostalBorkenMenu.asi (131072), PostalBorkenMenu.ini (2296), README.txt (1991).
Both binaries checked for MZ/PE signature and AMD64 machine 0x8664.
ZIP SHA256: 4c83737d2e1c92d0c0b4b6ccdb870f32231bc7639153bd2e97729f775ba3774d
Matches GitHub asset digest and CI log. Main stayed at 7d660eb.
INI, loader and IL2CPP interface part1 unchanged relative to H90 promoted source.

Unity documents DontUnloadUnusedAsset as preventing Resources.UnloadUnusedAssets:
https://docs.unity3d.com/es/2020.1/ScriptReference/HideFlags.DontUnloadUnusedAsset.html
This is narrower than protection from explicit destruction/bundle unload.

TODO: obtain same-process DLC -> normal result and log before next build.
No current session game logs or IL2CPP dump were available locally; analysis used
repository source, historical audit records, user verdicts and Unity documentation.
