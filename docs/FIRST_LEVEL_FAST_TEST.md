# Fast Jungle Hijinxs test scope

The next user-facing delivery is intentionally smaller than a faithful full-game
preview. Its purpose is to get a Windows build into the user's hands quickly and
collect real play feedback.

Current implementation estimate: **85 percent complete, 15 percent remaining**.
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
- Package the direct tester and normal shell preview together.

Remaining before delivery:

- Complete one real MSVC Windows build.
- Run the direct executable with a supported legal ROM.
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
