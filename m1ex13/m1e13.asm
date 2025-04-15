	.cdecls "msp430.h"
	.global main
	
	.text

	.data								;inicio da ram
vetor1: .word   1,  2,   4,   8,  16,  32,   64,  128
vetor2:	.word 256,512,1024,2048,4096,8192,16384,32678		
vetorS: .word   0,  0,   0,   0,   0,   0,    0,    0


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	call #mapSum16			;chama a subrotina
	jmp $					;loop infinito
    nop

mapSum16:				

	push R4		; salva na pilha R4-R7
	push R5
	push R6
	push R7
	mov #vetor1, R5
	mov #vetor2, R6
	mov #vetorS, R7 
	mov #8, R4

loop:

	add 0(R5), 0(R7)
	add 0(R6), 0(R7)
	
	add #2, R5	;incremento dos ponteiros para apontar para o proximo elemento dos vetores
	add #2, R6
	add #2, R7
	dec R4		;mais uma iteração concluida. Qdo esse decréscimo resultar em zero, o jump abaixo não será realizado
	jnz loop

	pop R7
	pop R6
	pop R5
	pop R4
	ret					;fim da subrotina