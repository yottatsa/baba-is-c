// Fast conio
#include <conio.h>

#ifndef _FASTCONIO_H
#define _FASTCONIO_H

// Commodore-specific implementation
#define SCREEN_WIDTH 40

#ifdef __C64__

// C64
#include <c64.h>
#define TEXT_RAM       ((unsigned char*)0x0400)
#define BGCOLOR        ((unsigned char*)0xD021)

unsigned char *_textptr;
unsigned char *_colorptr;
unsigned char *_bgcolorptr = BGCOLOR;
unsigned char _fgcolor, _bgcolor;
#define fastgotoxy(x, y) _textptr = TEXT_RAM + x + y * SCREEN_WIDTH; _colorptr = COLOR_RAM + x + y * SCREEN_WIDTH;
#define fasttextcolor(c) _fgcolor = c; textcolor(c);
#define fastbgcolor(c) *(_bgcolorptr) = c;
#define fastcputs(s, l) { \
	unsigned char i; \
	for (i = 0; i < l; ++i) { \
		*(_textptr++) = s[i]; \
		*(_colorptr++) = _fgcolor; \
	} \
}

#define YELLOW COLOR_YELLOW
#define WHITE COLOR_WHITE
#define DARKGRAY COLOR_GRAY1
#define BLACK COLOR_BLACK

#endif


// ANSI C implementation
#ifndef __C64__
#define fastgotoxy(x, y) gotoxy(x, y)
#define fasttextcolor(c) textcolor(c);
#define fastbgcolor(c) bgcolor(c);
#define fastcputs(s, l) { \
	unsigned int w = SCREEN_WIDTH - wherex(); \
	printf("%.*s%s", w, s, (l >= w) ? "\n" : ""); \
}

#endif

#endif
