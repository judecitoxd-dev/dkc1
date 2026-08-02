param(
    [ValidateSet("Release", "Debug")]
    [string]$Configuration = "Release",

    [ValidateSet("x64")]
    [string]$Architecture = "x64",

    [string]$RomPath = "",

    [string]$BuildDirectory = "build-windows",

    [string]$OutputZip = "DK1-Jungle-Hijinxs-Preview-Windows-x64.zip"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$buildPath = Join-Path $repositoryRoot $BuildDirectory
$packagePath = Join-Path $buildPath "package"
$outputPath = if ([System.IO.Path]::IsPathRooted($OutputZip)) {
    $OutputZip
} else {
    Join-Path $repositoryRoot $OutputZip
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    throw "Visual Studio Installer/vswhere.exe was not found. Install Visual Studio 2022 Build Tools with Desktop development with C++."
}

$installationPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ([string]::IsNullOrWhiteSpace($installationPath)) {
    throw "No Visual Studio installation with the MSVC x64 C++ tools was found."
}

$developerCommand = Join-Path $installationPath "Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path $developerCommand)) {
    throw "VsDevCmd.bat was not found at: $developerCommand"
}

New-Item -ItemType Directory -Force -Path $buildPath | Out-Null

$configureAndBuild = @(
    "call `"$developerCommand`" -no_logo -arch=$Architecture -host_arch=x64",
    "cmake -S `"$repositoryRoot`" -B `"$buildPath`" -A x64",
    "cmake --build `"$buildPath`" --config $Configuration --target dk1_win32 dk1_level_test_win32 --parallel"
) -join " && "

Write-Host "Configuring and building the Windows preview..."
& $env:COMSPEC /d /s /c $configureAndBuild
if ($LASTEXITCODE -ne 0) {
    throw "The MSVC build failed with exit code $LASTEXITCODE."
}

Remove-Item -Recurse -Force $packagePath -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $packagePath | Out-Null

$configurationPath = Join-Path $buildPath $Configuration
$previewSource = Join-Path $configurationPath "dk1_win32.exe"
$directSource = Join-Path $configurationPath "dk1_level_test_win32.exe"
if (-not (Test-Path $previewSource) -or -not (Test-Path $directSource)) {
    throw "The expected Windows executables were not produced."
}

$previewTarget = Join-Path $packagePath "DK1-Jungle-Hijinxs-Preview.exe"
$directTarget = Join-Path $packagePath "DK1-Jungle-Hijinxs-Direct-Test.exe"
Copy-Item $previewSource $previewTarget
Copy-Item $directSource $directTarget

$readme = @'
DK1 Jungle Hijinxs provisional Windows test package

FASTEST TEST
1. Run DK1-Jungle-Hijinxs-Direct-Test.exe.
2. Select your own unheadered Donkey Kong Country (USA) Rev 2 .sfc ROM.
3. The program validates the ROM and performs a startup preflight for textures,
   palettes, terrain, object streaming, dynamic BG1 and player graphics.
4. The window title reports route progress and four checkpoints.
5. Reaching 100 percent writes DK1-Jungle-Hijinxs-Test-Report.txt.

PREFLIGHT ONLY
Run from PowerShell or Command Prompt:
  DK1-Jungle-Hijinxs-Direct-Test.exe --preflight "C:\path\game.sfc"

This loads and certifies the first-level startup systems, writes
DK1-Jungle-Hijinxs-Preflight.txt and exits without opening the playable window.

CONTROLS
- Left/Right or A/D: move
- Z: jump
- U: pick up / throw barrel
- R: restart
- Enter: restart after completion
- F2: rescue warp forward by 10 percent for incomplete collision testing
- Esc: exit

FULL PREVIEW FLOW
Run DK1-Jungle-Hijinxs-Preview.exe for the provisional intro, title, menu,
Kongo Jungle map, Jungle Hijinxs and return-to-map flow.

No ROM or copyrighted extracted assets are included. Both programs read graphics
and palettes from the user's legal ROM or its validated local cache. This is a
first-level vertical slice, not a complete game build.
'@
Set-Content -Path (Join-Path $packagePath "README.txt") -Value $readme -Encoding UTF8

if (-not [string]::IsNullOrWhiteSpace($RomPath)) {
    $resolvedRom = (Resolve-Path $RomPath).Path
    $quotedRom = '"' + $resolvedRom + '"'
    Write-Host "Running the first-level startup preflight..."
    $preflightProcess = Start-Process -FilePath $directTarget -ArgumentList @("--preflight", $quotedRom) -WorkingDirectory $repositoryRoot -Wait -PassThru
    if ($preflightProcess.ExitCode -ne 0) {
        throw "The startup preflight failed with exit code $($preflightProcess.ExitCode)."
    }
    $preflightReport = Join-Path $repositoryRoot "DK1-Jungle-Hijinxs-Preflight.txt"
    if (-not (Test-Path $preflightReport)) {
        throw "The executable exited successfully but did not write the startup preflight report."
    }
    Copy-Item $preflightReport $packagePath
}

Remove-Item $outputPath -Force -ErrorAction SilentlyContinue
Compress-Archive -Path (Join-Path $packagePath "*") -DestinationPath $outputPath -CompressionLevel Optimal

$hash = Get-FileHash -Algorithm SHA256 $outputPath
Write-Host ""
Write-Host "Windows preview package created:"
Write-Host "  $outputPath"
Write-Host "SHA-256: $($hash.Hash.ToLowerInvariant())"
