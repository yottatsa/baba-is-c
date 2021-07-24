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


// 25 dim pf%(mx):rem playfield map
static unsigned char pf[MX];
unsigned char you = 52;


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
void draw_screen(void) {
	unsigned char i;
	char *tile;

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
	fastgotoxy(0, 0);
	fasttextcolor(TILE_INACTIVE);
	for (i = 0; i < MX; ++i) {
		tile = (char *) gr[pf[i] & 31];
		if (i == you) fasttextcolor(TILE_ACTIVE);
		fastcputs(tile, 4);
		if (i == you) fasttextcolor(TILE_INACTIVE);
	}
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

void move(char x, char y) {
	you = you + x + y * W;
}

unsigned char main_loop(void) {
	unsigned char k;
	while (1) {
		draw_screen();
		textcolor(TEXTCOLOR);
		gotoxy(0, H+1);
		printf(">");
		k = cgetc();
		switch (k) {
			case 'q':
				win();
				return 0;
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
