/*
 * baba-is-c -- a demake -- by yottatsa 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "fastconio.h"
#include "game.h"
#include "resources.h"

#ifndef __C64__
#define DEBUG
#endif

#ifdef __C64__
#define GRAPHIC
#endif


// 25 dim pf%(mx):rem playfield map
unsigned char pf[MX], pfbm[MX];
unsigned char props[16];
unsigned char objs[8];
unsigned char p_you, p_stop, p_win, p_push, is_alive, is_won;
unsigned char *sprite_codes;
#define is_you(v) (v & p_you)
#define is_stop(v) (v & p_stop)
#define is_win(v) (v & p_win)
#define is_push(v) (v & p_push)


void unpack_level(char *lv) {
	unsigned char i, x, t, tile;

	memset(pf, 0, MX);
	memset(pfbm, 0, MX);
	lv++; //W
	lv++; //H

	// move object to bitmap layers, leaving only code in the indexed layer
	for (i = 0; i < MX; ++i) {
		tile = lv[i];
		if (tile < 8) {
			pfbm[i] = (1 << tile);
		} else {
			pf[i] = tile;
		}
	}

}

void draw_screen(unsigned char draw_me) {
	unsigned char i, x, y, tile, tile_index;

	x = 0; y = 0;
	fastgotoxy(x, y);
	for (i = 0; i < MX; ++i) {
		if (x == W) {
			x = 0;
			y++;
		       	fastgotoxy(x, y);
		}
		tile = pfbm[i];

		// get code tile
		tile_index = pf[i] & 31; 
		if (tile & 1 << 0) tile_index = 0;
		else if (tile & 1 << 1) tile_index = 1;
		else if (tile & 1 << 2) tile_index = 2;
		else if (tile & 1 << 3) tile_index = 3;
		else if (tile & 1 << 4) tile_index = 4;
		else if (tile & 1 << WATER) tile_index = WATER;
		else if (tile & 1 << 6) tile_index = 6;
		else if (tile & 1 << 7) tile_index = 7;

		// decode tile functions
		if (is_stop(tile)) {
			tile_index = props[STOP];
		}
		if (is_push(tile)) {
			tile_index = props[PUSH];
		}
		if (is_win(tile)) {
			tile_index = props[WIN];
		}
		if (is_you(tile)) {
			// if (draw_me) fasttextcolor(TILE_PLAYER);  //todo
			tile_index = props[YOU];
		}

		fasttextcolor(sprites_colors[tile_index + tile_index]);
		fastcputc(&sprite_codes[tile_index]);
		x++;
	}
	textcolor(TEXTCOLOR);
}

void win(void) {
	is_won = 1;
	gotoxy(0, H+2);
	printf("congratulations. press any for next\n");
	cgetc();
}

void lose(void) {
	is_alive = 0;
	gotoxy(0, H+2);
	printf("nothing is you. press any for repeat\n");
	cgetc();
}

void calculate(void) {
	/* Works only with code layer
	 */

	unsigned char i, tile;
	memset(props, 0, 16);
	memset(objs, 0, 8);
	for (i = 0; i < MX; ++i) {
		tile = pf[i] & 31;
		if (tile == IS) {
			props[(pf[i + 1] & 31) - 16] = (pf[i - 1] & 31) - 8;
			objs[(pf[i - 1] & 31) - 8] = (pf[i + 1] & 31) - 16;
		}
	}

	p_you = 1 << props[YOU], p_stop = 1 << props[STOP], p_win = 1 << props[WIN], p_push = 1 << props[PUSH];

	if (props[0] == 0) lose();
}

char update(char my_position, signed char shift) {
	unsigned char me = pfbm[my_position];
	unsigned char neigh = pfbm[my_position + shift];
	unsigned char act = 0, prop = 0;
	
	if (neigh & 1 << 0) act = act | 1 << objs[0];
	if (neigh & 1 << 1) act = act | 1 << objs[1];
	if (neigh & 1 << 2) act = act | 1 << objs[2];
	if (neigh & 1 << 3) act = act | 1 << objs[3];
	if (neigh & 1 << 4) act = act | 1 << objs[4];
	if (neigh & 1 << 5) act = act | 1 << objs[5];
	if (neigh & 1 << 6) act = act | 1 << objs[6];
	if (neigh & 1 << 7) act = act | 1 << objs[7];
	
#ifdef DEBUG
	gotoxy(0, H+6);
	printf("%2d, %3d, 0x%3x 0x%3x", my_position, shift, neigh, act);
#endif

	if (act & 1 << WIN) {
		win();
		return 0;
	}
	if (act & 1 << STOP) {
		return 0;
	}
	if (act & 1 << PUSH) {
		pfbm[my_position + shift + shift] = pfbm[my_position + shift + shift] | p_push;
		pfbm[my_position + shift] = neigh & ~p_push | p_you;
		pfbm[my_position] = me & ~p_you;
		return 0;
	}
	if (act & 1 << YOU || act == 0) { // hack
		pfbm[my_position + shift] = neigh | p_you;
		pfbm[my_position] = me & ~p_you;
	}
	return 0;
}

