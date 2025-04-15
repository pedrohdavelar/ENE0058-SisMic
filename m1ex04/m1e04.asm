	.cdecls "msp430.h"
	.global main
	
	.text

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL
	
	mov.w	#0x0005, R4		;R4 será somado
	mov.w	#0x0003, R5		;R5 vezes 
	clr		R13				;zerando R13 para armazenar o resultado 
repete_soma:
	add		R4, R13
	dec		R5
	jnz		repete_soma

	jmp $
    nop 