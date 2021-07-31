#if defined(__C64__) && defined(GRAPHIC)
unsigned char *chargena;
extern unsigned char _CHARSETA_START__;
#endif

#if defined(__C64__) && defined(GRAPHIC) && defined(ANIMATION)
unsigned char *chargenb;
extern unsigned char _CHARSETB_START__;
//unsigned char *chargenc;
//extern unsigned char _CHARSETC_START__;
//unsigned char *chargend;
// extern unsigned char _CHARSETD_START__;
//#define _CHARSETD_START__ *((unsigned char*)0x1800);
#endif

#if defined(__C64__) && defined(GRAPHIC)
#define PROCESSOR_PORT				*((unsigned char*)0x01)
#define PROCESSOR_PORT_IOCHAR_VISIBILITY	0x04
#define CIA1_CRA_TIMER				0x01

void set_graphic_mode(void) {
	unsigned char *origchargen = (void*)0xd000;

	/* https://www.c64-wiki.com/wiki/CIA
	 * Turn off interrupts
	 */
	CIA1.cra = CIA1.cra & ~CIA1_CRA_TIMER;

	/* https://sta.c64.org/cbm64mem.html
	 * Make Character ROM visible at $D000-$DFFF.
	 */
	PROCESSOR_PORT = PROCESSOR_PORT & ~PROCESSOR_PORT_IOCHAR_VISIBILITY;

	// Copy character set from ROM to RAM
	memcpy(chargena, origchargen, 512); 

	/* https://sta.c64.org/cbm64mem.html
	 * Restore I/O area visible at $D000-$DFFF.
	 */
	PROCESSOR_PORT = PROCESSOR_PORT | PROCESSOR_PORT_IOCHAR_VISIBILITY;
	
	// Turn on interrupts
	CIA1.cra = CIA1.cra | CIA1_CRA_TIMER;

	/* https://www.c64-wiki.com/wiki/Standard_Character_Mode
	 * In Control Register 1 ($d011) bit 6 (ECM) must be cleared and bit 5 (BMM) must be cleared. In Control Register 2 ($d016), bit 4 (MCM) must be cleared. 
	 */
	// ECM
	VIC.ctrl2 = VIC.ctrl2 & ~(1 << 4);
	VIC.ctrl1 = (VIC.ctrl1 & ~(1 << 5)) | (1 << 6);

	VIC.bgcolor1 = BGCOLOR1;
	VIC.bgcolor2 = BGCOLOR2;
	VIC.bgcolor3 = BGCOLOR3;

	/* https://www.c64-wiki.com/wiki/53272 https://sta.c64.org/cbm64mem.html
	 * When in text screen mode, the VIC-II looks to 53272 for information on where the character set and text screen character RAM is located:
	 * - The four most significant bits form a 4-bit number in the range 0 thru 15: Multiplied with 1024 this gives the start address for the screen character RAM.
	 * - Bits 1 thru 3 (weights 2 thru 8) form a 3-bit number in the range 0 thru 7: Multiplied with 2048 this gives the start address for the character set.
	 * Notice that all the start addresses of character sets, screen character RAM, bitmaps, and color information, are all relative to the start address of the current VIC bank!.
	 */

	VIC.addr = (VIC.addr & BYTE(1111,0000)) | BYTE(0000,1110);
}
#endif

#ifdef __SPECTRUM__
#include <sys/ioctl.h>                // required for querying system capabilites and for switching screen modes

void set_graphic_mode(void) {
	void *param = &sprites;
	console_ioctl(IOCTL_GENCON_SET_UDGS, &param);
        
        // On the ZX Spectrum, we switch to 32 column mode
        #if defined(__SPECTRUM__)
                printf("%c%c",1,32);
        #endif
}
#endif
