	.cdecls "msp430.h"
	.global main
	
	.text

	.data								;inicio da ram
vetor:	.byte 100,100,100,100,100,100,100,100,100,100		;vetor de bytes


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	mov #vetor, R12
	mov #10, 	R13

	call #reduceSum8				;chama a subrotina
	jmp $							;loop infinito
    nop

reduceSum8:				

	push R4
	push R5
	clr  R4
	clr  R5

loop_soma:

	mov.b 0(R12), R4 
	add R4, R5
	add #1, R12
	dec R13
	jnz loop_soma

	mov R5, R12
	pop R5
	pop R4
	ret					;fim da subrotina