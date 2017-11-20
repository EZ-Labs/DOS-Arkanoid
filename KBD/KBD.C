#include <stdio.h>

#define BIT(n) (0x01 << n)

#define OUT_BUF_FULL BIT(0)
#define BREAK_CODE BIT(7)
#define KBD_INT_BIT BIT(1)

#define STATUS_PORT 0x64
#define PIC_PORT 0x21
#define DATA_PORT 0x60

#define ESC_BREAK 0x81
#define EXT_CODE 0xE0
#define KBD_ERROR 0xFF

static unsigned char extended = 0;

unsigned char kbd_read() {

    unsigned char status = 0;

    status = inportb(STATUS_PORT);

    if(status & OUT_BUF_FULL) {
        return inportb(DATA_PORT);
    }

    return KBD_ERROR;
}

void kbd_print(unsigned char kbd_code) {
	
	if(kbd_code == EXT_CODE) {
		extended = 1;
	} else {
		
		if(kbd_code & BREAK_CODE) {
			printf("Break: ");
		} else {
			printf("Make: ");
		}
		
		if(extended != 0) {
			
			extended = 0;
			printf("0xE0 ");
		}
		
		printf("0x%02X\n", kbd_code);
	}
}

int main() {

    unsigned char data = 0;
    unsigned char kbd_code = 0;

    data = inportb(PIC_PORT);

    data = data | KBD_INT_BIT; /* disable keyboard interrupt */

    outportb(PIC_PORT, data);

    while(kbd_code != ESC_BREAK) {

        kbd_code = kbd_read();
        if(kbd_code != KBD_ERROR) kbd_print(kbd_code);
    }

    data = inportb(PIC_PORT);

    data = data & ~(KBD_INT_BIT);

    outportb(PIC_PORT, data);

    return 0;
}