void move(signed char x, signed char y) {
	signed char i;
	#define shift (signed char)(x + y * W)
	#define tile(i) (pfbm[i] & 31)
	#define call_update(i) if (is_you(tile(i))) update(i, shift);
	if (shift > 0) {
		for (i = MX - 1; i >= 0; --i) call_update(i)
	} else if (shift < 0) {
		for (i = 0; i < MX; ++i) call_update(i)
	}
}

unsigned char main_loop(void) {
	unsigned char k;
#ifdef DEBUG
	unsigned char i;
#endif
	is_alive = 1;
	is_won = 0;
	calculate(); // speed debug only
	while (is_won == 0) {
		//calculate();
		draw_screen(is_alive);
		

		if (is_alive == 0) return 1;

		k = cgetc();
		switch (k) {
			case 'q':
				return 0;
			case 'r':
				return 1;
			case 'a':
				move(-1, 0);
				break;
			case 'd': 
				move(1, 0);
				break;
			case 'w': 
				move(0, -1);
				break;
			case 's': 
				move(0, 1);
				break;
			default: ;;
		}
	}
	return 0;
}

#ifdef __C64__

#define PROCESSOR_PORT				*((unsigned char*)0x01)
#define PROCESSOR_PORT_IOCHAR_VISIBILITY	0x04
#define CIA1_CRA_TIMER				0x01

/* https://www.c64-wiki.com/wiki/VIC_bank https://sta.c64.org/cbm64mem.html
 * Make sure bits 0 and 1 are set to outputs and change banks
 */
#define CIA2_SET_BANK(x) CIA2.ddra = CIA2.ddra | 0x03; CIA2.pra = CIA2.pra & ~x
unsigned char *chargen = (void*)0x3000;

void set_graphic_mode(void) {
	unsigned char *origchargen = (void*)0xd000;

	/* https://www.c64-wiki.com/wiki/CIA
	 * Turn off interrupts
	 */
	CIA1.cra = CIA1.cra & ~CIA1_CRA_TIMER;

	/* https://sta.c64.org/cbm64mem.html
	 * Make Character ROM visible at $D000-$DFFF.
	 */
	PROCESSOR_PORT = PROCESSOR_PORT & ~PROCESSOR_PORT_IOCHAR_VISIBILITY;

	// Copy character set from ROM to RAM
	memcpy(chargen, origchargen, 1024); 

	/* https://sta.c64.org/cbm64mem.html
	 * Restore I/O area visible at $D000-$DFFF.
	 */
	PROCESSOR_PORT = PROCESSOR_PORT | PROCESSOR_PORT_IOCHAR_VISIBILITY;
	
	// Turn on interrupts
	CIA1.cra = CIA1.cra | CIA1_CRA_TIMER;

	/* https://www.c64-wiki.com/wiki/VIC_bank https://sta.c64.org/cbm64mem.html
	 * 3: $C000–$FFFF
	 */
	//CIA2_SET_BANK(0x03);

	/* https://www.c64-wiki.com/wiki/53272 https://sta.c64.org/cbm64mem.html
	 * When in text screen mode, the VIC-II looks to 53272 for information on where the character set and text screen character RAM is located:
	 * - The four most significant bits form a 4-bit number in the range 0 thru 15: Multiplied with 1024 this gives the start address for the screen character RAM.
	 * - Bits 1 thru 3 (weights 2 thru 8) form a 3-bit number in the range 0 thru 7: Multiplied with 2048 this gives the start address for the character set.
	 * Notice that all the start addresses of character sets, screen character RAM, bitmaps, and color information, are all relative to the start address of the current VIC bank!.
	 */
	// VIC.addr = (VIC.addr & 0x0F) | 0xC0; // b1100.... -> screen memory 0xf000
	// VIC.addr = (VIC.addr & 0xF0) | 0x0E; // b....1110 -> char memory ??
	VIC.addr = (VIC.addr & 240) | 12;
	
	/* https://www.c64-wiki.com/wiki/Standard_Character_Mode
	 * In Control Register 1 ($d011) bit 6 (ECM) must be cleared and bit 5 (BMM) must be cleared. In Control Register 2 ($d016), bit 4 (MCM) must be cleared. 
	 */
	// ECM
	VIC.ctrl2 = VIC.ctrl2 & ~(1 << 4);
	VIC.ctrl1 = (VIC.ctrl1 & ~(1 << 5)) | (1 << 6);
}
#endif


int main (void) {
	unsigned char i;

	fastbgcolor(BGCOLOR);
	textcolor(TEXTCOLOR);

	sprite_codes = (unsigned char*) malloc(32);
	for (i = 0; i < 32; ++i) {
		sprite_codes[i] = (unsigned char) i+32;
#ifdef GRAPHIC
		sprite_codes[i] = bgcolormask(sprites_colors[i + i + 1], sprite_codes[i]);
#endif
	}

#ifdef GRAPHIC
	VIC.bgcolor1 = BGCOLOR1;
	VIC.bgcolor2 = BGCOLOR2;
	VIC.bgcolor3 = BGCOLOR3;

	set_graphic_mode();

	memcpy(chargen + 32 * 8, sprites, 256);
#endif

	for (i = 0; i < ML; ++i) {
		do {
			clrscr();
			printf("loading level %d", i+1);
			unpack_level(levels[i]);
			clrscr();
		} while (main_loop() != 0);
	}
	return EXIT_SUCCESS;
}
