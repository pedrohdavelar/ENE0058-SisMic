	.cdecls "msp430.h"
	.global main
	
	.text

	.data
vetor: .word 20000,500,30000,100,30000

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	mov  #vetor, R12 ;passagem de parametros
	mov  #5,    R13

	call #maior16			;chama a subrotina
	jmp $					;loop infinito
    nop

maior16:

	push R4
	push R5
	push R6

	clr R6
	mov #0x0000, R4 ; primeira comparação com 0000 para garantir que o numero sempre será maior ou igual

loop:

	mov 0(R12), R5
	cmp R4,R5
	jeq num_igual
	jge num_maior
	jmp prox_loop

num_igual:
	inc R6		;apenas incremento o contador
	jmp prox_loop
num_maior:
	mov R5, R4	;atualiza o novo numero menor
	clr R6		;reseta o contador
	inc R6		;incremento o contador

prox_loop:
	dec R13				;se for zero nao ha mais numero para comparação
	jz fim_loop			
	add #2, R12			;R5 recebe o prox numero a ser comparado
	mov 0(R12), R5
	jmp loop

fim_loop
	mov R4, R12
	mov R6, R13
	pop R6
	pop R5
	pop R4
	ret
	


