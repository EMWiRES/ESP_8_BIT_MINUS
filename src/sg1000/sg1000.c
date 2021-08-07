/*
 * SG1000 emulator main file.
 *
 * Based on ColecoVision emulator https://github.com/EMWiRES/ESP_8_BIT_PLUS.
 * Based on ESP_8_BIT https://github.com/rossumur/ESP_8_BIT.
 *
 * (c) 2021 EMWiRES / Allard van der Bas.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "sg1000.h"
#include "z80.h"
#include "sn76496.h"
#include "tms9918.h"
#include "system.h"

/* SG1000 context */
t_sg1000 sg1000;

tms9918 *cv_vdp=NULL;

unsigned char *sg1000_get_video(void) {
	return(cv_vdp->videoout);
}

/* Run the virtual console emulation for one frame */
void sg1000_frame(int skip_render) {
	int cnt;
	
	// 262 scanlines 192 active.
    for(cnt = 0; cnt < 262; cnt++) {
        
		tms9918_periodic(cv_vdp,cnt);
        
		/* Run the Z80 for a line */
        z80_execute(227);
    }

    /* Update the emulated sound stream */
    SN76496Update(0, snd.buffer, snd.bufsize, 0xFF);
}

void sg1000_init(void) {
    cpu_reset();
    sg1000_reset();
}

void sg1000_reset(void) {
	// Reset the RAM.
    memset(sg1000.ram, 0, 0x400);
        
    if (!cv_vdp) {
    	cv_vdp = tms9918_create();
    }
    
    // Add the video output to it.
    cv_vdp->videoout = bitmap.data;
    
    // Clear the VDP RAM.
    memset(cv_vdp->memory,0,16384);
    
    // Explicitly clear everything.
    cv_vdp->status = 0;
    cv_vdp->scanline = 0;
    cv_vdp->address = 0;
    cv_vdp->flags = 0;
    cv_vdp->readahead = 0;
    cv_vdp->addrsave = 0;
    memset(cv_vdp->regs,0,8);
    
    // SN76496_init(0, MASTER_CLOCK, 255, 15720);
}

int sg1000_irq_callback(int param) {
    return (0xFF);
}

/* Reset Z80 emulator */
void cpu_reset(void) {
    z80_reset(0);
    z80_set_irq_callback(sg1000_irq_callback);
}

/* Write to memory */
void cpu_writemem16(int address, int data) {
	/* printf("Write mem: 0x%04X 0x%02X\n",address,data); */
	
	if (address & 0xC000) {
		sg1000.ram[address&0x3FF] = data;
	}
	
}

// I/O ports SG1000
//
// 040H-07FH:PSG SN76489AN
// 080H-0BFH:VDP TMS9918A
// 0C0H-0FFH:JOY
//
// 07FH PSG
// 0BEH-0BFH VDP
// 0DCH SG-1000
//
//  d7:JOY2 2??(DOWN )   1=???0=??
//  d6:JOY2 1??(UP   )   1=???0=??
//  d5:JOY1 9??(2??? )   1=???0=??
//  d4:JOY1 6??(1??? )   1=???0=??
//  d3:JOY1 4??(RIGHT)   1=???0=??
//  d2:JOY1 3??(LEFT )   1=???0=??
//  d1:JOY1 2??(DOWN )   1=???0=??
//  d0:JOY1 1??(UP   )   1=???0=??
//
// 0DDH SG-1000
//  d3:JOY2 9??(2??? )   1=???0=??
//  d2:JOY2 6??(1??? )   1=???0=??
//  d1:JOY2 4??(RIGHT)   1=???0=??
//  d0:JOY2 3??(LEFT )   1=???0=??

/* Write to an I/O port */
void cpu_writeport(int address, int data) {
    
    if ( (address & 0xC0) == 0x80) {	
		
		if (address & 1) {
	    	/* printf("tms9918_writeport1(0x%02X)\n",data); */
	    	tms9918_writeport1(cv_vdp,data);
		} else {
	    	/* printf("tms9918_writeport0(0x%02X)\n",data); */
	    	tms9918_writeport0(cv_vdp,data);
		}
			
    } else if (address & 0x40) {
		SN76496Write(0,data);
    } else {
		/* printf("cv_io_write: 0x%02x = 0x%02x.\n", address & 0xff, data); */
    }
    
}

/* Read from an I/O port */
int cpu_readport(int address) {
	unsigned char answer;
	
    if ( (address & 0xc0) == 0x80 ) {
		
		// VDP	
		if (address & 1) {
	    	return tms9918_readport1(cv_vdp);
		} else {
	    	return tms9918_readport0(cv_vdp);
		}

	}
	
	answer = 0;
	
	if ( (address & 0xC0) == 0xC0) {
		
		if (address & 1) {
			if (input.pad[1] & INPUT_LEFT)
				answer |= 0x01;
						
			if (input.pad[1] & INPUT_RIGHT)
				answer |= 0x02;
					
			if (input.pad[1] & INPUT_BUTTON1)
				answer |= 0x4;
					
			if (input.pad[1] & INPUT_BUTTON2)
				answer |= 0x8;
		} else {
			// Joystick.
			if (input.pad[1] & INPUT_UP)
				answer |= 0x40;
					
			if (input.pad[1] & INPUT_DOWN)
				answer |= 0x80;
				
			if (input.pad[0] & INPUT_UP)
				answer |= 0x01;
				
			if (input.pad[0] & INPUT_DOWN)
				answer |= 0x02;
	
			if (input.pad[0] & INPUT_LEFT)
				answer |= 0x04;
				
			if (input.pad[0] & INPUT_RIGHT)
				answer |= 0x08;
	
			if (input.pad[0] & INPUT_BUTTON1)
				answer |= 0x10;
				
			if (input.pad[0] & INPUT_BUTTON2)
				answer |= 0x20;
		}

		return(~answer);
		
    } else {
		/* printf("cv_io_read: 0x%02x.\n", address & 0xff); */
    }

    return 0xFF;
}

int cpu_readmem16(int address) {
	/* printf("Read mem: 0x%04X\n",address); */
	
	if (address < 0xC000) {
		return(cart.rom[address]);
	}
	
	return(sg1000.ram[address & 0x3FF] );
}

