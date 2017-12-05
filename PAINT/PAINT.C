#include <STDIO.H>
#include <DOS.H>
#include "MYTYPES.H"
#include "VGA.H"

#define BIT(n) (0x01 << n)
#define LB_PRESS BIT(0)
#define RB_PRESS BIT(1)
#define MB_PRESS BIT(2)

uint8_t mouseGetButtons() {
	
	union REGS regs;
	
	regs.x.ax = 0x03;
	int86(0x33, &regs, &regs);
	
	return regs.h.bl;
}

void mousePaint() {
	
	union REGS regs;
	
	regs.x.ax = 0x03;
	int86(0x33, &regs, &regs);
	
	vg_pixel(regs.x.cx >> 1, regs.x.dx, 0x02);
}

void mouseShowCursor() {
	
	union REGS regs;
	
	regs.x.ax = 0x01;
	int86(0x33, &regs, &regs);
}

void mouseHideCursor() {
	
	union REGS regs;
	
	regs.x.ax = 0x02;
	int86(0x33, &regs, &regs);
}

int main(int argc, char** argv) {
	
	uint8_t buttonStatus;
	
	vgaSetMode(VGA256);
	
	mouseShowCursor();
	
	do {
		
		buttonStatus = mouseGetButtons();
		if(buttonStatus & RB_PRESS) mousePaint();
		
	} while(!(buttonStatus & LB_PRESS));
	
	mouseHideCursor();
	
	vgaSetMode(TEXT);
	
	return 0;
}