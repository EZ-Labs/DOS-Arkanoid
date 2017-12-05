#include <STDIO.H>
#include <DOS.H>
#include "VGA.H"
#include "PAINT.H"

static char far* video_mem = (char far*) 0xA0000000L;
static unsigned short h_res = 320;
static unsigned short v_res = 200;
static char bits_per_pixel = 8;

/* Uses BIOS int 10h to set video mode (no VESA Modes) */
void vgaSetMode(char mode) {
	
  union REGS regs;
  
  regs.h.ah = 0x00;
  regs.h.al = mode;
  int86(0x10, &regs, &regs);
}

void vg_pixel(unsigned short x, unsigned short y, unsigned long color) {
	
	char far* vptr = video_mem;
	
	if(x > h_res - 1 || y > v_res - 1) return;
	
	vptr += x + y * h_res;
	*vptr = color & 0xFF;
}