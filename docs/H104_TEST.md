# H104 TEST - Unbound command defaults and early quit fence

Base: H103 scope-only release + user-validated H102 timer + accepted H101 functions.
Scope-only H103 lacks an explicit user visual verdict. User reports quit crash on
2026-10-25 and approves personal INI settings as future defaults, removing shortcuts.
Goal: zero command shortcuts by default, matching ASI/INI defaults; stop mod work
before engine teardown on Quit. F1 remains menu access, not a gameplay command binding.
Success: fresh INI and missing-key fallback agree; timer works, click Quit closes
normally repeatedly without crash. No claim of runtime success from offline tests.

## Defaults
Settings: MenuKey=112 (F1), UpdaterBeta=1, NoCrosshair=1, SkipStartupLogos=1,
LevelTimer=1. Visual: NoHud=0, NoHudWithCrossHair=0, NoModel=0, NoMenu=0,
WeaponPostProcessing=0. All Keys=0 (all command defaultVk=0). TimeScale argument=0.5,
WeaponKeysMode argument=OFF to disable automatic mod remapping of number keys 1-9;
other arguments empty. ASI missing-key fallbacks match.
Existing personal INI and bindings remain unchanged by the updater/install policy;
these defaults affect fresh configs/missing entries/Default key reset. No default
GodMode/Noclip activation. F3 remains available after native F3 hook; no default action.

## Quit evidence
Latest Unity Player.log and Windows Application events show 0xc0000005 on exit
(2026-10-25 18:17:22), ntdll.dll+0x1a12d. Unity stack includes ASI+0x91e5,
USER32.CallWindowProcW and UnityPlayer. Disassembly of the actual H103 package maps
0x91e5 to the return site of the il2cpp_thread_attach function pointer in the
WM_APP_LEVEL_TIMER handler (0x803e); subsequent handler is weapon post-processing
(0x803d). No mod shutdown marker precedes this. This demonstrates thread-attach
while processing a timer message at crash; teardown timing is the repair hypothesis,
not proof that every possible exit crash shares the same cause.
Evidence copied to ignored local out/quit-evidence-20261025 (not committed/uploaded).

## Repair
Metadata-resolved GameManager.ExitGame and UnityEngine.Application.Internal_ApplicationQuit
get forwarding hooks. Before forwarding, call the existing early shutdown fence,
restore original WndProc, hide menu, stop timer/wheel/input tasks and worker. GameManager
static/instance ABI selected via method_is_instance; original called exactly once.
Unity quitting event provides fallback for other quit paths; WM_CLOSE/destruction
fallbacks retained. Hook mask logged (1 game manager, 2 Unity; expected 3).
EnsureIl2CppThread refuses entry once stopping. Present bypasses overlay without backend
cleanup during teardown; Resize also bypasses overlay. Process exit reclaims resources.
No forced process termination, no changes to the game's exit/Steam cleanup behavior.
No game file patches or game launch. Installation unchanged during this development.

## Checks / test / rollback
Tests: defaults consistency, isolated quit hook ordering/forwarding/ABI (both static
and instance), prior F3 hook fixture and timer-format boundaries. Runtime quit still
EXPERIMENTAL until user validates; no game launched by agent.
Test with current personal config first: open menu, play with timer, return title,
click Quit; verify [QUIT H104] marker and normal exit several times. Also test Quit
from startup/title and WM_CLOSE; opening menu only should preserve cursor behavior.
Fresh default test: back up personal INI, temporarily use packaged defaults, confirm
all command bindings Unbound and F1 menu; timer/post preference restore after 10s.
Rollback: H103 files and backed-up personal INI. Existing INI is not silently rewritten.
TODO: user quit verdict and H103 scope-only verdict; only then promote quit fix.

## Verified delivery and install
Source/tag c63ccb5e07ddfdad93155e17365a69d3d5abf37c. CI 40216240856 successful:
ASI build, quit forwarding tests for both static and instance ABI, defaults consistency,
F3 fixture, timer-format tests, package and prerelease.
Downloaded ZIP digest matched: ad2e25536a278cd99ee98a3a08588980dfc51e6f30e119f6b4d9d7d6696d6b49.
Four root files / AMD64 verified: dxgi 9216; ASI 1006592; INI 2623; README 25208 bytes.
Installed H104 TEST with game closed, all four installed file hashes verified.
Backup: game/.PostalBorkenMenu-backups/H104-install-20261025-182607-763.
Personal INI preserved except explicitly requested Keys all 0 and WeaponKeysMode=OFF;
MenuKey=112 and existing Settings/Visual preferences retained. Original INI backed up.
No game launched. Quit correction remains EXPERIMENTAL; user must confirm normal exit.
Main/release H103 not replaced by this test branch/tag. Timer and scope paths unchanged.


## H104 release acceptance - 2026-10-25

User reports installed H104 TEST appears okay and explicitly requests GitHub release.
Promote the tested behavior unchanged; only version labels/delivery docs change.
See docs/H104_RELEASE.md. Test history remains separate and cumulative.
