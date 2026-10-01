POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H33 READ-ONLY LOGO DIAGNOSTIC

BASE
----
H33 starts directly from the validated V2A42-H13 lineage.

PURPOSE
-------
H32 is rejected. H33 does NOT attempt to skip any startup logo.

H33 exists only to identify the exact native RCLogoComponent behavior used by
the game's startup-logo sequence before any further modification is attempted.

READ-ONLY DIAGNOSTIC
--------------------
During the startup-logo window, H33:
- resolves RCLogoComponent exactly,
- logs its namespace and class name,
- logs every method and parameter type,
- logs every field and field type/offset when available,
- observes the live RCLogoComponent on the Unity main thread,
- logs when a live logo component appears/disappears or changes,
- logs the active RCLogo GameObject name.

H33 does NOT:
- call RCLogoComponent.OnPointerClick,
- call any skip/next/transition method,
- redirect level0/level1,
- modify globalgamemanagers,
- call SceneManager.LoadScene,
- scan process memory,
- simulate keyboard or mouse input,
- patch any game file.

The temporary observation hook is removed after the short startup window.

CONFIG
------
[Settings]
SkipStartupLogos=1

For H33 this setting only enables the read-only diagnostic.

TEST
----
1. Restore original game files.
2. Copy the four mod files to the game directory.
3. Launch the game normally and let the startup logos play.
4. After reaching the menu, send PostalBorkenMenu.log.

The important section starts with:
[LOGO H33] ===== RCLogoComponent metadata audit =====
