	.cdecls "msp430.h"
	.global main
	
	.text


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	call #fib32				;chama a subrotina
	jmp $					;loop infinito
    nop

fib32:				

	push R4				;salva R4-R9 na pilha	
	push R5				; R4R5 - fib(n-2); R6R7 - fib(n-1); R8R9 - fib(n)
	push R6
	push R7
	push R8
	push R9



	mov.w #0x0, R5;		os dois primeiros numeros são inicializados como zero e um
	mov.w #0x0, R4
	mov.w #0x0, R7
	mov.w #0x1, R6

	mov.w #0x1, R10	; contador do # de iterações

proximo_fib:
	clr R8
	clr R9			
	
	add 	R4, R8		;soma da primeira word menos significativa
	add 	R6, R8		;soma da segunda word menos significativa

	addc 	R5, R9		;soma da primeira word mais significativa junto do carry
	add 	R7, R9		;soma da segunda word mais significativa

	jc 	fib32_max		;quando a soma gerar carry significa que houve estouro dos 32 bits

	mov R7, R5			;move fib(n-1) para fib(n-2)
	mov R6, R4

	mov R9, R7			; move fib(n) para fib(n-1)
	mov R8, R6

	inc R10
	jmp proximo_fib


fib32_max:
	mov R7, R13			;salva o maior numero de fibonacci encontrado em R13R12
	mov R6, R12
	
	pop R9				;restaura o valor dos registros salvos na pilha
	pop R8
	pop R7
	pop R6				
	pop R5
	pop R4
	ret					;fim da subrotina