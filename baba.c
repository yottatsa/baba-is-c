/*
 * baba-is-c -- a demake -- by yottatsa 2020
 *
 * Based on
 * 9 rem baba is c64 -- a demake -- by nick bensema 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "fastconio.h"
#include "game.h"

#define PROGRESS cputc(*".");
#define TEXTCOLOR WHITE

#ifndef __C64__
#define DEBUG
#endif


// 25 dim pf%(mx):rem playfield map
unsigned char pf[MX], pfbm[MX];
unsigned char props[16];
unsigned char objs[8];
unsigned char p_you, p_stop, p_win, p_push, is_alive, is_won;
unsigned char *sprites;
#define is_you(v) (v & p_you)
#define is_stop(v) (v & p_stop)
#define is_win(v) (v & p_win)
#define is_push(v) (v & p_push)


void unpack_level(char *lv) {
	unsigned char i, x, t, tile;

	// 119 for x=0 to mx:pf%(x)=0:next x
	memset(pf, 0, MX);
	PROGRESS

	// 120 i=0
	// 135 x=1
	i = 0;
	x = 0;

	// 140 t=asc(mid$(lv$,x,1))
	t = (int) lv[x];

	// 155 i=i+1:x=x+1:ifx<=len(lv$)andi<mxthen 140
	while (t != 0 && i < MX) {
		/*
		if ('a' <= t && t <= 'z') t = t - 32;
		else if ('A' <= t && t <= 'Z') t = t + 128;
		if (t == 126) t = 255;
		*/
		
		// 141 ift=191then l2=l2+1:goto 120
		if (t == 191) return;

		// 144 ift=255thent=34
		if (t == 255) t = 34;

		// 145 ift>=64 then pf%(i)=31andt:ift>128theni=i+1:pf%(i)=is
		if (t >= 64) {
			pf[i] = 31 & t;
			if (t > 128) {
				i = i + 1;
				pf[i] = IS;
			}
		}

		// 147 ift>32 and t<48 and i<mx then pf%(i)=0:i=i+1:t=t-1:goto 147
		while (t > 32 && t < 48 && i < MX) {
			pf[i] = 0;
			i = i + 1;
			t = t - 1;
		}

		// 150 ift>48andt<64andi<mxthenpf%(i)=pf%(i-1):ift>49theni=i+1:t=t-1:goto150
		if (t > 48 && t < 64 && i < MX) {
			while (t > 49) {
				if (t > 48 && t < 64 && i < MX) pf[i] = pf[i-1];
				i = i + 1;
				t = t - 1;
			}
		}

		// 155 i=i+1:x=x+1:ifx<=len(lv$)andi<mxthen 140
		i = i + 1;
		x = x + 1;

		// 140 t=asc(mid$(lv$,x,1))
		t = (int) lv[x];
		PROGRESS
	}
	
	// move object to bitmap layers, leaving only code in the indexed layer
	for (i = 0; i < MX; ++i) {
		tile = pf[i] & 31;
		if (tile < 8) {
			pfbm[i] = (1 << tile);
			pf[i] = 0;
		}
	}

}

#define TILE_INACTIVE DARKGRAY
#define TILE_FLOOR DARKRED
#define TILE_PLAYER WHITE
#define TILE_WIN GREEN
#define TILE_WATER LIGHTBLUE
#define TILE_STOP LIGHTGRAY
#define TILE_PUSH ORANGE
#define TILE_ROCK ORANGE
void draw_screen(unsigned char draw_me) {
	unsigned char i, x, y, tile, tile_index;
	unsigned char *tile_str;

#define draw_tile(t) \
	tile_str = sprites; \
	tile_str += t; \
	fastcputs(tile_str, 1);

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
	x = 0; y = 0;
	for (i = 0; i < MX; ++i) {
		if (x == 0) fastgotoxy(x, y);
		tile = pfbm[i];

		// get code tile
		tile_index = pf[i] & 31; 
		fasttextcolor(TILE_INACTIVE);

		// decode tile bitmap
		if (tile & 1 << 0) tile_index = 0;
		if (tile & 1 << 1) tile_index = 1;
		if (tile & 1 << 2) { tile_index = 2; fasttextcolor(TILE_ROCK);}
		if (tile & 1 << 3) tile_index = 3;
		if (tile & 1 << 4) tile_index = 4;
		if (tile & 1 << WATER) { tile_index = WATER; fasttextcolor(TILE_WATER);}
		if (tile & 1 << 6) tile_index = 6;
		if (tile & 1 << 7) tile_index = 7;

		// decode tile functions
		if (is_stop(tile)) {
			fasttextcolor(TILE_STOP);
			tile_index = props[STOP];
		}
		if (is_push(tile)) {
			fasttextcolor(TILE_PUSH);
			tile_index = props[PUSH];
		}
		if (is_win(tile)) {
			fasttextcolor(TILE_WIN);
			tile_index = props[WIN];
		}
		if (is_you(tile)) {
			if (draw_me) fasttextcolor(TILE_PLAYER);
			tile_index = props[YOU];
		}

		draw_tile(tile_index);
		x++;
		if (x == W) {
			x = 0;
			y++;
		}
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
	// swap me
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
	while (is_won == 0) {
		calculate();
		draw_screen(is_alive);
		

#ifdef DEBUG
		gotoxy(0, H+3);
		for (i = 0; i < 8; ++i) {
			printf("%s:%s %d ", gr[i], gr[objs[i]+16], objs[i]);
		}
		gotoxy(0, H+4);
		for (i = 0; i < 16; ++i) {
			printf("%d:%s %d ", i, gr[props[i]], props[i]);
		}
#endif

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

void set_char(void) {
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
	//VIC.addr = 28;
	VIC.addr = (VIC.addr & 240) | 12;
}
#endif

int main (void) {
	unsigned char i;

	sprites = (unsigned char*) malloc(32);
	for (i = 0; i < 32; ++i) {
		sprites[i] = (unsigned char) i+64;
	}

#ifdef __C64__
	set_char();
	for (i = 0; i < 32; ++i) {
		memset(chargen + (i + 64)*8, 255, 8);
	}
#endif

	fastbgcolor(BLACK);
	textcolor(TEXTCOLOR);

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
