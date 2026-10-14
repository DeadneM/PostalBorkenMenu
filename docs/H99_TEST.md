# H99 - menu input, supplied defaults and updater

Base: H97 validated gameplay + H98B integrated ImGui retained by user, with
GodMode rebind conflict and cursor requiring pause reported on 2026-10-14.
No gameplay fixes claimed. H97 remains stable main; H99 is experimental.

Screenshot: ImGui conflicting-ID warning. Table header column 4 "Change" and
GodMode command index 4 "Change" used the same integer ID scope. H99 adds a
CommandRows scope distinct from headers. Rebind implementation otherwise retained.

Defaults: supplied personal INI copied into config/PostalBorkenMenu.ini; every
known command's compiled defaultVk matches its Keys entry. NoCrosshair defaults
to 1, TimeScale 0.5 / VK192, WeaponKeysMode ALT, GiveAllWeapons M, NoHud H,
base/DLC wheels B/N, NoWeapon VK189/187; other supplied keys preserved.
Missing INI uses those same built-in defaults. Existing user INI is respected.
MenuKey=112 and UpdaterBeta=0 are new Settings defaults. Menu opening is rebindable
in Hotkeys, saved as MenuKey. Escape cancels, menu binding cannot be unbound or
reuse a command key. Command rebinding rejects the current menu key. Default=F1.

Cursor: capture Unity Cursor lockState/visible on first opening, set None/visible
through game-window action messages, reapply every 100 ms while focused/open,
restore original state on close. No IL2CPP call from Present; early shutdown
fence preserved. Native ClipCursor released during visible rendering. No forced
pause or TimeScale change. Runtime mouse warp, camera and focus behavior pending.

Updater: Updates tab, stable/beta channel persisted, explicit Check, version and
status, confirmed Install after Exit and rollback. Native background WinHTTP
reads this repository's GitHub releases; JSON via nlohmann 3.11.3. Only matching
HTTPS repository asset URLs, bounded ZIP sizes and SHA256 metadata accepted.
Stable may be older than current test build; confirmation explicitly warns.

PowerShell helper embedded as ASI RCDATA, staged in LOCALAPPDATA, launched hidden
only after user's confirmation. Waits for parent game exit; aborts if game running
again. Downloads ZIP, validates SHA256, size, exactly four root names (no traversal
or duplicates) and two AMD64 PE headers. Backups existing ASI/DXGI/README before
replacement, restores backup if copying fails. Never overwrites existing personal
INI or original game assets. Rollback restores only mod files, not personal INI.
Result/error: PostalBorkenMenu-update-result.txt beside game. Backup folder:
.PostalBorkenMenu-backup. Network/permissions/helper installation runtime pending.
No silent automatic download/install/check. PowerShell/TLS/proxy availability
can cause errors without replacing the mod. H99 current build constant tags TEST.

Tests to run: GodMode Change -> different free key -> restart/persistence;
Hotkeys menu-key Change -> free key -> close/reopen/restart; confirm no ID warning.
Open directly during gameplay: free mouse clicks and controls, no pause required,
close restores control. Check alt-tab, scene changes and exit. Verify default
INI and missing-INI fallback (back up personal INI first). Check updater stable/
beta version reporting, cancellation, queue/exit/result and rollback with backups.
Check native normal/DLC wheels and direct cold normal DLC art as regression tests.

Status: EXPERIMENTAL; build and package verification distinct from user validation.
Rollback: exit and reinstall H98B for menu or H97 stable, preserve personal INI.
Next: user's menu/cursor/updater verdict and logs before promoting any release.

Delivery revision H99B: adds checked helper error reporting, path rechecks and
release-tag/URL agreement. Initial H99 superseded. All forty recognized command
keys match supplied INI; helper syntax parsed. No runtime installation performed.


Final delivery H99C includes full nlohmann license alongside existing licenses.
Offline helper tests: genuine four-file AMD64 package accepted; wrong SHA256,
traversal entry, duplicate entry and malformed PE rejected for their expected
reasons. These checks do not establish online download/install or in-game behavior.


Final H99D also keeps dynamic binding-button IDs stable (### identifiers),
protects Default operations from colliding with menu key, and releases native
cursor confinement only while this game is foreground. Isolated online helper
installation and rollback passed with exact original hashes and personal INI
sentinel preserved; actual game exit waiting and user gameplay remain untested.


## H99D delivery verification - 2026-10-14
CONSTRUIT / CONTROLE / EXPERIMENTAL; user gameplay verdict pending.
Source/release 9d151d6dc88b0ffc9443f499fe77dca59348e9cf.
CI SUCCESS https://github.com/DeadneM/PostalBorkenMenu/actions/runs/38640890075
Release https://github.com/DeadneM/PostalBorkenMenu/releases/tag/V2A42-H99D-TEST
Published ZIP downloaded: exactly four root files (DXGI 9216, ASI 990208,
INI 2342, README 8640 bytes), two AMD64 PEs verified. GitHub digest matches:
fd347b4082b2fcdfa259b6acb7157e9917a0c63e35317aac6919d3cea89714b0
Packaged INI equals config defaults; forty recognized compiled keys match supplied
INI. Embedded updater resource 101 (5900 bytes) equals verified script source,
normalizing line endings. No VCRUNTIME/MSVCP redistributable DLL imports.

Helper tests: genuine package accepted; SHA mismatch, traversal, duplicate root
entry and bad PE rejected with expected reason. Isolated real GitHub download,
install backup and rollback succeeded using an already-exited parent simulation;
all original mod-file hashes and personal INI sentinel preserved. An unrelated
live parent executable was rejected. Real game shutdown waiting was not tested.
No live game files were changed by assistant. C++ menu-network check and Unity
cursor behavior remain runtime tests; helper network/install tests are distinct.

Main remains H97 c1e4630; part2 gameplay and loader unchanged. Earlier H99/H99B
packages superseded by H99D; H99C run cancelled. Current updater recognizes
V2A42-H numbered release/test tags; beta selects experimental releases, stable
currently H97 (explicit older-version install confirmation). No auto-install.
Next: GodMode rebind, menu-key persistence, free mouse without pause, close/focus
restore, updater stable/beta reporting, and native wheel regression test.
