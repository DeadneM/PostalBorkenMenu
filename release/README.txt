POSTAL: Brain-Damaged - PostalBorkenMenu V2A42-H55 WHEEL SPRITE AUDIT

BASE
----
H55 starts from H54.

H54 RESULT
----------
The normal-campaign DLC wheel path reaches the correct logical state:
- exact DLC WeaponIds found: 5
- DLC targets collected/ready: 5
- BASE->DLC slots swapped after native open: 5

But the five DLC weapons still do not render correctly in the wheel.

H54 also revealed WeaponWheelButton fields:
- _weaponId
- _backgroundImage
- _weaponSelectedImage
- _spiralTransform
- _notSelectedColor
- _selectedColor
- _ammoCountText

and methods:
- Awake
- OnEnable
- OnDisable
- CheckAmmoState
- SetAmmoState
- Enable
- Select
- Deselect
- OnUpdate
- UpdatePosition

There is no obvious RefreshWeaponIcon method.

H55 PURPOSE
-----------
H55 does NOT change wheel behavior.

After a successful five-slot DLC swap in normal campaign, H55 logs:

For each mapped button:
- mapped index
- current WeaponId
- _weaponSelectedImage runtime type
- current _weaponSelectedImage sprite name
- _backgroundImage runtime type
- current _backgroundImage sprite name

For each DLC WeaponId:
- visual-related method names containing Icon / Sprite / Image / Visual / Texture
- every WeaponId field name
- each field's declared type
- for Sprite / Texture / Image / GameObject-like fields, the referenced Unity object name

This determines whether:
1. the real DLC icon already exists inside WeaponId and only needs to be assigned to the button Image, or
2. the true DLC wheel uses a different prefab/button configuration not derivable from WeaponId alone.

SAFETY
------
- H51 Base Weapon Wheel block remains untouched.
- H54 behavior is unchanged.
- No global metadata scan.
- Audit runs once per launch after a successful 5-slot normal-campaign DLC swap.

TEST
----
Normal campaign only:

1. Open DLC Weapon Wheel once.
2. Wait a second.
3. Release.
4. Close the game.
5. Send PostalBorkenMenu.log.

Important lines:
[WHEEL H55] ===== mapped button visual audit begin =====
[WHEEL H55] current WeaponId:
[WHEEL H55] sprite name:
[WHEEL H55 WEAPON FIELD]
[WHEEL H55] visual field object name:

PACKAGE
-------
Exactly:
- dxgi.dll
- PostalBorkenMenu.asi
- PostalBorkenMenu.ini
- README.txt
