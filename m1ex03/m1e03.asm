	.cdecls "msp430.h"
	.global main
	
	.text

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL
	
	;mov.w	#0xFFFF, R4		;essa soma vai gerar carry
	mov.w	#0x000F, R4		;essa soma não vai gerar carry
	mov.w	#0x0001, R5
	add		R4, R5
	jnc		sem_carry
	mov.w	#0xFFFF, R5


sem_carry: 


	jmp $
    nop 