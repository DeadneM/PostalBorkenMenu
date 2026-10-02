POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H39 REAL NATIVE CANCELSPLASH TEST

BASE
----
H39 starts directly from validated V2A42-H13.

H38 RESULT
----------
H38 reached the real native Unity Splash state:
- exact UnityPlayer verification = 1
- native state observed = 1 (Begin)
- direct state stop = 1

But the visible startup logos remained.

Why:
H38 emulated only the final fields:
  state = 3
  active = 0

Exact UnityPlayer disassembly shows that the real CancelSplashScreen does more.

REAL NATIVE CANCELSPLASHSCREEN
------------------------------
Exact retail UnityPlayer.dll:

SHA-256:
27b88589c217589675c976bd303984286bb4b71516ab68efac1c178401a31e94

Unity:
2021.3.14f1

Native CancelSplashScreen:
RVA 0x003C2230

Its code first calls:
RVA 0x003CA820

with state = 3.

That helper:
- writes the native splash state,
- dispatches registered Unity state-change callbacks,
- processes/removes callback entries,
- then returns to CancelSplashScreen.

Only after that does CancelSplashScreen write:
  active byte +0x70 = 0

This callback dispatch was missing from H38.

H39 METHOD
----------
H39 does not emulate CancelSplashScreen.

It watches only the exact known native state object:
  UnityPlayer + 0x01A36340

When the state is:
  1 = Begin
or
  2 = Fade

H39 calls the real Unity native function:
  UnityPlayer + 0x003C2230
  CancelSplashScreen()

After the call H39 verifies:
  state == 3
  active == 0

It continues watching briefly so a later native Begin can be caught and cancelled
again if Unity restarts the splash state.

No memory scan.
No UnityPlayer code patch.
No file modification.
No scene redirect.
No IL2CPP call is used for the splash fix.

EXPECTED LOG
------------
[SPLASH H39] UnityPlayer exact-build verification: 1
[SPLASH H39] Real CancelSplashScreen calls: N
[SPLASH H39] Confirmed state=3/active=0 after cancel: N
[SPLASH H39] Re-Begin events seen after cancel: N
[SPLASH H39] Last observed native splash state: N

If Real CancelSplashScreen calls > 0, H39 executed Unity's own complete native
cancel path including its internal callback dispatch.

CONFIG
------
[Settings]
SkipStartupLogos=1

Set to 0 to disable H39.
