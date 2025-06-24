/* 
Módulo 2 - Exercício 12
[GPIO] Configuração de interrupções
*/

#include <msp430.h>

int main(void)
{

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

/*Operações com bit:
  //Bit Set:    |=    (OR)
  //Bit Clear:  &=~   (&!)
  //Bit Toggle: ^=    (XOR)
  */

/*Explicação GPIO  
  //GPIO do MSP430F5529
  //Pinos são agrupados em portas de 8 pinos - p.ex. P1.0 ~ P1.7

  //PxDIR - define se o pino é entrada ou saída
  //PxDIR = 0: Entrada  / PxDIR = 1: Saída

  //PxOUT - define o estado lógico de um pino configurado como saída 
  //      OU o tipo de resistor de um pino configurado como entrada
  //PxOUT = 0: 0V / PxOUT = 1: VCC  ; OUT = 0: Pull-down  / OUT = 1: Pull-up

  //PxIN - Lê o estado atual de um pino configurado como entrada
  //PxIN = 0: 0V /  PxIN = 1: VCC

  //PxREN - Habilita ou desabilita o resistor de pull-up/pull-down; sempre deve estar DESABILITADO se o pino for de saída (DIR = 1)
  //PxREN = 0: Resistor desabilitado  / PxREN = 1: Resistor habilitado. 

  //De acordo com o pino, deve então ser utilizado o bit correspondente. Por exemplo, para o pino P4.7, serão utilizados os registradores
  //do pino 4 (P4DIR, P4IN, P4OUT, P4REN) e o BIT7 
  */

//Configuração do LED vermelho (P1.0)
  P1DIR |= BIT0;   // Define P1.0 como saída
  P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
  P1OUT &= ~BIT0;  // Inicializa o led apagado

//Configuração do LED verde (P4.7)
  P4DIR |= BIT7;  // Define P4.7 como saída
  P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
  P4OUT &= ~BIT7; // Inicializa o LED verde como apagado- led apagado em baixa e acesso em alta
    
  
  //Configuração da chave S1 (P2.1) como entrada com resistor de pull-up com acionamento via interrupção
    P2DIR &= ~BIT1;  // Define P2.1 como entrada
    P2REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P2OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P2IE  |= BIT1;   // Habilita Interrupção
    P2IES |= BIT1;   // Interrupção na borda de descida
    P2IFG &= ~BIT1;  // Limpa a flag de interrupção

  //Configuração da chave S2 (P1.1) como entrada com resistor de pull-up com acionamento via interrupção
    P1DIR &= ~BIT1;  // Define P2.1 como entrada
    P1REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P1OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P1IE  |= BIT1;   // Habilita Interrupção
    P1IES |= BIT1;   // Interrupção na borda de descida
    P1IFG &= ~BIT1;  // Limpa a flag de interrupção
  
  __enable_interrupt();

  
  while(1){}                                  // superloop
}

// Rotina de serviço de interrupção para a Porta 1 (botão S2)
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    if (P1IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S2
        __delay_cycles(20000);   // Debounce
        if (!(P1IN & BIT1)) {    // Confirma o pressionamento
            P4OUT ^= BIT7;       // Alterna o estado do LED verde (toggle)
        }
        P1IFG &= ~BIT1;          // Limpa a flag de interrupção
    }
}

// Rotina de serviço de interrupção para a Porta 2 (botão S1)
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
    if (P2IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S1
        __delay_cycles(20000);   // Debounce
        if (!(P2IN & BIT1)) {    // Confirma o pressionamento
            P1OUT ^= BIT0;       // Alterna o estado do LED vermelho (toggle)
        }
        P2IFG &= ~BIT1;          // Limpa a flag de interrupção
    }
}

