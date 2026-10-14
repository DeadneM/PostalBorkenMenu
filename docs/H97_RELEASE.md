# V2A42-H97 - user-validated stable release

User validated H97 on 2026-10-14 including cold normal-campaign DLC wheel graphics
without visiting a DLC level, and requested GitHub promotion/release.
Validated source c92c73e unchanged for this stable rebuild.

Cumulative menu/gameplay, native tap/hold, lazy startup, wheel caches, native
sprite/texture retention, exact normal silhouette transparency/restoration and
one-shot postalplayer_dlc Resources asset loading (without Instantiate).
INI defaults, shortcuts, loader and gameplay code preserved.

Exactly four root files: dxgi.dll, PostalBorkenMenu.asi, PostalBorkenMenu.ini,
README.txt. Install beside game EXE; preserve personal INI, remove -debug.
No original game assets or saves modified.

First cold load may hitch; other game builds are not independently validated.
Failed asset/API loading keeps the DLC-visit fallback. H97 TEST log banner is
intentionally retained to avoid changing code after user validation.
Rollback: exit and reinstall an earlier preserved package, keep personal INI.
Next: H97 is the stable base; future experiments on separate branches.
## Stable H97 delivery verified - 2026-10-14
User VALIDATED H97 cold normal loading and requested stable release.
Main/release source commit 06edb6177e72acddcce892bef7fa7d3a2fa4aa8d.
Source/config/loader unchanged from validated H97 test c92c73e.
CI success https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38608549197
Stable/latest https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H97
Published ZIP downloaded, four root files: DXGI 9216, ASI 135680, INI 2296,
README 1115 bytes. Both PE AMD64 checked; digest matches GitHub asset:
9dd1e65fb585a70d5486a4834e20be8e2690ec407e8f07014efcd69fc70d3b2d
Remote main/tag verified, prerelease=false, releases/latest=V2A42-H97.
Next: preserve H97 as validated base; no pending functional fix in user-tested scenario.
