	.cdecls "msp430.h"
	.global main
	
	.text

main:
	mov.w	#(WDTPW|WDTHOLD), &WDTCTL

	mov.w 	#0xFFFF, R4		; Inicializa R4 = -1
	mov.w 	#0x4321, R5		; Inicializa R5 = 0x4321
	add.b	R4, R5			; Decrementa 1 e R5

	jmp		$
	nop
