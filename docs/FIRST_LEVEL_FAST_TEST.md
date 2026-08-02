# Fast Jungle Hijinxs test scope

The canonical repository for this work is now:

```text
judecitoxd-dev/dkc1
```

The next user-facing delivery is intentionally smaller than a faithful full-game
preview. Its purpose is to get a Windows build into the user's hands quickly and
collect real play feedback.

Current implementation estimate: **99 percent complete, 1 percent remaining**.
This percentage measures only the fast Windows first-level tester described
below, not the complete game or a fully faithful Jungle Hijinxs conversion.

Implemented for the fast test:

- Validate the legal USA Rev 2 ROM.
- Preload scene textures, palettes and first visible sprites.
- Start Jungle Hijinxs directly from a dedicated executable.
- Render the camera-driven level background.
- Support walking, jumping, terrain collision and camera movement.
- Stream the currently translated Gnawty and Barrel actors.
- Track monotonic route progress from 0 to 100 percent and four gameplay checkpoints.
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
  for valid terrain near seven points from 0 to 99 percent, updates camera,
  dynamic BG1 and object streaming, renders a real frame at every point and
  verifies provisional completion.
- Require monotonic world/camera movement, advancing BG1 generation or uploaded
  columns, different rendered frames between the beginning and end, and a final
  rendered position within 256 world pixels of the provisional exit.
- Record per-checkpoint world position, camera, active object counts, nonzero
  pixel count, BG1 generation/columns and frame signatures in a route report.
- Provide a local Visual Studio builder that compiles the two Windows frontends
  plus the headless validator, builds ROM-backed regressions for compile
  coverage, runs key ROM-independent smoke tests, records the build log,
  packages the result and calculates its SHA-256.
- Refuse to package a ROM-certified build unless both reports contain
  `ready=1`, the route report contains seven checkpoints,
  `streaming_progress=1` and provisional completion is confirmed.
- Allow a legal ROM to be dragged onto `BUILD-WINDOWS-PREVIEW.bat` to build,
  certify and package in one operation.
- Package the direct tester, normal shell preview and route validator together.

## Successful hosted build

The migrated repository's first validation run succeeded on both runners:

- Linux configured and compiled the project, ran the ROM-independent suite and
  checked Python syntax.
- Windows compiled the preview, direct tester, headless route validator and the
  selected regression targets.
- The Windows smoke tests `first_level_route`, `dynamic_bg1_runtime` and
  `software_frontend_dispose` all passed.
- GitHub Actions uploaded artifact `8837513552` with artifact digest
  `sha256:4c2242154f54dc5c0f770942c56fe88a31c7e297e800c487d6f3b55e953b87bd`.

The downloadable package inside that artifact contains:

```text
DK1-Jungle-Hijinxs-Direct-Test.exe
DK1-Jungle-Hijinxs-Preview.exe
DK1-Jungle-Hijinxs-Route-Validate.exe
DK1-Windows-Build.log
README.txt
```

The inner user package SHA-256 is:

```text
42a8033428d8d628750dc186ab595b32df5bfe936cd135c39ea407e9fcbd6a39
```

Remaining before marking the fast tester complete:

- Launch the newly compiled direct tester with a supported legal ROM.
- Confirm startup preflight reports `ready=1`.
- Run or play through the level and confirm the seven-point route validator
  reports `ready=1`, `streaming_progress=1` and `route_completed=1`.
- Fix any first-run or gameplay defect revealed by that ROM-backed test.

Deferred until after this fast test:

- Original intro, title, menu and world-map presentation.
- Full music and sound-effect fidelity.
- Every collectible, enemy and scenery callback.
- Original checkpoint and exit callbacks.
- Complete DK/Diddy partner, loss and swap behavior.

The deferred systems remain necessary for a faithful Jungle Hijinxs conversion,
but they no longer block the earliest playable feedback build.
