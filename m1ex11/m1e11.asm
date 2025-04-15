	.cdecls "msp430.h"
	.global main
	
	.text

	.data								;inicio da ram
vetor:	.word 50000,50000,50000,50000,50000,50000,50000,50000,50000,50000		;vetor de words


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	mov #vetor, R12
	mov #10, 	R13

	call #reduceSum16				;chama a subrotina
	jmp $							;loop infinito
    nop

reduceSum16:				

	push R4		; R4 vai armazenar o elemento a ser somado do vetor
	push R5		; R6R5 vai armazenar a soma - R6 = MSWord, R5 = LSWord
	push R6
	clr  R4
	clr  R5
	clr  R6

loop_soma:

	mov 0(R12), R4 
	add R4,  R5
	addc #0, R6	;em caso de carry na soma das LSWords, incrementamos a MSWord com o carry
	add #2, R12	;como estamos trabalhando com words, o ponteiro de endereço do vetor precisa ser incrementado de dois em dois
	dec R13		;mais uma iteração concluida. Qdo esse decréscimo resultar em zero, o jump abaixo não será realizado
	jnz loop_soma

	mov R6, R13		; R13R12 - Soma realizada
	mov R5, R12
	pop R6			;retorno dos valores armazenados na pilha
	pop R5
	pop R4
	ret					;fim da subrotina