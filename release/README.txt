POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H80

STATUS
------
V2A42-H80 is the current user-validated release checkpoint.

VALIDATED
---------
- Stable lazy IL2CPP initialization introduced in H77.
- Native tap/hold weapon-wheel behavior from H74.
- Base and DLC weapon-wheel shortcuts work in both directions.
- Short press swaps to the previous weapon.
- Hold opens the selected weapon wheel.
- Cross-mode weapon selection is correct.
- Cross-mode foreground weapon icons are correct.
- F1 overlay and normal gameplay functions are working.
- Skip Startup Logos remains active through the native H39 path.
- V2A8 shutdown/lifetime protections remain preserved.

KNOWN VISUAL LIMITATION
-----------------------
The only remaining known wheel issue is visual:
the background/silhouette behind icons on the alternative cross-mode weapon wheel can still show the scene-native weapon graphic.

The generic WeaponWheelButton._backgroundImage is not the weapon-specific silhouette.
Do not treat this as a weapon-selection or input bug.

INSTALLATION
------------
Copy all four files next to POSTAL Brain Damaged.exe.

Remove -debug from the game's launch options before using PostalBorkenMenu.

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt

NOTES
-----
- F1 opens/closes PostalBorkenMenu.
- IL2CPP initialization is lazy and begins after the first explicit F1 or bound gameplay hotkey action.
- The mod does not patch POSTAL Brain Damaged.exe, GameAssembly.dll, UnityPlayer.dll, or Unity asset files on disk.
- "Borken" is intentional.
