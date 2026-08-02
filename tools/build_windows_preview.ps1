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
$buildLog = Join-Path $buildPath "DK1-Windows-Build.log"
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
Remove-Item $buildLog -Force -ErrorAction SilentlyContinue

$targets = @(
    "dk1_win32",
    "dk1_level_test_win32",
    "dk1_first_level_validate",
    "test_first_level_route",
    "test_dynamic_bg1_runtime",
    "test_software_frontend_dispose",
    "test_apu_driver_catalog",
    "test_preview_asset_warmup",
    "test_level_dynamic_bg1"
) -join " "
$smokePattern = "^(first_level_route|dynamic_bg1_runtime|software_frontend_dispose)$"
$configureBuildAndTest = @(
    "call `"$developerCommand`" -no_logo -arch=$Architecture -host_arch=x64",
    "cmake -S `"$repositoryRoot`" -B `"$buildPath`" -A x64",
    "cmake --build `"$buildPath`" --config $Configuration --target $targets --parallel",
    "ctest --test-dir `"$buildPath`" -C $Configuration --output-on-failure -R `"$smokePattern`""
) -join " && "

Write-Host "Configuring, building and smoke-testing the Windows preview..."
$buildOutput = & $env:COMSPEC /d /s /c $configureBuildAndTest 2>&1
$buildExitCode = $LASTEXITCODE
$buildOutput | Tee-Object -FilePath $buildLog
if ($buildExitCode -ne 0) {
    throw "The MSVC build or smoke test failed with exit code $buildExitCode. See $buildLog"
}

Remove-Item -Recurse -Force $packagePath -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $packagePath | Out-Null

$configurationPath = Join-Path $buildPath $Configuration
$previewSource = Join-Path $configurationPath "dk1_win32.exe"
$directSource = Join-Path $configurationPath "dk1_level_test_win32.exe"
$routeSource = Join-Path $configurationPath "dk1_first_level_validate.exe"
if (-not (Test-Path $previewSource) -or
    -not (Test-Path $directSource) -or
    -not (Test-Path $routeSource)) {
    throw "The expected Windows executables were not produced."
}

$previewTarget = Join-Path $packagePath "DK1-Jungle-Hijinxs-Preview.exe"
$directTarget = Join-Path $packagePath "DK1-Jungle-Hijinxs-Direct-Test.exe"
$routeTarget = Join-Path $packagePath "DK1-Jungle-Hijinxs-Route-Validate.exe"
Copy-Item $previewSource $previewTarget
Copy-Item $directSource $directTarget
Copy-Item $routeSource $routeTarget
Copy-Item $buildLog $packagePath

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
Interactive:
  DK1-Jungle-Hijinxs-Direct-Test.exe --preflight "C:\path\game.sfc"

Quiet automation:
  DK1-Jungle-Hijinxs-Direct-Test.exe --preflight-quiet "C:\path\game.sfc"

HEADLESS ROUTE VALIDATION
  DK1-Jungle-Hijinxs-Route-Validate.exe "C:\path\game.sfc"

The route validator loads the same first-level runtime, searches for valid terrain
near six points from 0 to 95 percent, updates camera/object/background streaming,
renders a real frame at every point and verifies provisional completion. It writes
DK1-Jungle-Hijinxs-Route-Validation.txt and returns a failing process exit code if
any point cannot be placed, stepped or rendered.

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

No ROM or copyrighted extracted assets are included. The programs read graphics
and palettes from the user's legal ROM or its validated local cache. This is a
first-level vertical slice, not a complete game build. The package includes the
MSVC build log and ROM-independent smoke-test results. A package built by dragging
a legal ROM onto BUILD-WINDOWS-PREVIEW.bat additionally contains successful
startup and headless route reports.
'@
Set-Content -Path (Join-Path $packagePath "README.txt") -Value $readme -Encoding UTF8

$romCertified = $false
$routeCertified = $false
if (-not [string]::IsNullOrWhiteSpace($RomPath)) {
    $resolvedRom = (Resolve-Path $RomPath).Path
    $quotedRom = '"' + $resolvedRom + '"'
    $preflightReport = Join-Path $repositoryRoot "DK1-Jungle-Hijinxs-Preflight.txt"
    $routeReport = Join-Path $repositoryRoot "DK1-Jungle-Hijinxs-Route-Validation.txt"
    Remove-Item $preflightReport -Force -ErrorAction SilentlyContinue
    Remove-Item $routeReport -Force -ErrorAction SilentlyContinue

    Write-Host "Running the first-level startup preflight..."
    $preflightProcess = Start-Process -FilePath $directTarget -ArgumentList @("--preflight-quiet", $quotedRom) -WorkingDirectory $repositoryRoot -Wait -PassThru
    if ($preflightProcess.ExitCode -ne 0) {
        throw "The startup preflight failed with exit code $($preflightProcess.ExitCode). See $preflightReport"
    }
    if (-not (Test-Path $preflightReport)) {
        throw "The executable exited successfully but did not write the startup preflight report."
    }
    $preflightText = Get-Content -Raw $preflightReport
    if ($preflightText -notmatch "(?m)^ready=1\s*$") {
        throw "The startup preflight report did not certify ready=1."
    }
    $romCertified = $true
    Copy-Item $preflightReport $packagePath

    Write-Host "Running the headless first-level route validation..."
    Push-Location $repositoryRoot
    try {
        & $routeTarget $resolvedRom $routeReport
        $routeExitCode = $LASTEXITCODE
    } finally {
        Pop-Location
    }
    if ($routeExitCode -ne 0) {
        throw "The headless route validation failed with exit code $routeExitCode. See $routeReport"
    }
    if (-not (Test-Path $routeReport)) {
        throw "The route validator exited successfully but did not write its report."
    }
    $routeText = Get-Content -Raw $routeReport
    if ($routeText -notmatch "(?m)^ready=1\s*$" -or
        $routeText -notmatch "(?m)^route_completed=1\s*$" -or
        $routeText -notmatch "(?m)^checkpoint_count=6\s*$") {
        throw "The headless route report did not certify all six checkpoints and completion."
    }
    $routeCertified = $true
    Copy-Item $routeReport $packagePath
}

Remove-Item $outputPath -Force -ErrorAction SilentlyContinue
Compress-Archive -Path (Join-Path $packagePath "*") -DestinationPath $outputPath -CompressionLevel Optimal

$hash = Get-FileHash -Algorithm SHA256 $outputPath
$manifest = @(
    "configuration=$Configuration",
    "architecture=$Architecture",
    "smoke_tests=first_level_route,dynamic_bg1_runtime,software_frontend_dispose",
    "rom_preflight=$romCertified",
    "headless_route=$routeCertified",
    "zip_sha256=$($hash.Hash.ToLowerInvariant())"
)
Set-Content -Path (Join-Path $buildPath "DK1-Windows-Package.txt") -Value $manifest -Encoding UTF8

Write-Host ""
Write-Host "Windows preview package created:"
Write-Host "  $outputPath"
Write-Host "Startup preflight certified: $romCertified"
Write-Host "Headless route certified: $routeCertified"
Write-Host "SHA-256: $($hash.Hash.ToLowerInvariant())"
