	.cdecls "msp430.h"
	.global main
	
	.text

	.data
vetor: .byte 0,0,0,0

main:

	mov.w 	#(WDTPW|WDTHOLD), &WDTCTL	; desativa o watchdog

	mov  #vetor, r13 ;passagem de parametros
	mov  #0xC4F3,r12

	call #W16_ASC			;chama a subrotina
	jmp $					;loop infinito
    nop

W16_ASC:

	push r4
	push r5
	push r6	

	mov r12, r4
	mov r13, r5
	add #3,  r5		
	
	and #0x000F, r4		;isola ultimo nibble
	mov  r4, r14			
	call #NIB_ASC
	mov.b  r14, 0(r5)
	dec r5
	mov  #4, r6
	call #shift_4
	mov  r12, r4
	
	and  #0x000F, r4	;isola o 3o nibble
	mov  r4, r14
	call #NIB_ASC
	mov.b  r14, 0(r5)
	dec r5
	mov #4, r6
	call #shift_4
	mov  r12, r4
	
	and  #0x000F, r4	;isola o 2o nibble
	mov  r4, r14
	call #NIB_ASC
	mov.b  r14, 0(r5)
	dec r5
	mov #4, r6
	call #shift_4
	mov  r12, r4
	
	and  #0x000F, r4	;isola o 1o nibble
	mov  r4, r14
	call #NIB_ASC
	mov.b  r14, 0(r5)	

	pop r6
	pop r5
	pop r4
	ret

shift_4:
shift_loop:
	rra r12				;desloca 4x para direita o r12
	dec r6
	jnz shift_loop
	ret
	
NIB_ASC:
	cmp #10, r14	
	jlo digito		;se for menor que 10 é pq r12 tem valor entre 0 e 9; se não tem valor entre A e F
	add #0x07, r14	;os numeros na tabela ascii comecam em 0x30; as letras em 0x37; adicionando esses valores a r12 faremos a conversão do nibble para o respectivo caractere da tabela ascii
digito:
	add #0x30, r14	;se for um digito, adiciona apenas 0x30; se for uma letra, ira adicionar 0x07 e 0x30 -> 0x37
	ret

