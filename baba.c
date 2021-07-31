/*
 * baba-is-c -- a demake -- by yottatsa 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resources.h"

#include "fastconio.h"
#include "game.h"
#include "animation.h"
#include "graphic.h"


// 25 dim pf%(mx):rem playfield map
unsigned char pf[MX], pfbm[MX];
unsigned char props[16];
unsigned char objs[8];
unsigned char p_you, p_stop, p_win, p_push, is_alive, is_won;
unsigned char left, top, right, width, height;
unsigned char sprite_codes[32];
#define is_you(v) (v & p_you)
#define is_stop(v) (v & p_stop)
#define is_win(v) (v & p_win)
#define is_push(v) (v & p_push)


void unpack_level(unsigned char *lv) {
	unsigned char i, tile;

	width = *(lv++);
	height = *(lv++);

	memset(pf, 0, MX);
	memset(pfbm, 0, MX);

	left = SCREEN_WIDTH / 2 - width / 2;
	right = left + width;
	top = SCREEN_HEIGHT / 2 - height / 2;

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

	x = left; y = top;
	fastgotoxy(x, y);
	for (i = 0; i < MX; ++i) {
		if (x == right) {
			x = left;
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
	gotoxy(0, 0);
	printf("congratulations. press any for next\n");
	cgetc();
}

void lose(void) {
	is_alive = 0;
	gotoxy(0, 0);
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

int main (void) {
	unsigned char i;

	fastbgcolor(BGCOLOR);
	textcolor(TEXTCOLOR);

	for (i = 0; i < 32; ++i) {
		sprite_codes[i] = (unsigned char) i + UDG_SHIFT;
#ifdef __C64__
		sprite_codes[i] = bgcolormask(sprites_colors[i + i + 1], sprite_codes[i]);
#endif
	}

#if defined(__C64__) && defined(GRAPHIC)
	chargena = &_CHARSETA_START__;
	set_graphic_mode();

	memcpy(chargena + 32 * 8, sprites, 256);
#endif

#if defined(__C64__) && defined(GRAPHIC) && defined(ANIMATION)
	chargenb = &_CHARSETB_START__;

	memcpy(chargenb, chargena, 512); 
	for (i = 0; i < 255; ++i) {
		if (p1[i] != FF) {
			*(chargenb + 32 * 8 + i) = p1[i];
		}
	}
	set_animation();
#endif

#if defined(__SPECTRUM__) && defined(GRAPHIC)
	set_graphic_mode();
#endif

	for (i = 0; i < ML; ++i) {
		do {
			clrscr();
			printf("loading level %d", i+1);
			unpack_level(levels[i]);
			clrscr();
		} while (main_loop() != 0);
	}
	//return EXIT_SUCCESS;
	return 0;
}
