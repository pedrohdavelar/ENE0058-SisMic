	.cdecls "msp430.h"
	.global main
	
	.text

	.data				;.data define o inicio da RAM no endereço de memoria 0x2400
fib20: .word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 	;reserva 40 bytes ou 20 words para armazenar os primeiros 20 numeros da sequencia de fibonacci - .bss preenche a area de memoria com zeros

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	mov #0x0014 , R15		;R15 recebe a qtde de #s fibonnaci a ser calculada/armazenada
	mov #fib20 , R14		;R14 recebe o endereço inicial de memoria do vetor fib20 onde serão armazenados os números
	call #fibonacci			;chama a subrotina
	jmp $					;loop infinito
    nop

fibonacci:				

	push R4				;salva R4, R5 e R6 na pilha	
	push R5				; R4 - fib(n-2); R5 - fib(n-1); R6 - fib(n)
	push R6

	mov.w #0x0, R4;		os dois primeiros numeros são inicializados como zero e um
	mov.w #0x1, R5;

	mov R4, 0(R14)		;salva fib(0) no primeiro endereço do vetor
	add #2, R14			;incrementa o endereço por dois bytes
	mov R5, 0(R14)		;salva fib(1) no segundo endereço do vetor
	add #2, R14			;incrementa o endereço por dois bytes
	sub #2, R15			;decresce os dois primeiros numeros calculados de R15

	
next_fibonacci:			;calcula o próximo número da série
	clr R6 				;reseta o valor de R6 antes de somar
	add R4, R6			
	add R5, R6			;adiciona-se a R6 os dois numeros anteriores da sequencia.
	mov R6, 0(R14)		;salva o valor no vetor
	add #2, R14			;aumenta o endereço para a proxima word

	mov R5, R4			;prepara os registros para a proxima iteração
	mov R6, R5

	dec R15				;mais um numero da sequencia calculado
	jnz next_fibonacci	;itera pelo loop enquanto R15 > 0
	
	pop R6				;restaura o valor dos registros salvos na pilha
	pop R5
	pop R4
	ret					;fim da subrotina