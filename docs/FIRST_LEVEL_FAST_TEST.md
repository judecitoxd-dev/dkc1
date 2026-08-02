# Fast Jungle Hijinxs test scope

The next user-facing delivery is intentionally smaller than a faithful full-game
preview. Its purpose is to get a Windows build into the user's hands quickly and
collect real play feedback.

Current implementation estimate: **97 percent complete, 3 percent remaining**.
This percentage measures only the fast Windows first-level tester described
below, not the complete game or a fully faithful Jungle Hijinxs conversion.

Implemented for the fast test:

- Validate the legal USA Rev 2 ROM.
- Preload scene textures, palettes and first visible sprites.
- Start Jungle Hijinxs directly from a dedicated executable.
- Render the camera-driven level background.
- Support walking, jumping, terrain collision and camera movement.
- Stream the currently translated Gnawty and Barrel actors.
- Track monotonic route progress from 0 to 100 percent and four checkpoints.
- Display a clear provisional completion result at the end of the route.
- Restart manually or automatically after falling outside the test envelope.
- Provide an F2 rescue warp so an incomplete collision cannot block later-route testing.
- Write a diagnostic report containing progress, cache, graphics and restart data.
- Certify startup textures, palettes, packages, terrain, objects, dynamic BG1,
  player DMA, view and route bounds before opening the playable window.
- Support interactive `--preflight` and unattended `--preflight-quiet` modes.
- Write a failure-stage report and meaningful process exit code when quiet
  certification cannot initialize or validate the level.
- Use one shared SPC RAM definition and compile both audio-driver headers
  together to prevent the former macro-redefinition warning.
- Build a portable headless route validator that loads Jungle Hijinxs, searches
  for valid terrain near six points from 0 to 95 percent, updates camera,
  dynamic BG1 and object streaming, renders a real frame at every point and
  verifies provisional completion.
- Record per-checkpoint world position, camera, active object counts, nonzero
  pixel count, BG1 generation/columns and frame signatures in a route report.
- Provide a local Visual Studio builder that compiles the two Windows frontends
  plus the headless validator, builds ROM-backed regressions for compile
  coverage, runs key ROM-independent smoke tests, records the build log,
  packages the result and calculates its SHA-256 without depending on GitHub
  Actions.
- Refuse to package a ROM-certified build unless both reports contain
  `ready=1`, the route report contains six checkpoints and provisional
  completion is confirmed.
- Allow a legal ROM to be dragged onto `BUILD-WINDOWS-PREVIEW.bat` to build,
  certify and package in one operation.
- Package the direct tester, normal shell preview and route validator together.

The isolated startup-preflight regression compiles and passes with strict C11
warnings treated as errors. The new route validator is connected to strict
compiler flags and the Windows builder, but still requires the pending full
MSVC/legal-ROM execution before it can be called certified.

Local Windows build:

```powershell
./tools/build_windows_preview.ps1
```

Build, certify the startup and six route points with a legal ROM, then package:

```powershell
./tools/build_windows_preview.ps1 -RomPath "C:\path\game.sfc"
```

The simplest certified route is to drag the legal `.sfc` file onto:

```text
BUILD-WINDOWS-PREVIEW.bat
```

Remaining before delivery:

- Complete one real full-project MSVC Windows build.
- Run startup preflight and the six-point headless route validator with a
  supported legal ROM, confirming `ready=1` and `route_completed=1`.
- Fix any compile or first-run defect found by that final execution.
- Publish the resulting Windows ZIP artifact.

GitHub-hosted Linux and Windows jobs remain unavailable at runner startup: the
latest validation attempt terminated before checkout and exposed no steps or
logs. The local certified builder is now the primary delivery route rather than
repeatedly retrying an unavailable runner.

Deferred until after this fast test:

- Original intro, title, menu and world-map presentation.
- Full music and sound-effect fidelity.
- Every collectible, enemy and scenery callback.
- Original checkpoint and exit callbacks.
- Complete DK/Diddy partner, loss and swap behavior.

The deferred systems remain necessary for a faithful Jungle Hijinxs conversion,
but they no longer block the earliest playable feedback build.
