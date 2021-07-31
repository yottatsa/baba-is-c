#if defined(__C64__) && defined(GRAPHIC) && defined(ANIMATION)
// from animation.s
void set_interrupt(void);

unsigned char tick = 0;
extern void animate(void) 
{ 
	switch (tick)
	{
		case 0:
			VIC.addr = (VIC.addr & BYTE(1111,0000)) | BYTE(0000,1110);
			break;
		case 12:
			VIC.addr = (VIC.addr & BYTE(1111,0000)) | BYTE(0000,1100);
			break;
		case 24:
			VIC.addr = (VIC.addr & BYTE(1111,0000)) | BYTE(0000,1110);
			break;
		case 35:
			VIC.addr = (VIC.addr & BYTE(1111,0000)) | BYTE(0000,1100);
			break;
	}
	if (tick == 0) tick = 50;
	tick--; 
} 

void set_animation(void)
{
	/* https://sta.c64.org/cbm64mem.html https://www.c64-wiki.com/wiki/Raster_interrupt
	 */
	SEI(); 
	CIA1.icr = BYTE(0111,1111);  // to fix
	VIC.ctrl1 = VIC.ctrl1 & BYTE(0111,1111);
	__asm__("lda $DCOD");
	__asm__("lda $DDOD");
	VIC.rasterline = 210;
	set_interrupt();
	VIC.imr = BYTE(0000,0001);
	CLI(); 
}
#endif
