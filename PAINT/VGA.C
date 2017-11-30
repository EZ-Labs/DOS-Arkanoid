#include <STDIO.H>
#include <DOS.H>
#include "VGA.H"

/* Uses BIOS int 10h to set video mode (no VESA Modes) */
void vgaSetMode(char mode) {
	
  union REGS regs;
  
  regs.h.ah = 0x00;
  regs.h.al = mode;
  int86(0x10, &regs, &regs);
}