	.cdecls "msp430.h"
	.global main
	
	.text

	.data
vetor: .word 10,20,-30,40,50,-60,-70,80,90,-100

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	mov  #vetor, r12 ;passagem de parametros
	mov  #10,    r13

	call #extremos			;chama a subrotina
	jmp $					;loop infinito
    nop

extremos:

	push r4		;elemento n-1
	push r5		;elemento n
	push r6		;elemento menor
	push r7		;elemento maior

	mov 0(r12), r4	;inicizalizando com o elemento n-1
	mov r4, r6		;o qual automaticamente seré o menor
	mov r4, r7		;e o maior no inicio da iteração pelo vetor

	dec r13			;já iteramos um elemento
	jz	fim_loop	;se o vetor só tiver um elemento então acabou
	add #2, r12		;ponteiro r12 já comeca o loop apontando para o 2o elemento

loop:

	mov 0(r12), r5
	cmp r4,r5
	jl  num_menor
	jge num_maior
	jmp prox_loop

num_menor:
	mov r5, r6	;atualiza o menor numero
	jmp prox_loop
num_maior:
	mov r5, r7	;atualiza o maior numero

prox_loop:
	dec R13				;se for zero nao ha mais numero para comparação
	jz fim_loop			
	mov r5, r4			;r4 recebe o numero atual
	add #2, R12			;r5 recebe o prox numero a ser comparado
	mov 0(R12), r5
	jmp loop

fim_loop
	mov r6, r12
	mov r7, r13
	pop r7
	pop r6
	pop r5
	pop r4
	ret
	


