#define W 10
#define H 12
#define MX H*W
#define IS 31
#define YOU 0
#define WIN 1
#define STOP 2
#define PUSH 3

/* 
 * 1. 2 maps: 7 + transparency layers bitmap, and code
 * 2. your object is always ontop
 * 3. code is always push
 * 4. objects are driven by code
 */

// 40 dim gr$(31):rem gfx tiles 4x1
static char gr[32][5] = {
	"....", ".ZZ.",
	"****", "====",
	"FLAG", "////",
	"WWWW", "O -P",
	// 9050 rem names 8-15
	"  8 ", "baba",
	"rock", "wall",
	"flag", "watr",
	"key ", "door",
	// 9100 rem properties/verbs 16-31
	// 9110 rem property is ru%(obj) and...
	"you ",
	"win!",
	"stop",
	"push",
	"shut",
	"open",
	"sink",
	"lose",
	"hot ",
	"melt",
	" 26 ",
	" 27 ",
	" 28 ",
	" 29 ",
	"has ", " is ",
};

char l1[] = {42, 201, 80, 33, 203, 82, 43, 67, 55, 33, 69, 51, 66, 69, 35, 69, 65, 69, 49, 66, 69, 49, 68, 33, 69, 51, 66, 69, 35, 67, 55, 43, 204, 81, 33, 202, 83, 0};
char* levels[] = {
	// 9270 rem level data - use "d" to create
	l1,
};

#define ML 1
