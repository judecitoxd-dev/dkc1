# Dynamic BG1 tilemap streaming

The scene loader initializes character graphics, tilemaps and palettes from the
legal user-provided ROM or its validated local cache. During gameplay the BG1
runtime now maintains a separate 64 KiB VRAM shadow for camera-dependent map
updates.

Each 32-pixel camera column entering the viewport is expanded from the original
level map into four SNES tile columns and written into the BG1 ring tilemap. The
renderer reads that runtime VRAM image using the current camera scroll, while
player and object frame graphics continue through their existing OAM/DMA paths.

This milestone covers camera-driven BG1 tilemap words. It does not claim that
all original transition-time graphics packages, palette animation, effects or
mid-level special uploads have been translated.
