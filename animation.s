; https://www.c64-wiki.com/wiki/Raster_interrupt
.export _set_interrupt
.import _animate

_set_interrupt:
	LDA #<_irq            ; set interrupt vectors, pointing to interrupt service routine below
	STA $0314
	LDA #>_irq
	STA $0315
	RTS

_irq:       
	JSR _animate
	ASL $D019            ; acknowledge the interrupt by clearing the VIC's interrupt flag
	JMP $EA31            ; jump into KERNAL's standard interrupt service routine to handle keyboard scan, cursor display etc.

