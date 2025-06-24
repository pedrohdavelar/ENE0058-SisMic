/* 
Módulo 2 - Problema 02
Decodificação de Códigos NEC
*/

#include <msp430.h>

//Constantes para calibração da detecção  de sinal
#define TIMER_CLK_FREQ    1048576 //SMCLK
#define START_BIT_MIN     8500    //Tolerância ~9ms
#define START_BIT_MAX     9500    
#define LOGIC_0_MIN       1000    //Tolerância ~1.125ms
#define LOGIC_0_MAX       1300
#define LOGIC_1_MIN       2000    //Tolerância ~2.25ms
#define LOGIC_1_MAX       2400
#define REPEAT_CODE_MIN   10500   //Tolerância ~110ms entre sinais
#define END_PULSE_MIN     500     //Tolerância ~562.5us
#define END_PULSE_MAX     700

//Variáveis globais para uso do detector IR
unsigned long lastCapture = 0;
unsigned int captureCount = 0;
unsigned char receivedData[4];
unsigned char byteIndex = 0;
unsigned char bitCount = 0;

volatile unsigned char frameComplete = 0;

void led_Init();
void button_Init();
void timer_Init();

int main(void)
{

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  led_Init();
  button_Init();
  timer_Init();
  __enable_interrupt();
  
  while(1){                                  // superloop
    if (frameComplete){
      frameComplete = 0;
      unsigned char command = receivedData[2];
      unsigned char inverted = receivedData[3];

      if ((command & 0xFF) == (~inverted & 0xFF)){
        switch(command){
          case 0xE0: P1OUT ^= BIT0; break;
          case 0xA8: P4OUT ^= BIT7; break;
          default: break;
        }
      }
    }
    
  }
}

//***********************
//Rotinas de Configuração
//***********************


void led_Init(){
  //Configuração do LED vermelho (P1.0)
  P1DIR |= BIT0;   // Define P1.0 como saída
  P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
  P1OUT |= BIT0;  // Inicializa o led apagado

//Configuração do LED verde (P4.7)
  P4DIR |= BIT7;  // Define P4.7 como saída
  P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
  P4OUT &= ~BIT7; // Inicializa o LED verde como apagado- led apagado em baixa e acesso em alta
}

void button_Init(){
    //Configuração da chave S1 (P2.1) como entrada com resistor de pull-up com acionamento via interrupção
    P2DIR &= ~BIT1;  // Define P2.1 como entrada
    P2REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P2OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P2IE  |= BIT1;   // Habilita Interrupção
    P2IES |= BIT1;   // Interrupção na borda de descida
    P2IFG &= ~BIT1;  // Limpa a flag de interrupção

  //Configuração da chave S2 (P1.1) como entrada com resistor de pull-up com acionamento via interrupção
    P1DIR &= ~BIT1;  // Define P1.1 como entrada
    P1REN |= BIT1;   // Habilita o resistor de pull-up/pull-down
    P1OUT |= BIT1;   // Configura como pull-up (alto quando solto, baixo quando pressionado)
    P1IE  |= BIT1;   // Habilita Interrupção
    P1IES |= BIT1;   // Interrupção na borda de descida
    P1IFG &= ~BIT1;  // Limpa a flag de interrupção
}

void timer_Init(){
  //Configuração do VS1838B - P1.2 - Capture Mode do Timer TA0.1
  P1DIR &= ~BIT2;    // P1.2 como entrada
  P1SEL |= BIT2;     // Função extra do P1.2

  //Configuração do Timer
  TA0CTL = TASSEL_2 + MC_2 + TACLR + TAIE; //SMCLK, modo continuo, limpa TAR, habilita interrupt
  TA0CCTL1 = CM_2 + CCIS_0 + CAP + CCIE;   //Borda subida/descida, entrada CCIA, modo de captura, habilita interrupt
}


//**********************
//Rotinas de Interrupção
//**********************

// Rotina de serviço de interrupção para a Porta 1 (botão S2)
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    if (P1IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S2
        __delay_cycles(20000);   // Debounce
        if (!(P1IN & BIT1)) {    // Confirma o pressionamento
          P1OUT ^= BIT0;
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
          P4OUT ^= BIT7;
        }
        P2IFG &= ~BIT1;          // Limpa a flag de interrupção
    }
}

// Interrupção para o timer para detecção de pulsos
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TimerA0_ISR(void) {
    
    P4OUT ^= BIT7;
    unsigned long currentCapture = TA0CCR1;

    if (TA0IV == TA0IV_TACCR1) {
        P1OUT ^= BIT0;
        if (lastCapture != 0) {
            unsigned long delta = currentCapture - lastCapture;

            if (delta > REPEAT_CODE_MIN) {
                // Inicio de um novo frame
                byteIndex = 0;
                bitCount = 0;
            } else if (delta > START_BIT_MIN && delta < START_BIT_MAX) {
                // Bit inicial detectado
                byteIndex = 0;
                bitCount = 0;
            } else if (delta > END_PULSE_MIN && delta < END_PULSE_MAX) {
                // fim do frame
                if (byteIndex == 4) {
                    // Frame completo capturado
                    frameComplete = 1;
                    byteIndex = 0;
                }
            } else if (delta > LOGIC_0_MIN && delta < LOGIC_0_MAX) {
                // Sinal Lógico 0
                receivedData[byteIndex] >>= 1;
                bitCount++;
            } else if (delta > LOGIC_1_MIN && delta < LOGIC_1_MAX) {
                // Sinal Lógico 1
                receivedData[byteIndex] = (receivedData[byteIndex] >> 1) | 0x80;
                bitCount++;
            }

            if (bitCount == 8) {
                //completou um bit
                bitCount = 0;
                byteIndex++;
            }
        }

        lastCapture = currentCapture;
    }

    TA0CCTL1 &= ~CCIFG; // Clear flag
}