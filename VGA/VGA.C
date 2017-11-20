#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <limits.h>
#include <dos.h>

#define MODE13H_SIZE 64000L

static char* video_mem = (char*) 0xA0000000L;
static char* double_buffer;
static unsigned short h_res = 320;
static unsigned short v_res = 200;
static char bits_per_pixel = 8;

static void set_video_mode(char mode);
static void vg_fill(unsigned char color);
static unsigned long parse_ulong(char* str, int base);
static void vg_draw(void);
static void vg_pixel(unsigned short x, unsigned short y, unsigned long color);
static void vg_palette(void);

static FILE* logger;

int main(int argc, char** argv) {
	
	unsigned long color;
	size_t n;
	color = parse_ulong(argv[1], 16);
	
	if (color == ULONG_MAX) {
		printf("vga: color out of range\n");
		return 1;
	}
	
	double_buffer = (char*) malloc(MODE13H_SIZE);
	
	printf("Color: 0x%02X\n", color);

	set_video_mode(0x13);
	vg_fill(0);
	vg_palette();
	
	vg_draw();

	sleep(3);

	set_video_mode(0x03);	
	
	free(double_buffer);
	return 0;
}

static void vg_palette(void) {
	
	size_t i, j, k;
	unsigned char color = 0;
	char* vptr = double_buffer;
	
	for(k = 0; k < 8; k++) {
		for(i = 0; i < 10; i++) {
			for(j = 0; j < 32; j++, color++) {
				
				memset(vptr, color, 10);
				vptr += 10;
			}
			
			color -= 32;
		}
		
		color += 32;
	}
}

static void vg_draw(void) {
	
	memcpy(video_mem, double_buffer, MODE13H_SIZE);
}

static void vg_fill(unsigned char color) {
	
	memset(double_buffer, color, MODE13H_SIZE);
}

static void vg_pixel(unsigned short x, unsigned short y, unsigned long color) {
	
	char* vptr = double_buffer;
	
	if(x < 0 || y < 0 || x > h_res - 1 || y > v_res - 1) return;

	vptr += x + y * h_res;
	*vptr = color & 0xFF;
}

static void set_video_mode(char mode) {
	
  union REGS regs;
  
  regs.h.ah = 0x00;
  regs.h.al = mode;
  int86(0x10, &regs, &regs);
}

static unsigned long parse_ulong(char* str, int base) {
	
	char* endptr;
	unsigned long val;
	int errsv;

	errno = 0;
	val = strtoul(str, &endptr, base);
    errsv = errno;

    if (errsv != 0) {
		char* errorstr = strerror(errsv);
		printf("vga: parse_ulong: error %d - %s\n", errsv, errorstr);
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("vga: parse_ulong: no digits were found in \"%s\"\n", str);
		return ULONG_MAX;
	}

	return val;
}