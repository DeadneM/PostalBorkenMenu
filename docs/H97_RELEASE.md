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