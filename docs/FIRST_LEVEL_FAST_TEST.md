# Fast Jungle Hijinxs test scope

The next user-facing delivery is intentionally smaller than a faithful full-game
preview. Its purpose is to get a Windows build into the user's hands quickly and
collect real play feedback.

Current implementation estimate: **95 percent complete, 5 percent remaining**.
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
- Provide a local Visual Studio builder that compiles both Windows executables,
  builds ROM-backed regressions for compile coverage, runs the key
  ROM-independent smoke tests, records the build log, packages the result and
  calculates its SHA-256 without depending on GitHub Actions.
- Allow a legal ROM to be dragged onto `BUILD-WINDOWS-PREVIEW.bat` to build,
  certify and package in one operation.
- Package the direct tester and normal shell preview together.

The isolated startup-preflight regression compiles and passes with strict C11
warnings treated as errors. This validates the new certification logic, but is
not a substitute for the pending full MSVC build and legal-ROM launch.

Local Windows build:

```powershell
./tools/build_windows_preview.ps1
```

Build, certify with a legal ROM and package:

```powershell
./tools/build_windows_preview.ps1 -RomPath "C:\path\game.sfc"
```

The simplest certified route is to drag the legal `.sfc` file onto:

```text
BUILD-WINDOWS-PREVIEW.bat
```

Remaining before delivery:

- Complete one real full-project MSVC Windows build.
- Run the newly built direct executable with a supported legal ROM and confirm
  that the generated preflight report contains `ready=1`.
- Fix any compile or first-run defect found by that final execution.
- Publish the resulting Windows ZIP artifact.

GitHub-hosted Linux and Windows jobs remain unavailable at runner startup: the
latest validation attempt again terminated before checkout and exposed no steps
or logs. The local certified builder is now the primary delivery route rather
than repeatedly retrying an unavailable runner.

Deferred until after this fast test:

- Original intro, title, menu and world-map presentation.
- Full music and sound-effect fidelity.
- Every collectible, enemy and scenery callback.
- Original checkpoint and exit callbacks.
- Complete DK/Diddy partner, loss and swap behavior.

The deferred systems remain necessary for a faithful Jungle Hijinxs conversion,
but they no longer block the earliest playable feedback build.
