#include <conio.h>
#include "binliteral.h"

#ifndef _FASTCONIO_H
#define _FASTCONIO_H

// Commodore-specific implementation
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

#ifdef __C64__

// C64
#include <c64.h>
#define TEXT_RAM       ((unsigned char*)0x0400)

unsigned char *_textptr;
unsigned char *_colorptr;
unsigned char _fgcolor, _bgcolor;
#define fastgotoxy(x, y) _textptr = TEXT_RAM + x + y * SCREEN_WIDTH; _colorptr = COLOR_RAM + x + y * SCREEN_WIDTH;
#define fasttextcolor(c) _fgcolor = c;
#define fastbgcolor(c) VIC.bgcolor0 = c; // for stdchr
#define fastcputs(s, l) { \
	unsigned char i; \
	for (i = 0; i < l; ++i) { \
		*(_textptr++) = *(s++); \
		*(_colorptr++) = _fgcolor; \
	} \
}
#define fastcputc(c) \
	*(_textptr++) = *(c); \
	*(_colorptr++) = _fgcolor;

#define bgcolormask(m, c) ((c & BYTE(0011,1111)) | m)

#define BLACK		COLOR_BLACK
#define WHITE		COLOR_WHITE
#define RED		COLOR_RED
#define CYAN		COLOR_CYAN
//#define COLOR_VIOLET
//#define COLOR_PURPLE
#define GREEN		COLOR_GREEN
#define BLUE		COLOR_BLUE
#define YELLOW		COLOR_YELLOW
#define ORANGE		COLOR_ORANGE
#define BROWN		COLOR_BROWN
//#define COLOR_LIGHTRED
#define DARKGRAY	COLOR_GRAY1
//#define COLOR_GRAY2
//#define COLOR_LIGHTGREEN
#define LIGHTBLUE	COLOR_LIGHTBLUE
#define LIGHTGRAY	COLOR_GRAY3

#define BGCOLORMASK0	BYTE(0000,0000)
#define BGCOLORMASK1	BYTE(0100,0000)
#define BGCOLORMASK2	BYTE(1000,0000)
#define BGCOLORMASK3	BYTE(1100,0000)

#endif


// ANSI C implementation
#ifndef __C64__
#define fastgotoxy(x, y) gotoxy(x, y)
#define fasttextcolor(c) textcolor(c);
#define fastbgcolor(c) bgcolor(c);
#define fastcputs(s, l) { \
	unsigned int x = wherex(); \
	printf("%s%.*s", (x >= SCREEN_WIDTH) ? "\n" : "", l, s); \
}
#define fastcputc(c) cputc(*(c));

#define ORANGE		RED
#endif

#endif
