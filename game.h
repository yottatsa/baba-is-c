#define W 13
#define H 9
#define MX H*W
#define IS 31
#define YOU 0
#define WIN 1
#define STOP 2
#define PUSH 3
#define WATER 5

/* 
 * 1. 2 maps: 7 + transparency layers bitmap, and code
 * 2. your object is always ontop
 * 3. code is always push
 * 4. objects are driven by code
 */


#define EM 0

// layers
#define PL	1
#define RK	2
#define WL	3
#define FG	4

// noun
#define BA	9
#define RO	10
#define WA	11
#define FL	12

// adj/verb
#define YO	16
#define WI	17
#define ST	18
#define PU	19

unsigned char l1[] = {
	13,	9,	// W,H
//	01  02  03  04  05  06  07  08  09  10  11  12  13		
	BA, IS, YO, EM, EM, EM, EM, EM, EM, EM, FL, IS, WI,
	EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM,
	WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
	EM, EM, EM, EM, EM, EM, RK, EM, EM, EM, EM, EM, EM,
	EM, PL, EM, EM, EM, EM, RK, EM, EM, EM, EM, FG, EM,
	EM, EM, EM, EM, EM, EM, RK, EM, EM, EM, EM, EM, EM,
	WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL, WL,
	EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM, EM,
	WA, IS, ST, EM, EM, EM, EM, EM, EM, EM, RO, IS, PU,
};

char* levels[] = {
	// 9270 rem level data - use "d" to create
	l1,
};

#define ML 1
