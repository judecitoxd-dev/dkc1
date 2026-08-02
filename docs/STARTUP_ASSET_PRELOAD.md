# Startup asset preload

After the user selects and validates a legal USA Rev 2 ROM, the Windows preview
now prepares Jungle Hijinxs before the intro begins:

1. Load or restore every configured scene VRAM package.
2. Load the complete scene palette recipe into CGRAM.
3. Initialize the camera-driven BG1 ring tilemap.
4. Initialize the first visible source objects.
5. Render one hidden frame to materialize the initial player, Gnawty and Barrel
   frame uploads.

The prepared level runtime remains resident while the provisional intro, title,
game menu and Kongo Jungle map are shown. Entering Jungle Hijinxs for the first
time consumes that already prepared runtime instead of loading the level a
second time. A later re-entry rebuilds and warms a clean runtime.

The Windows package also contains a dedicated direct tester. It performs the
same ROM validation and startup warmup, then opens Jungle Hijinxs immediately.
The window title reports route progress from 0 to 100 percent and a completion
message appears at the current provisional end.

The intro/title/menu/map artwork itself is still provisional native UI. This
milestone preloads the original level textures, palettes and first visible
sprites; identifying and rendering the original front-end screen packages is a
separate fidelity task.
