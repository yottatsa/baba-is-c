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
unsigned char pf[MX];
unsigned char props[16];
unsigned char objs[8];


void unpack_level(char *lv) {
	unsigned char i, x, t;

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
}

#define TILE_INACTIVE DARKGRAY
#define TILE_ACTIVE YELLOW
void draw_screen(unsigned char draw_me) {
	unsigned char i, tile, you = props[0];
	char *tile_str;

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
	fastgotoxy(0, 0);
	fasttextcolor(TILE_INACTIVE);
	for (i = 0; i < MX; ++i) {
		tile = pf[i] & 31;
		tile_str = (char *) gr[pf[i] & 31];
		if (draw_me && tile == you) fasttextcolor(TILE_ACTIVE);
		fastcputs(tile_str, 4);
		if (draw_me && tile == you) fasttextcolor(TILE_INACTIVE);
	}
	textcolor(TEXTCOLOR);
}

void win(void) {
	gotoxy(0, H+2);
	printf("congratulations. press any for next\n");
	cgetc();
}

void lose(void) {
	gotoxy(0, H+2);
	printf("nothing is you. press any for repeat\n");
	cgetc();
}

unsigned char calculate(void) {
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
	if (props[0] == 0) return 0;
	return 1;
}

char update(char my_position, signed char shift) {
	unsigned char you = props[0];
	unsigned char neigh = pf[my_position + shift];
	unsigned char neigh_act = objs[neigh];
	
#ifdef DEBUG
	gotoxy(0, H+6);
	printf("%2d, %3d, %d, %d, %s", my_position, shift, neigh, neigh_act, gr[neigh_act + 16]);
#endif
	// swap me
	switch (neigh_act) {
		case WIN:
			return 1;
		case STOP:
			break;
		case YOU:
			pf[my_position] = pf[my_position + shift]; // should be BG
			pf[my_position + shift] = you;
			break;
		case PUSH:
		default:
			// should run checks recursively
			pf[my_position] = pf[my_position + shift + shift]; // should be BG
			pf[my_position + shift + shift] = pf[my_position + shift];
			pf[my_position + shift] = props[0];
			break;
	}
	return 0;
}

char move(signed char x, signed char y) {
	signed char i;
	unsigned you = props[0];
	#define shift (signed char)(x + y * W)
	#define tile(i) (pf[i] & 31)
	#define check_and_call_update(i) if (tile(i) == you) if (update(i, shift) == 1) return 1;
	if (shift > 0) {
		for (i = MX - 1; i >= 0; --i) check_and_call_update(i)
	} else if (shift < 0) {
		for (i = 0; i < MX; ++i) check_and_call_update(i)
	}
	return 0;
}

unsigned char main_loop(void) {
#define call_check_move(x, y) if (move(x, y) != 0) { win(); return 0; }
	unsigned char k, i, alive;
	while (1) {
		alive = calculate();
		draw_screen(alive);
		

#ifdef DEBUG
		gotoxy(0, H+3);
		for (i = 0; i < 8; ++i) {
			printf("%s:%s %d ", gr[i], gr[objs[i]+16], objs[i]);
		}
#endif

		if (alive == 0) {
			lose();
			return 1;
		}

		k = cgetc();
		switch (k) {
			case 'q':
				return 0;
			case 'r':
				return 1;
			case 'a':
				call_check_move(-1, 0);
				break;
			case 'd': 
				call_check_move(1, 0);
				break;
			case 'w': 
				call_check_move(0, -1);
				break;
			case 's': 
				call_check_move(0, 1);
				break;
			default: ;;
		}
	}
	return 0;
}

int main (void) {
	unsigned char i;

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
