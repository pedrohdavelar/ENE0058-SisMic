	.cdecls "msp430.h"
	.global main
	
	.text

	.data								;inicio da ram
vetor1: .byte   0,1,2,3,4,5,6,7,8,9

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	mov  #vetor1, R12 ;passagem de parametros
	mov  #10,     R13

	call #m2m4			;chama a subrotina
	jmp $					;loop infinito
    nop

m2m4:				

	push R4	;numero de multiplos de 2
	push R5	;numero de multiplos de 4
	clr R4
	clr R5

loop:	
	
	bit.b #0x01, 0(R12)	;verifica se o numero é multiplo de 2
	jnc m2				;se o carry não ativar é pq eh multiplo de 2
	jmp prox_loop


m2:
	inc R4
	bit #0x03, 0(R12)	;verifica se o número é multiplo de 4
	jnc	m4				; se o carry não ativar é pq eh multiplo de 4
	jmp prox_loop

m4:
	inc R5

prox_loop:
	add #1, R12
	dec R13
	jnz loop

	mov R4, R12
	mov R5, R13
	pop R5
	pop R4
	
	ret					;fim da subrotina