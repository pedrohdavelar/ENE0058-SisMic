	.cdecls "msp430.h"
	.global main
	
	.text

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL
	
	mov.w	#0xFAF0, R4		;este valor será salvo na pilha
	mov.w	#0xCAFE, R5		;este também 
	mov.b	#0xFF, 	 R12	;operando 1
	mov.b	#0xFF,	 R13	;operando 2
	call #multiplica
	jmp $
    nop 

multiplica:
	push R4		;salva R4 na pilha
	push R5		;salva R5 na pilha
	mov.b R12, R4	;move R12 e R13 para R4 e R5
	mov.b R13, R5	;respectivamente, para a realização do calculo
	clr R12			;limpa R12 antes de comecar o calculo
	cmp #0x0000, R13	;precisamos saber se o 2o operador é zero
	jeq fim_multiplica	;se R13 for zero o resultado da multiplicação é zero


repete_soma:	;soma R5 vezes
	add R4, R12			;incrementa R12 pelo valor de R4
	dec	R5				;decrementa R5
	jnz repete_soma		;repete até R5 der zero
	jmp fim_multiplica	;qdo for, termina a multiplicação

fim_multiplica:
	pop R5				;busca o valor de R5 na pilha
	pop R4				;busca o valor de R4 na pilha
	ret



	