# Fast Jungle Hijinxs test scope

The next user-facing delivery is intentionally smaller than a faithful full-game
preview. Its purpose is to get a Windows build into the user's hands quickly and
collect real play feedback.

Current implementation estimate: **92 percent complete, 8 percent remaining**.
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
- Support `--preflight` to write a startup report and exit without playing.
- Provide a local Visual Studio builder that compiles, packages and hashes both
  Windows executables without depending on GitHub Actions.
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

`BUILD-WINDOWS-PREVIEW.bat` is a one-click wrapper for the same builder.

Remaining before delivery:

- Complete one real full-project MSVC Windows build.
- Run the newly built direct executable with a supported legal ROM.
- Fix any compile or first-run defects discovered by that validation.
- Publish the resulting Windows ZIP artifact.

Deferred until after this fast test:

- Original intro, title, menu and world-map presentation.
- Full music and sound-effect fidelity.
- Every collectible, enemy and scenery callback.
- Original checkpoint and exit callbacks.
- Complete DK/Diddy partner, loss and swap behavior.

The deferred systems remain necessary for a faithful Jungle Hijinxs conversion,
but they no longer block the earliest playable feedback build.
