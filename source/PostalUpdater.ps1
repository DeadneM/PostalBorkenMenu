param([Parameter(Mandatory=$true)][string]$GameDir,[int]$ParentPid=0,
    [ValidateSet('Install','Restore','Verify')][string]$Mode='Install',
    [string]$Tag,[string]$Url,[string]$Digest,[long]$ExpectedSize=0,[string]$LocalZip)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
$root=[IO.Path]::GetFullPath($GameDir).TrimEnd('\')
$allowed=@('PostalBorkenMenu.asi','dxgi.dll','PostalBorkenMenu.ini','README.txt')
$replace=@('PostalBorkenMenu.asi','dxgi.dll','README.txt')
$backup=Join-Path $root '.PostalBorkenMenu-backup'
$rootSafe=$false
function NoLink([string]$path){if((Test-Path -LiteralPath $path) -and ((Get-Item -LiteralPath $path).Attributes -band [IO.FileAttributes]::ReparsePoint)){throw 'Refusing reparse-point target'}}
function ValidateZip([string]$zipPath,[string]$unpack){
    if($Digest -notmatch '^[a-fA-F0-9]{64}$' -or (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash -ne $Digest){throw 'SHA256 mismatch'}
    if($ExpectedSize -gt 0 -and (Get-Item -LiteralPath $zipPath).Length -ne $ExpectedSize){throw 'ZIP length mismatch'}
    $zip=[IO.Compression.ZipFile]::OpenRead($zipPath)
    try{
        if($zip.Entries.Count -ne 4){throw 'Expected four ZIP entries'}
        $seen=@{}
        foreach($entry in $zip.Entries){
            if($allowed -cnotcontains $entry.FullName -or $seen.ContainsKey($entry.FullName) -or $entry.Length -gt 32MB){throw 'Invalid ZIP root/size/duplicate'}
            $seen[$entry.FullName]=$true
            $target=Join-Path $unpack $entry.FullName
            $output=[IO.File]::Create($target);$input=$entry.Open()
            try{$input.CopyTo($output)}finally{$input.Dispose();$output.Dispose()}
            if($entry.FullName -match '\.(asi|dll)$'){
                $bytes=[IO.File]::ReadAllBytes($target)
                if($bytes.Length -lt 128 -or $bytes[0] -ne 77 -or $bytes[1] -ne 90){throw 'Invalid PE'}
                $pe=[BitConverter]::ToInt32($bytes,60)
                if($pe -lt 0 -or $pe+26 -gt $bytes.Length -or [BitConverter]::ToUInt32($bytes,$pe) -ne 17744 -or [BitConverter]::ToUInt16($bytes,$pe+4) -ne 34404){throw 'Expected AMD64 PE'}
            }
        }
    }finally{$zip.Dispose()}
}
$stage=Join-Path ([IO.Path]::GetTempPath()) ('PostalUpdate-'+[guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($stage)|Out-Null
try{
    NoLink $root;NoLink $backup
    foreach($name in $allowed){NoLink (Join-Path $root $name);NoLink (Join-Path $backup $name)}
    $rootSafe=$true
    if($Mode -eq 'Verify'){ValidateZip $LocalZip $stage;'PACKAGE VERIFIED';exit 0}
    if(!(Test-Path -LiteralPath (Join-Path $root 'POSTAL Brain Damaged.exe'))){throw 'Game EXE not found'}
    if($ParentPid -le 0){throw 'Parent process required'}
    $parent=Get-Process -Id $ParentPid -ErrorAction SilentlyContinue
    if($parent){if([IO.Path]::GetFullPath($parent.Path) -ne (Join-Path $root 'POSTAL Brain Damaged.exe')){throw 'Unexpected parent executable'};$parent.WaitForExit()}
    if(Get-Process -ErrorAction SilentlyContinue | Where-Object {$_.Path -eq (Join-Path $root 'POSTAL Brain Damaged.exe')}){throw 'Game running again; update aborted'}
    NoLink $root;NoLink $backup
    foreach($name in $allowed){NoLink (Join-Path $root $name);NoLink (Join-Path $backup $name)}
    if($Mode -eq 'Restore'){
        foreach($name in $replace){if(!(Test-Path -LiteralPath (Join-Path $backup $name))){throw 'Complete rollback backup unavailable'}}
        foreach($name in $replace){Copy-Item -LiteralPath (Join-Path $backup $name) -Destination (Join-Path $root $name) -Force}
        'Rollback completed; personal INI preserved.'|Set-Content -LiteralPath (Join-Path $root 'PostalBorkenMenu-update-result.txt');exit 0
    }
    if($Tag -notmatch '^V2A42-H[0-9]+[A-Z]?(-TEST)?$' -or $Url -notmatch '^https://github\.com/DeadneM/PostalBorkenMenu/releases/download/[^/]+/POSTAL_Brain_Damaged_PostalBorkenMenu_V2A42_H[A-Za-z0-9_.-]+\.zip$' -or $ExpectedSize -lt 1000 -or $ExpectedSize -gt 32MB){throw 'Invalid release metadata'}
    if(!$Url.StartsWith('https://github.com/DeadneM/PostalBorkenMenu/releases/download/'+$Tag+'/')){throw 'Release tag/URL mismatch'}
    [Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12
    $zipPath=Join-Path $stage 'package.zip';Invoke-WebRequest -UseBasicParsing -Uri $Url -OutFile $zipPath
    ValidateZip $zipPath $stage
    # Back up before replacing; never replace a personal INI.
    foreach($name in $replace){if(!(Test-Path -LiteralPath (Join-Path $root $name))){throw 'Existing mod file missing; install manually'}}
    [IO.Directory]::CreateDirectory($backup)|Out-Null
    foreach($name in $replace){Copy-Item -LiteralPath (Join-Path $root $name) -Destination (Join-Path $backup $name) -Force}
    try{foreach($name in $replace){Copy-Item -LiteralPath (Join-Path $stage $name) -Destination (Join-Path $root $name) -Force}}
    catch{foreach($name in $replace){Copy-Item -LiteralPath (Join-Path $backup $name) -Destination (Join-Path $root $name) -Force};throw}
    if(!(Test-Path -LiteralPath (Join-Path $root 'PostalBorkenMenu.ini'))){Copy-Item -LiteralPath (Join-Path $stage 'PostalBorkenMenu.ini') -Destination (Join-Path $root 'PostalBorkenMenu.ini')}
    "Installed $Tag; personal INI preserved."|Set-Content -LiteralPath (Join-Path $root 'PostalBorkenMenu-update-result.txt')
}catch{
    if($Mode -ne 'Verify' -and $rootSafe -and (Test-Path -LiteralPath $root)){"Updater failed: $($_.Exception.Message)"|Set-Content -LiteralPath (Join-Path $root 'PostalBorkenMenu-update-result.txt')}
    throw
}finally{
    $tempRoot=[IO.Path]::GetFullPath([IO.Path]::GetTempPath()).TrimEnd('\')+'\'
    if([IO.Path]::GetFullPath($stage).StartsWith($tempRoot) -and (Split-Path $stage -Leaf) -like 'PostalUpdate-*'){Remove-Item -LiteralPath $stage -Recurse -Force}
}
