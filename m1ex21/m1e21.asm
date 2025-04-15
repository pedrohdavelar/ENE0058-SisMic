	.cdecls "msp430.h"
	.global main
	
	.text

	.data
vetor: 
	.byte 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240	;pedi o Qwen Chat para gerar esse vetor mas o restante do código fui eu que fiz!
    .byte 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224
    .byte 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208
    .byte 207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192
    .byte 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176
    .byte 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160
    .byte 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144
    .byte 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128
    .byte 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112
    .byte 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96
    .byte 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80
    .byte 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64
    .byte 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48
    .byte 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32
    .byte 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16
    .byte 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	mov #vetor, r12	;passagem de parametros
	mov #256, r13

	call #bubble_sort		;chama a subrotina
	jmp $					;loop infinito
    nop

bubble_sort:			

	push r4	; elemento n-1	
	push r5	; elemento n
	push r6	; swap
	push r7	; max elementos a iterar
	push r8	; iteração atual
	push r9	; endereço elemento n

	mov r13, r7 ; para um vetor com n elementos, cada iteração faz no máximo n-1 swaps;
	dec r7		; por isso esse decremento de r7. os prómimos decrementos ocorrerão no final do loop para determinar se deve ser feita uma nova iteração do vetor

pre_loop:			;procedimentos previos a cada iteração pelo vetor
	
	mov  #0, r8		;a cada iteração resetamos esse contador
	mov r12, r9		;e o ponteiro dos elementos iterados
	mov.b 0(r9), r4	;e já inicializamos o elemento n-1

loop:				;iteração pelo vetor
	inc r9			;a cada loop aumentamos o ponteiro para pegar o proximo elemento
	mov.b 0(r9), r5	;e o salvamos em r5
	cmp r5,r4		;se o elemento n-1 for menor que o elemento n, não é necessário fazer o swap
	jl no_swap		;caso contrário, fazemos então a troca de posições	
	mov.b r4,  0(r9)	; r9 aponta para o elemento n; trocamos seu valor pelo elemento n-1
	mov.b r5, -1(r9)	; e trocamos o valor do elemento n-1 pelo elemento n
	jmp swap
no_swap:
	mov.b r5, r4	;repassamos então o elemento n para n-1 se nao houve swap
swap:				;se houve entao esse repasse ja foi feito
	inc r8			;e finalizamos uma iteração
	cmp r7, r8		;se o # da iteração for menor que o # max de iterações, repetimos então
	jl  loop
	dec r7			;se o vetor tem n elementos, o max de swaps a fazer é n-1. a cada iteração o vetor a ser organizado diminui em 1
	jnz pre_loop	;se ainda restar elementos, repetir o pré loop e mais um loop completo

	pop R9			;restaura o valor dos registros salvos na pilha
	pop R8
	pop R7
	pop R6				
	pop R5
	pop R4
	ret					;fim da subrotina