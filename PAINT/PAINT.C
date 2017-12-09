#include <STDIO.H>
#include <DOS.H>
#include "PAINT.H"
#include "MYTYPES.H"
#include "VGA.H"

#define BIT(n) (0x01 << n)
#define LB_PRESS BIT(0)
#define RB_PRESS BIT(1)
#define MB_PRESS BIT(2)

typedef struct {
	int16_t x;
	int16_t y;
} mouse_coords_t;

mouse_coords_t mouse_coords = {0, 0};

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

uint8_t mouseHandler() {
	
	union REGS regs;
	
	regs.x.ax = 0x03;
	int86(0x33, &regs, &regs);
	
	if(regs.h.bl & RB_PRESS) {
		
		mouseHideCursor();
		vg_pixel(regs.x.cx >> 1, regs.x.dx, 0x03);
		mouseShowCursor();
	}
	
	return regs.h.bl;
}

void updateMousePosition(void) {
	
	union REGS read;
	union REGS set;
	
	read.x.ax = 0x0B;
	int86(0x33, &read, &read);
	
	mouse_coords.x += read.x.cx;
	mouse_coords.y += read.x.dx;
	
	if(mouse_coords.x < 0) mouse_coords.x = 0;
	if(mouse_coords.y < 0) mouse_coords.y = 0;
	if(mouse_coords.x > 640) mouse_coords.x = 640;
	if(mouse_coords.y > 200) mouse_coords.y = 200;
	
	set.x.ax = 0x04;
	set.x.cx = mouse_coords.x;
	set.x.dx = mouse_coords.y;
	
	int86(0x33, &set, &set);
}

int main(int argc, char** argv) {
	
	uint8_t buttonStatus;
	
	vgaSetMode(VGA256);
	
	mouseShowCursor();
	
	do {
		
		buttonStatus = mouseHandler();
		updateMousePosition();
	} while(!(buttonStatus & LB_PRESS));
	
	mouseHideCursor();
	
	vgaSetMode(TEXT);
	
	return 0;
}