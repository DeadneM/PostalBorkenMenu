PostalBorkenMenu V2A42-H2 TARGETED HOOK AUDIT + DXGI LOADER
POSTAL: Brain-Damaged

BASE
Built DIRECTLY from the user-validated V2A42 branch.
V2A42 remains the canonical validated base.

DXGI ASI LOADER
This package now includes dxgi.dll as the standard ASI loader.
The loader:
- forwards the real System32 DXGI exports
- loads PostalBorkenMenu.asi
- does NOT hook Present
- does NOT render anything
- does NOT patch swapchains or vtables
- does NOT touch the menu, mouse, cursor, or gameplay

H2 SCOPE
No menu/UI behavior is changed.
No global Assembly-CSharp scan is performed.
No experimental Hook method is invoked.

When the existing Give All Weapons command is used, H2 only enumerates matching
method/field NAMES on these two already-known classes:
- Hyperstrange.PBD.PlayerComponents.PlayerInventoryComponent
- Hyperstrange.PBD.PlayerComponents.Inventory.WeaponsInventory

Filters are limited to:
Hook / Weapon / Collect / Give / Equip / Init / Cooldown / Inventory

Then the original validated GiveAllWeapons() call runs unchanged, with the
existing Hook runtime snapshot before and after it.

WHY
The previous H1 global scan crashed before GiveAllWeapons was even called.
Historical logs also prove get_Hook() can already be non-null before GiveAllWeapons,
so the missing behavior is likely a separate player/inventory initialization path.

TEST
1. Copy all four package files next to the game executable.
2. Launch normally.
3. Confirm the V2A42 menu and Weapon Catalog behave normally.
4. Enter gameplay.
5. Run Give All Weapons ONCE.
6. Confirm the grappling hook becomes usable as usual.
7. Exit normally.
8. Send PostalBorkenMenu.log.

PACKAGE CONTENTS
dxgi.dll
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

======================================================================
ORIGINAL V2A42 README
======================================================================

PostalBorkenMenu V2A42
POSTAL: Brain-Damaged

BASE
Built from V2A41 while preserving the V2A40 stable-window subclass fix that the user validated.

THIS BUILD
V2A42 reconstructs the desired five-tab overlay and cleans the named weapon lists.

TABS
- Gameplay
- Weapons
- Arsenal
- Special
- Visual

ARSENAL
Only single WeaponId variants are exposed.

Base:
- Shovel
- Pistol
- Shotgun
- Machine Gun
- Rocket Launcher
- Lightning Gun
- Gatling Gun
- Dildo Bow
- Cat Canon

These Sunny Daze / PTSD:
- Um Drill
- Piss Gun
- Meat Shotgun
- Bubble Gum Machine Gun
- Nuclear Syringe

All _Akimbo WeaponIds are intentionally removed from the overlay.

SPECIAL
Kept:
- Hook (Add Only)
- Cutscene Weapon DLC
- No Weapon
- No Weapon DLC

Removed:
- Dong
- Dong Confusion
- Dong Fire
- Dong Ice

The Hook path remains AddWeapon-only and is never passed through EquipWeapon.

SKIP INTRO VIDEOS
A new checkbox appears in the Gameplay tab.

Default:
SkipIntroVideos=1

The value is persisted in PostalBorkenMenu.ini under [Settings].
Changing the checkbox affects the next launch.

Implementation for this test:
- no game files are renamed or deleted
- SAVE_DATA.cfg is not modified
- no EXE/GameAssembly patching
- while V2A40 qualifies the foreground game HWND, after the same HWND has remained valid for 1 second, the ASI sends exactly one synthetic Escape key pulse if Skip Intro Videos is enabled
- the normal V2A40 WndProc subclass is still delayed until 3 seconds of HWND stability
- no repeated key injection occurs

This deliberately targets startup only and does not touch normal in-game cutscenes.

STABILITY PRESERVED
- V2A40: 3-second stable foreground HWND qualification before SetWindowLongPtrW
- V2A8: early WM_CLOSE shutdown fencing
- no permanent IL2CPP crosshair polling
- V2A29 weapon-wheel behavior
- exact-name WeaponId resolver remains targeted
- no broad Assembly-CSharp scans
- no startup Hook recovery

TEST
1. Launch normally with SkipIntroVideos=1.
2. Confirm whether the intro video is skipped.
3. Confirm the game reaches the menu and remains stable.
4. Open F1.
5. Check all five tabs.
6. Confirm Arsenal contains no Akimbo entries.
7. Confirm Special contains no Dong entries.
8. Toggle Skip Intro Videos off and on once to verify checkbox persistence.
9. Test one normal Arsenal weapon and, if owned, one DLC weapon.
10. Exit normally and send PostalBorkenMenu.log.

PACKAGE CONTENTS
PostalBorkenMenu.asi
PostalBorkenMenu.ini
README.txt

PostalBorkenMenu.log is generated at runtime and is intentionally not included.
