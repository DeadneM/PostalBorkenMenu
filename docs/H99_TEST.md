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

