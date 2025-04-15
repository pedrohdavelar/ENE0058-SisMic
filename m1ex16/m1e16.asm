	.cdecls "msp430.h"
	.global main
	
	.text

	.data
vetor: .byte 9,1,3,5,1,3,5,9,1,3

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	mov  #vetor, R12 ;passagem de parametros
	mov  #10,    R13

	call #menor			;chama a subrotina
	jmp $					;loop infinito
    nop

menor:

	push R4
	push R5
	push R6

	clr R6
	mov #0x00FF, R4 ; primeira comparação com 00FF para garantir que o numero sempre será menor ou igual

loop:

	mov.b 0(R12), R5
	cmp R4,R5
	jeq num_igual
	jl  num_menor
	jmp prox_loop

num_igual:
	inc R6		;apenas incremento o contador
	jmp prox_loop
num_menor:
	mov R5, R4	;atualiza o novo numero menor
	clr R6		;reseta o contador
	inc R6		;incremento o contador

prox_loop:
	dec R13				;se for zero nao ha mais numero para comparação
	jz fim_loop			
	add #1, R12			;R5 recebe o prox numero a ser comparado
	mov.b 0(R12), R5
	jmp loop

fim_loop
	mov R4, R12
	mov R6, R13
	pop R6
	pop R5
	pop R4
	ret
	


