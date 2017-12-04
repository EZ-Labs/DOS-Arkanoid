#ifndef __VGA_H
#define __VGA_H

#define TEXT   0x03
#define VGA256 0x13

/* Uses BIOS int 10h to set video mode (no VESA Modes) */
void vgaSetMode(char mode);

void vg_pixel(unsigned short x, unsigned short y, unsigned long color);

#endif /* __VGA_H */