#ifndef DK1_SPC_MEMORY_H
#define DK1_SPC_MEMORY_H

/* The SPC700 exposes one 64 KiB address space. Keep the capacity in a single
 * header so driver-startup and catalog code cannot redefine it differently. */
#define DK1_SPC_RAM_SIZE 0x10000u

#endif
