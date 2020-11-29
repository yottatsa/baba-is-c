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
#include "game.h"
#include "const.h"

#define PROGRESS cputc(*".");


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

void draw_screen(void) {
	unsigned char i;
#ifdef __C64__
	register char *tile;
	register unsigned int char_mem = 0x0400;
	unsigned char j, pos = 0;
#else
	char *tile;
#endif

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
#ifndef __C64__
	gotoxy(0, 0);
#endif
	gotoxy(5, H+1);
	printf("%3d", you);
	for (i = 0; i < MX; ++i) {
		tile = (char *) gr[pf[i] & 31];
#ifndef __C64__
		if (i != 0 && i % W == 0) puts("");
		if (i == you) textcolor(YELLOW);
		printf(tile);
		if (i == you) textcolor(WHITE);
#else
		j = 0;
		draw_screen_loop: ;
		__asm__("ldy %v", j);
		__asm__("lda (%v),y", tile);
		__asm__("beq %g", draw_screen_loop_end);
		__asm__("ldy %v", pos);
		__asm__("sta (%v),y", char_mem);
		if (i == you) {
			__asm__("lda #$00");
			__asm__("ldy %v", pos);
			__asm__("sta $D800,y");
		}
		if (i != you) {
			__asm__("lda #$03");
			__asm__("ldy %v", pos);
			__asm__("sta $D800,y");
		}
		++pos;
		++j;
		if (pos == 0) char_mem = char_mem + 0x100;
		goto draw_screen_loop;

		draw_screen_loop_end: ;
#endif
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

unsigned char main_loop(void) {
	unsigned char k;
	while (1) {
		draw_screen();
		gotoxy(0, H+1);
		printf(">");
		k = cgetc();
		switch (k) {
			case 'q':
				win();
				return 0;
			case 'a':
				--you;
				break;
			case 'd': 
				++you;
				break;
			case 'w': 
				you = you - W;
				break;
			case 's': 
				you = you + W;
				break;
			default: ;;
		}
	}
	return 0;
}

int main (void) {
	unsigned char i;

	bgcolor(BLUE);
	textcolor(WHITE);

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
