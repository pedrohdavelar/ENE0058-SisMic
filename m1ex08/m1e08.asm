	.cdecls "msp430.h"
	.global main
	
	.text


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	call #fib16				;chama a subrotina
	jmp $					;loop infinito
    nop

fib16:				

	push R4				;salva R4, R5 e R6 na pilha	
	push R5				; R4 - fib(n-2); R5 - fib(n-1); R6 - fib(n)
	push R6

	mov.w #0x0, R4;		os dois primeiros numeros são inicializados como zero e um
	mov.w #0x1, R5;

proximo_fib:
	clr R6			
	add R4, R6
	add R5, R6
	jc 	fib16_max		;quando a soma gerar carry significa que houve estouro dos 16 bits

	mov R5, R4
	mov R6, R5
	jmp proximo_fib


fib16_max:
	mov R5, R12			;salva o maior numero de fibonacci encontrado em R12
	pop R6				;restaura o valor dos registros salvos na pilha
	pop R5
	pop R4
	ret					;fim da subrotina