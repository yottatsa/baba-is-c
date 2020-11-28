#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "game.h"
#include "const.h"

#define PROGRESS cputc(*".");


// 25 dim pf%(mx):rem playfield map
unsigned char pf[MX];	


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
	unsigned char i, pos = 0;
#ifdef __C64__
	register char *tile;
	register unsigned int offset = 0x0400;
	unsigned char j;
#else
	char *tile;
#endif

	// 905 print"{home}";:for n=0tomx:printgr$(pf%(n)and31);:next n
#ifndef __C64__
	gotoxy(0, 0);
#endif
	for (i = 0; i < MX; ++i) {
		tile = (char *) gr[pf[i] & 31];
#ifndef __C64__
		if (i != 0 && i % W == 0) puts("");
		printf(tile);
#else
		j = 0;
		draw_screen_loop: ;
		__asm__("ldy %v", j);
		__asm__("lda (%v),y", tile);
		__asm__("beq %g", draw_screen_loop_end);
		__asm__("ldy %v", pos);
		__asm__("sta (%v),y", offset);
		++pos;
		++j;
		if (pos == 0) offset = offset + 0x100;
		goto draw_screen_loop;

		draw_screen_loop_end: ;
#endif
	}
}

int main (void) {
	unsigned char i;

	bgcolor(BLUE);
	textcolor(WHITE);

	for (i = 0; i < ML; ++i) {
		clrscr();
		printf("Loading level %d", i+1);
		unpack_level(levels[i]);
		clrscr();
		draw_screen();
		gotoxy(0, H+2);
		printf("done");
		cgetc();
	}
	//clrscr();
	return EXIT_SUCCESS;
}
