/* 
Módulo 2 - Exercício 6
Amostrando flags do timer
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
  
  //Configuração do LED verde (P4.7) - não utilizado nesse exercício
    P4DIR |= BIT7;  // Define P4.7 como saída
    P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
    P4OUT &= ~BIT7; // Inicializa o LED verde como apagado- led apagado em baixa e acesso em alta

  //Configuração do LED vermelho (P1.0)
    P1DIR |= BIT0;   // Define P1.0 como saída
    P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
    P1OUT &= ~BIT0;  // Inicializa o led apagado
  
  // Configuração da chave S1 (P2.1) como entrada com resistor de pull-up
    P2DIR &= ~BIT1;  // Define P2.1 como entrada
    P2REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P2OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
  
/*Explicação Timer - Cap 7 Apostila
  //Timer A - 3 Instâncias (TA0, TA1, TA2)
  //Timer B - 1 Instância (TB0) 
  //Cada instância possui um contador de 16 bits - TA0R, TA1R, TA2R, TB0R
  //E varios registradores de comparação e captura (CC) - TAxCCNn e TAxCCTLn - x -> instância do timer; n -> instância da unidade de captura e comparação
  //Modos de contagem: Modo 0 - Parado; Modo 1 - Ascendente; Modo 2 - Contínuo; Modo 3 - Up/Down
  //Gerar saídas PWM: Cada unidade de captura e compara de cada timer pode selecionar um mecanismo para gerar saídas
  //Flags: TAIFG -> Vai a 1 toda vez que o contador passar pelo zero; CCIFG -> vai a 1 toda vez que o valor do contador coincidir com o valor do registrador de comparação
  */

  //
  
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
  TA0CCR0 = 8192-1;
  TA0CCR1 = TA0CCR0 /2;
  
  while(1){                                  // superloop
    
    //repetir cada vez que necessitar o atraso
    while (!(TA0CTL & TAIFG));      // Aguarda a flag de overflow (TAIFG) ser setada

    if (TA0CCTL1 & CCIFG){  //A flag CCIFG é setada quando o contador atinge TA0CCR1 (o duty cycle)
      P1OUT |= BIT0;        //Acende o led vermelho
      P4OUT &= ~BIT7;       //Apaga o  led verde
      TA0CCTL1 &= ~CCIFG;   //Limpa a flag de comparação
    }
    
    if (TA0CTL & TAIFG) {   //A flag TAIFG é setada quando o contador atinge TA0CCR0 (o período)
      P1OUT &= ~BIT0;       //Apaga o LED vermelho
      P4OUT |= BIT7;        //Acende o  led verde
      TA0CTL &= ~TAIFG;     //impa a flag de overflow
    }
  }
}

