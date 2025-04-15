	.cdecls "msp430.h"
	.global main
	
	.text

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL
	
	mov.w	#0xFFF0, R4		
	mov.w	#0x0001, R5		

	add 	R4, R5

	jn 		r_negativo	; jump se ativar a flag N
	;jnz		r_positivo  ; jump se NÃO ativar a flag N & NÃO ativar a flag zero
	jz	    fim         ; 3o caso - faz nada

r_positivo:

	add 	#1, R5
	jmp 	fim

r_negativo:

	sub 	#1, R5

fim:
	    jmp $	; loop infinito
    	nop