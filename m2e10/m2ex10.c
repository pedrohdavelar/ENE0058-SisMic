/* 
Módulo 2 - Exercício 10
PWM Variável
*/

#define CCR0      8191      //período para 128Hz
#define STEP_SIZE CCR0 / 8  //passos de ajuste do duty cycle

volatile unsigned int dutyCycle = CCR0 / 2; //Duty cycle original de 50%

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

  //Configuração do LED vermelho ligado ao pino P1.2(PWM)
    P1DIR |= BIT2;   // Define P1.2 como saída
    P1SEL |= BIT2;    // Função adicional do pino 1.2 para TA0.1
    
  
  //Configuração da chave S1 (P2.1) como entrada com resistor de pull-up
    P2DIR &= ~BIT1;  // Define P2.1 como entrada
    P2REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P2OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P2IE  |= BIT1;   // Habilita Interrupção
    P2IES |= BIT1;   // Interrupção na borda de descida
    P2IFG &= ~BIT1;  // Limpa a flag de interrupção

  //Configuração da chave S2 (P1.1) como entrada com resistor de pull-up
    P1DIR &= ~BIT1;  // Define P2.1 como entrada
    P1REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P1OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P1IE  |= BIT1;   // Habilita Interrupção
    P1IES |= BIT1;   // Interrupção na borda de descida
    P1IFG &= ~BIT1;  // Limpa a flag de interrupção
  
/*Explicação Timer - Cap 7 Apostila
  //Timer A - 3 Instâncias (TA0, TA1, TA2)
  //Timer B - 1 Instância (TB0) 
  //Cada instância possui um contador de 16 bits - TA0R, TA1R, TA2R, TB0R
  //E varios registradores de comparação e captura (CC) - TAxCCNn e TAxCCTLn - x -> instância do timer; n -> instância da unidade de captura e comparação
  //Modos de contagem: Modo 0 - Parado; Modo 1 - Ascendente; Modo 2 - Contínuo; Modo 3 - Up/Down
  //Gerar saídas PWM: Cada unidade de captura e compara de cada timer pode selecionar um mecanismo para gerar saídas
  //Flags: TAIFG -> Vai a 1 toda vez que o contador passar pelo zero; CCIFG -> vai a 1 toda vez que o valor do contador coincidir com o valor do registrador de comparação
  */

  
  /*Configuração de TA0

  TAxCTL - controla o modo do timer
  
  TASSEL_x -> Fonte do clock: 
  0 TCLK  (Clock externo) 
  1 ACLK  (Auxiliary clock, 32768 Hz) 
  2 SMCLK (Sub main clock, 1MHz) (na verdade é 1.048576MHzou 1048576Hz)
  3 INCLK (Clock externo)

  ID_x -> Divisor de clock
  0 Sem divisão
  1 Divide por 2
  2 Divide por 4
  3 Divide por 8 

  MC_x Modo de contagem
  0 Parado
  1 Up (Conta de 0 até TAxCCR0)
  2 Continuous (Conta até 0xFFFF)
  3 Up/Down (conta até TAxCCR0 e decrementa até 0)

  TAIFG -> Contador "estourou" (passou por TAxCCR0) - Define o período
  CCIFG -> Contador passou por TAxCCR1              - Define o duty cycle

  TAIFG<------------><------------><------------><------------>
  CCIFG<------><....><------><....><------><....><------><....>

  Contagem = fClk/fDesejada => 1048576/128 = 8192
  como a contagem começa do zero, subtraímos 1 => 7812 - 1 => TA0CCR0 = 7812 -1

  como o duty cycle é de 50%, TA0CCR1 = TA0CCR0/2

  */
  TA0CTL = TASSEL_2 | ID_0 | MC_1 | TACLR; // SMCLK, sem divisor, modo up, limpa o timer
  TA0CCR0 = CCR0;
  TA0CCR1 = dutyCycle;

  //Modo 7 para o canal 1 do timer 0 (TA0.1)
  //define o comportamento do pino para Reset/Set: Quando o timer atinge TAxCCR1 o pino é resetado (vai para 0); quando atinge TAxCCR0 ele é setado (vai para 1)
  //ou seja, PWM com frequencia - TAxCCR0 e Duty Cycle = TAxCCR1/TAxCCR0  
  //111111111111111100000000000000111111111111111100000000000000111111111111111100000000000000
  //<---TAxCCR1---->              <---TAxCCR1---->              <---TAxCCR1---->              
  //<--------TAxCCR0-------------><--------TAxCCR0-------------><--------TAxCCR0------------->
  TA0CCTL1 = OUTMOD_7;

  __enable_interrupt();

  
  while(1){}                                  // superloop
}

// Rotina de serviço de interrupção para a Porta 1 (botão S2)
#pragma vector=PORT1_VECTOR         //indica ao compilador que esta função é uma rotina de serviço de interrupção para a Porta 1
__interrupt void Port1_ISR(void) {
    if (P1IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S2
        __delay_cycles(20000);   // Debounce
        if (!(P1IN & BIT1)) {    // Confirma o pressionamento
            if (dutyCycle < CCR0 - STEP_SIZE) {
                dutyCycle += STEP_SIZE; // Aumenta o duty cycle
                TA0CCR1 = dutyCycle;    // Atualiza o PWM
            }
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
            if (dutyCycle > STEP_SIZE) {
                dutyCycle -= STEP_SIZE; // Diminui o duty cycle
                TA0CCR1 = dutyCycle;    // Atualiza o PWM
            }
        }
        P2IFG &= ~BIT1;          // Limpa a flag de interrupção
    }
}

