#include <conio.h>
#include "binliteral.h"

#ifndef _FASTCONIO_H
#define _FASTCONIO_H


// Commodore-specific implementation
#if defined(__C64__)

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25


// C64
#include <c64.h>
#include <6502.h>
#define TEXT_RAM       ((unsigned char*)0x0400)
#define UDG_SHIFT	32


unsigned char *_textptr;
unsigned char *_colorptr;
unsigned char _fgcolor, _bgcolor;
#define fastgotoxy(x, y) _textptr = TEXT_RAM + x + y * SCREEN_WIDTH; _colorptr = COLOR_RAM + x + y * SCREEN_WIDTH;
#define fasttextcolor(c) _fgcolor = c;
#define fastbgcolor(c) VIC.bgcolor0 = c; // for stdchr
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

#else

#define ORANGE		RED

#endif


// z88dk
#if defined(__SPECTRUM__)
#include <spectrum.h>
#define UDG_SHIFT	128

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 24

#define fastgotoxy(x, y) gotoxy(x, y)
#define fasttextcolor(c) textcolor(c);
#define fastbgcolor(c) textbackground(c); zx_border(c);
#define fastcputc(c) putch(*(c));


#endif


// ANSI C implementation
#if !defined(__C64__) && !defined(__SPECTRUM__)

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

#define cgetc getch
#define fastgotoxy(x, y) gotoxy(x, y)
#define fasttextcolor(c) textcolor(c);
#define fastbgcolor(c) textbackground(c);
#define fastcputc(c) putch(*(c));


#endif


#endif
