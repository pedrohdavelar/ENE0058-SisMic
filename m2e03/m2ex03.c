/* 
Módulo 2 - Exercício 3
Remoção de rebotes
*/

#include <msp430.h>

#define DEBOUNCE_CNT 10000


//função para realizar um certo delay no programa
void debounce (volatile int);

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
  
/*Configuração do LED verde (P4.7) - não utilizado nesse exercício
  P4DIR |= BIT7;  // Define P4.7 como saída
  P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
  P4OUT &= ~BIT7; // Inicializa o LED verde como apagado- led apagado em baixa e acesso em alta
*/

//Configuração do LED vermelho (P1.0)
  P1DIR |= BIT0;   // Define P1.0 como saída
  P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
  P1OUT &= ~BIT0;  // Inicializa o led apagado
  

  // Configuração da chave S1 (P2.1) como entrada com resistor de pull-up
    P2DIR &= ~BIT1;  // Define P2.1 como entrada
    P2REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P2OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
  
  while(1){                                  // superloop
    
    if ((P2IN & BIT1) == 0){    //Quando o botão for pressionado
      P1OUT ^= BIT0;          //flipa o LED vermelho
      debounce(DEBOUNCE_CNT); //faz o delay

      while(P2IN & BIT1 == 0){
        debounce(DEBOUNCE_CNT); //e repete o delay enquanto o botão estiver pressionado
      } 
    }
  }
}

void debounce (volatile int cnt){
  int i, j;
  for (i = 10; i > 0; --i){
    for (j = cnt; j > 0; --j){

    }
  }
}


