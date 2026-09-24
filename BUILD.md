# Building PostalBorkenMenu

## Requirements

- Windows x64
- Visual Studio / MSVC x64 Native Tools command prompt
- `cl.exe`, `link.exe`, and `lib.exe`

The source intentionally avoids the CRT and dynamically resolves GDI32 calls.

## Source layout

```text
source/
  PostalBorkenMenu.cpp
  PostalBorkenMenu_part1.inc
  PostalBorkenMenu_part2.inc
  PostalBorkenMenu_part3.inc

build/
  kernel32_ext.def
  user32_ext.def
```

The three `.inc` files are the original V2A8 translation unit split at safe
function boundaries. `PostalBorkenMenu.cpp` includes them in order.

## Generate import libraries

From an **x64 Native Tools Command Prompt**:

```bat
lib /nologo /machine:x64 /def:build\kernel32_ext.def /out:build\kernel32_ext.lib
lib /nologo /machine:x64 /def:build\user32_ext.def /out:build\user32_ext.lib
```

## Compile

```bat
cl /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Fo:build\PostalBorkenMenu.obj source\PostalBorkenMenu.cpp
```

## Link

```bat
link /nologo /DLL /MACHINE:X64 /NODEFAULTLIB /ENTRY:DllMain /SUBSYSTEM:WINDOWS ^
  /OPT:REF /OPT:ICF ^
  /OUT:build\PostalBorkenMenu.asi ^
  build\PostalBorkenMenu.obj ^
  build\kernel32_ext.lib ^
  build\user32_ext.lib
```

If your MSVC version rejects one of the no-exception switches, keep the core
constraints intact: x64, DLL, no default CRT, explicit `DllMain`, and the two
custom import libraries.

## Canonical binary check

The user-validated V2A8 binary used during development has:

```text
SHA-256
0ca3f8d01257b84daca0d55b9ea9fe6cea6c0f6d16ef080a83815ae33ad0706a
```

Compiler/linker-version differences can change the binary hash even when the
source is functionally identical. The hash above identifies the exact tested
artifact, not a promise that every rebuild will be byte-identical.

## Runtime requirement

PostalBorkenMenu is an ASI. A compatible x64 ASI loader is required separately.

**Remove `-debug` from POSTAL: Brain-Damaged launch options before testing.**
