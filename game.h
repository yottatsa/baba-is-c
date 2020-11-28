#define W 10
#define H 18
#define MX H*W
#define IS 31

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
	//  9110 rem property is ru%(obj) and...
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
	/*
	// 9000 rem objs 0-7
	"{blk}....","{wht}.ZZ.",
	"{gry3}{CBM-D}{rvon}{CBM-Y}{CBM-Y}{rvof}{CBM-F}","{orng}{rvon}{CBM-R}{CBM-E}{CBM-R}{CBM-E}{rvof}",
	"{yel} N{CBM-U} ","{lblu}////",
	"{yel} W{CBM-R} ","{rvon}{red}O -P{rvof}",
	// 9050 rem names 8-15
	"{red}{rvon}  8 ","{wht}{rvof}baba",
	"{wht}{rvof}rock","{wht}{rvof}wall",
	"{wht}{rvof}flag","{wht}{rvof}watr",
	"{wht}{rvof}key ","{wht}{rvof}door",
	// 9100 rem properties/verbs 16-31
	// 9110 rem property is ru%(obj) and...
	"{cyn}{rvon}you {rvof}", //:rem 1
	"{cyn}{rvon}win!{rvof}", //:rem 2
	"{cyn}{rvon}stop{rvof}", //:rem 4
	"{cyn}{rvon}push{rvof}", //:rem 8
	"{cyn}{rvon}shut{rvof}", //:rem 16
	"{cyn}{rvon}open{rvof}", //:rem 32
	"{cyn}{rvon}sink{rvof}", //:rem 64
	"{cyn}{rvon}lose{rvof}", //:rem 128
	"{cyn}{rvon}hot {rvof}", //:rem 256
	"{cyn}{rvon}melt{rvof}", //:rem 512
	"{cyn}{rvon} 26 {rvof}", //:rem 1024
	"{cyn}{rvon} 27 {rvof}", //:rem 2048
	"{cyn}{rvon} 28 {rvof}", //:rem 4096
	"{cyn}{rvon} 29 {rvof}", //:rem 8192
	"{wht}{rvof}has","{wht}{rvof} is",
	*/
};

char l1[] = {42, 201, 80, 33, 203, 82, 43, 67, 55, 33, 69, 51, 66, 69, 35, 69, 65, 69, 49, 66, 69, 49, 68, 33, 69, 51, 66, 69, 35, 67, 55, 43, 204, 81, 33, 202, 83, 0};
char* levels[] = {
	// 9270 rem level data - use "d" to create
	l1,
};

#define ML 1
