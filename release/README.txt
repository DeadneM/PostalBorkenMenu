POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H34 STARTUP METADATA AUDIT

BASE
----
H34 starts directly from validated V2A42-H13.

WHY
---
H33 proved RCLogoComponent is not the controller for the three startup logos:
it only exposes pointer events plus a cursor sprite field, and no live instance
was observed during the startup-logo sequence.

H34 performs a metadata-only audit to identify the real startup controller.

READ-ONLY
---------
H34 does not:
- skip any logo,
- call RCLogoComponent,
- redirect scenes,
- touch level0 or level1,
- patch globalgamemanagers,
- scan process RAM,
- hook Unity file I/O,
- simulate input,
- modify any game file.

WHAT IT LOGS
------------
H34 enumerates Assembly-CSharp metadata and records:
- every class in Hyperstrange.PBD.Ready,
- plus classes whose names contain terms such as Logo, Splash, Intro, Boot,
  Startup, Publisher, Platform, Ready, Cinematic, Video, or Movie,
- each candidate class's methods,
- each candidate class's fields.

TEST
----
Launch normally and let the logos play to the menu, then send
PostalBorkenMenu.log.

Important section:
[LOGO H34] ===== STARTUP / READY METADATA AUDIT =====
