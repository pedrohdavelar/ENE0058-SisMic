	.cdecls "msp430.h"
	.global main
	
	.text

	.data								;inicio da ram
vetorS: .byte  0,0,0,0,0,0,0,0,0,0
vetorA:	.byte 10,9,8,7,6,5,4,3,2,1		
vetorB: .byte  1,1,1,1,1,1,1,1,1,1


main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog
	
	mov #vetorS, R12
	mov #vetorA, R13
	mov #vetorB, R14
	mov #10, 	R15

	call #mapSub8			;chama a subrotina
	jmp $					;loop infinito
    nop

mapSub8:				

	push R4		; R4 vai armazenar cada elemento a ser computado. Os resultados serão computados diretamente no vetor S
	clr  R4

loop:

	mov.b 0(R13), R4 
	add.b R4, 0(R12) 

	mov.b 0(R14), R4
	sub.b R4, 0(R12)	; ao final, S tera sido acrescido pelo respectivo elemento de A e subtraido pelo respectivo elemento de B
	
	add #1, R12	;incremento dos ponteiros para apontar para o proximo byte dos vetores
	add #1, R13
	add #1, R14
	dec R15		;mais uma iteração concluida. Qdo esse decréscimo resultar em zero, o jump abaixo não será realizado
	jnz loop

	pop R4
	ret					;fim da subrotina