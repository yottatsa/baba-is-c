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
	unsigned char i, tile;
	char *tile_str;

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
	fastgotoxy(0, 0);
	fasttextcolor(TILE_INACTIVE);
	for (i = 0; i < MX; ++i) {
		tile = pfbm[i];

		// get code tile
		tile_str = (char *) gr[pf[i] & 31]; 
		fasttextcolor(TILE_INACTIVE);

		// decode tile bitmap
		if (tile & 1 << 0) tile_str = (char *) gr[0];
		if (tile & 1 << 1) tile_str = (char *) gr[1];
		if (tile & 1 << 2) { tile_str = (char *) gr[2]; fasttextcolor(TILE_ROCK);}
		if (tile & 1 << 3) tile_str = (char *) gr[3];
		if (tile & 1 << 4) tile_str = (char *) gr[4];
		if (tile & 1 << WATER) { tile_str = (char *) gr[WATER]; fasttextcolor(TILE_WATER);}
		if (tile & 1 << 6) tile_str = (char *) gr[6];
		if (tile & 1 << 7) tile_str = (char *) gr[7];

		// decode tile functions
		if (is_stop(tile)) {
			fasttextcolor(TILE_STOP);
			tile_str = (char *) gr[props[STOP]];
		}
		if (is_push(tile)) {
			fasttextcolor(TILE_PUSH);
			tile_str = (char *) gr[props[PUSH]];
		}
		if (is_win(tile)) {
			fasttextcolor(TILE_WIN);
			tile_str = (char *) gr[props[WIN]];
		}
		if (is_you(tile)) {
			if (draw_me) fasttextcolor(TILE_PLAYER);
			tile_str = (char *) gr[props[YOU]];
		}


		fastcputs(tile_str, 4);
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
