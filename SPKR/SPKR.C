#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define TIMER_FREQ       1193182		
#define TIMER_HOOK_BIT	 0				

#define ERROR			 0xFF			
#define COUNTER_MIN		 18				
#define COUNTER_MAX		 596591			

#define BIT(n)			 (0x01<<(n))

#define TIMER0_IRQ	     0				

#define TIMER_OUT        BIT(7)     	
#define TIMER_SEL(n)	 (n << 6)   	
#define TIMER(n)		 (0x40 + n) 	

#define MAX_UINT16		 0xFFFF			

/* I/O port addresses */

#define TIMER_0			 0x40				
#define TIMER_1			 0x41				
#define TIMER_2			 0x42				
#define TIMER_CTRL		 0x43				

#define SPEAKER_CTRL	 0x61				

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0		 0x00				
#define TIMER_SEL1		 BIT(6)				
#define TIMER_SEL2		 BIT(7)				
#define TIMER_RB_CMD	 (BIT(7)|BIT(6))  	

/* Register selection: bits 5 and 4 */

#define TIMER_LSB		 BIT(4)  			
#define TIMER_MSB		 BIT(5)  			
#define TIMER_LSB_MSB	 (TIMER_LSB | TIMER_MSB)

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE	 (BIT(2)|BIT(1)) 	
#define TIMER_RATE_GEN	 BIT(2)          	
#define TIMER_MODE_0	 0x00 				
#define TIMER_MODE_1	 BIT(1)				
#define TIMER_MODE_4	 BIT(3)				
#define TIMER_MODE_5	 (BIT(3)|BIT(1))	

/* Counting mode: bit 0 */

#define TIMER_BCD		 0x01   			
#define TIMER_BIN		 0x00   			

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT   BIT(5)
#define TIMER_RB_STATUS  BIT(4)
#define TIMER_RB_SEL(n)  BIT((n)+1)

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

int timer_set_square(unsigned char timer, unsigned long freq) {

	unsigned char status;
	unsigned short div = 0;
	
	outportb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_COUNT | TIMER_RB_SEL(timer));
	
	status = inportb(TIMER(timer));

	/*printf("Status byte of timer %d: 0x%02X\n\n", timer, status);*/

	outportb(TIMER_CTRL, TIMER_SEL(timer) | TIMER_LSB_MSB | (status & 0x0F));

	div = (unsigned short) (TIMER_FREQ / freq);
	
	printf("Freq: %d\n", freq);

	if (freq <= COUNTER_MIN) {
		printf("Frequency should be between 18.2Hz and 596591Hz\n");
		printf("Set timer %d frequency to 18.2Hz\n\n", timer);
		div = MAX_UINT16;
	} else if (freq > COUNTER_MAX) {
		printf("Frequency should be between 18.2Hz and 596591Hz\n");
		printf("Set timer %d frequency to 596591Hz\n\n", timer);
		div = 2;
	}

#if defined(DEBUG) && DEBUG == 1
	printf("Counter initialized to: %d\n\n", div & 0xFFFF);
#endif

	outportb(TIMER(timer), div & 0xFF);
	outportb(TIMER(timer), (div >> 8) & 0xFF);

	return 0;
}

int timer_display_conf(unsigned char conf) {

	printf("Counting mode:   ");
	if ((conf & TIMER_BCD) == TIMER_BCD) {
		printf("BCD\n");
	} else {
		printf("Binary\n");
	}

	printf("Programmed mode: ");
	if ((conf & TIMER_SQR_WAVE) == TIMER_SQR_WAVE)
		printf("Mode 3 (Square Wave Mode)\n");
	else if ((conf & TIMER_MODE_5) == TIMER_MODE_5)
		printf("Mode 5 (Hardware Triggered Strobe)\n");
	else if ((conf & TIMER_MODE_1) == TIMER_MODE_1)
		printf("Mode 1 (Hardware Retriggerable One-Shot)\n");
	else if ((conf & TIMER_MODE_4) == TIMER_MODE_4)
		printf("Mode 4 (Software Triggered Strobe)\n");
	else if ((conf & TIMER_RATE_GEN) == TIMER_RATE_GEN)
		printf("Mode 2 (Rate Generator)\n");
	else printf("Mode 0 (Interrupt On Terminal Count)\n");

	printf("Type of access:  ");
	if ((conf & TIMER_LSB_MSB) == TIMER_LSB_MSB)
		printf("LSB followed by MSB\n");
	else if ((conf & TIMER_LSB) == TIMER_LSB)
		printf("LSB\n");
	else if ((conf & TIMER_MSB) == TIMER_MSB)
		printf("MSB\n");
	else printf("Counter Latch Command\n");

	printf("Output:          ");
	if ((conf & TIMER_OUT) == TIMER_OUT)
		printf("High\n");
	else printf("Low\n");

	printf("\n");
	return 0;
}

int main(int argc, char *argv[]) {

	unsigned char speaker_status = 0;
	unsigned char timer_status = 0;
	unsigned long freq;
	unsigned char data = 0;
    unsigned char kbd_code = 0;
	char* end;
	
	freq = strtol(argv[1], &end, 10);

	data = inportb(PIC_PORT);

    data = data | KBD_INT_BIT; /* disable keyboard interrupt */

    outportb(PIC_PORT, data);

	speaker_status = inportb(SPEAKER_CTRL);
	outportb(SPEAKER_CTRL, speaker_status | BIT(0) | BIT(1));
	
    while(kbd_code != ESC_BREAK) {

        kbd_code = kbd_read();
		/* if(kbd_code != KBD_ERROR) printf("code: 0x%02X\n", kbd_code); */
        if(kbd_code ==  0x11) {
			if(freq < 4186) freq += 5;
			timer_set_square(2, freq);
		} else if(kbd_code == 0x1F) {
			if(freq > 20) freq -= 5;
			timer_set_square(2, freq);
		}
    }

    data = inportb(PIC_PORT);

    data = data & ~(KBD_INT_BIT);

    outportb(PIC_PORT, data);
	
	speaker_status = inportb(SPEAKER_CTRL);
	outportb(SPEAKER_CTRL, speaker_status & 0xFC);
	
	/*
	outportb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_COUNT | TIMER_RB_SEL(2));
	timer_status = inportb(TIMER_2);
	
	timer_display_conf(timer_status);
	*/
	
	return 0;
}