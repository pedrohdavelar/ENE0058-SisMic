#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <msp430.h>

//Pinos do display: SDA - 3.0; SCL 3.1
//Alimentação em 5V
#include "lcd_pcf8574_hd44780.h" 

// NEC Protocol Timing Constants 
// SMCLK frequency: 1048576 Hz.
// 1 tick = 1 / 1048576 s = 0,954us
// Duração dos pulsos entre bordas de descida

const char hexTable[] = "0123456789ABCDEF";

#define PULSE_ZERO_TICKS 1700
#define PULSE_ONE_TICKS  3000
#define PULSE_START_TICKS 14000 //nem devo usar


volatile char irPulseBits[32];

char irPulseBitsAddr[8];
char irPulseBitsAddrHex[3];
char irPulseBitsCmd[8];
char irPulseBitsCmdHex[3];

char irPulseBitsAddrInv[8];
char irPulseBitsAddrInvHex[3];
char irPulseBitsCmdInv[8];
char irPulseBitsCmdInvHex[3];

volatile unsigned int irBitCount = 0;

volatile int signalReady = 0;

int i;

// Function prototypes
void init_ir_receiver(void);

#define GREEN_LED 0
#define RED_LED 1

void init_onboard_peripherals();
void led_on(int led);
void led_off(int led);
void led_toggle(int led);
void simple_delay (int x);

void convertZUToHexFast(const char input[8], char hex[2]);

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    init_onboard_peripherals();
    init_ir_receiver();
    
    
    lcd_init(); // Initialize the LCD (includes I2C initialization)

    lcd_backlight(1); // Turn backlight ON

    lcd_clear();
    lcd_set_cursor(0, 0); // Set cursor to Row 0, Column 0
    lcd_print_string("Hello, MSP430!");

    lcd_set_cursor(1, 2); // Set cursor to Row 1, Column 2
    lcd_print_string("I2C LCD Test");    
    
    simple_delay(1000);

    __enable_interrupt();   // Enable global interrupts (GIE)
    
    lcd_clear();
    while (1) {
        if (signalReady){
            for (i = 0; i < 8; ++i){
                irPulseBitsAddr[i]    = irPulseBits[i];
                irPulseBitsAddrInv[i] = irPulseBits[i+8];
                irPulseBitsCmd[i]     = irPulseBits[i+16];
                irPulseBitsCmdInv[i]  = irPulseBits[i+24];
            }
            convertZUToHexFast(irPulseBitsAddr, irPulseBitsAddrHex);
            convertZUToHexFast(irPulseBitsCmd, irPulseBitsCmdHex);
            convertZUToHexFast(irPulseBitsAddrInv, irPulseBitsAddrInvHex);
            convertZUToHexFast(irPulseBitsCmdInv, irPulseBitsCmdInvHex);
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print_string("Adr:");
            lcd_set_cursor(0,4);
            lcd_print_string(irPulseBitsAddrHex);
            lcd_set_cursor(0,7);
            lcd_print_string("Cmd:");
            lcd_set_cursor(0,12);
            lcd_print_string(irPulseBitsCmdHex);
            lcd_set_cursor(1, 0);
            lcd_print_string("Botao: ");
            lcd_set_cursor(1, 7);
            if (memcmp(irPulseBitsCmdHex, "A2", 2) == 0){
                P1OUT &= ~BIT6;
                P6OUT &= ~BIT6;
                lcd_print_string("1");
                P3OUT |= BIT3;
            } else if (memcmp(irPulseBitsCmdHex, "62", 2) == 0){
                P3OUT &= ~BIT3;
                P6OUT &= ~BIT6;
                lcd_print_string("2");
                P1OUT |= BIT6;
            } else if (memcmp(irPulseBitsCmdHex, "E2", 2) == 0){
                P3OUT &= ~BIT3;
                P1OUT &= ~BIT6;
                lcd_print_string("3");
                P6OUT |= BIT6;
            } else if (memcmp(irPulseBitsCmdHex, "22", 2) == 0){
                lcd_print_string("4");
            } else if (memcmp(irPulseBitsCmdHex, "02", 2) == 0){
                lcd_print_string("5");
            } else if (memcmp(irPulseBitsCmdHex, "C2", 2) == 0){
                lcd_print_string("6");
            } else if (memcmp(irPulseBitsCmdHex, "E0", 2) == 0){
                lcd_print_string("7");
            } else if (memcmp(irPulseBitsCmdHex, "A8", 2) == 0){
                lcd_print_string("8");
            } else if (memcmp(irPulseBitsCmdHex, "90", 2) == 0){
                lcd_print_string("9");
            } else if (memcmp(irPulseBitsCmdHex, "98", 2) == 0){
                P3OUT &= ~BIT3;
                P1OUT &= ~BIT6;
                P6OUT &= ~BIT6;
                lcd_print_string("0");
            } else if (memcmp(irPulseBitsCmdHex, "68", 2) == 0){
                lcd_print_string("*");
            } else if (memcmp(irPulseBitsCmdHex, "B0", 2) == 0){
                lcd_print_string("#");
            } else if (memcmp(irPulseBitsCmdHex, "18", 2) == 0){
                lcd_print_string("^");
            } else if (memcmp(irPulseBitsCmdHex, "4A", 2) == 0){
                lcd_print_string("v");
            } else if (memcmp(irPulseBitsCmdHex, "10", 2) == 0){
                lcd_print_string("<");
            } else if (memcmp(irPulseBitsCmdHex, "5A", 2) == 0){
                lcd_print_string(">");
            } else if (memcmp(irPulseBitsCmdHex, "38", 2) == 0){
                lcd_print_string("Ok");
                P3OUT ^= BIT4;

            } else {
                lcd_print_string("?????");
            }
            led_toggle(GREEN_LED);
            lcd_backlight(1);
            signalReady = 0;
            irBitCount = 0;
            TA1CCTL1 &= ~CCIFG;
            TA1CCTL1 |= CCIE;
        }


    }
}


void init_ir_receiver(void) {

    // 2. Configure P2.0 for IR input (TA1.CCI1A)
    P2DIR &= ~BIT0;    // Set P2.0 as input
    P2SEL |= BIT0;     // Select peripheral function for P2.0 (TA1.CCI1A)
    P2REN |= BIT0;     // Enable pull-up/pull-down resistor on P2.0
    P2OUT |= BIT0;     // Select pull-up resistor

    // 3. Configure Timer_A1 for capture
    // SMCLK, Continuous mode, /4 divider (ID_2), TAIFG interrupt enable
    TA1CTL = TASSEL_2 | MC_2 | ID_0 | TAIE | TACLR;
    // Capture on falling edge, CCI1A, Sync, Capture mode, CCIE interrupt enable
    TA1CCTL1 = CM_2 | CCIS_0 | SCS | CAP | CCIE;
}




void init_onboard_peripherals(){
    //Configuração do LED vermelho (P1.0)
    P1DIR |= BIT0;   // Define P1.0 como saída
    P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
    P1OUT |= BIT0;  // Inicializa o led aceso

    //Configuração do LED verde (P4.7)
    P4DIR |= BIT7;  // Define P4.7 como saída
    P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
    P4OUT |= BIT7; // Inicializa o LED verde aceso

    //Pino 3.4 para saída da lampada
    P3DIR |= BIT4; 
    P3REN &= ~BIT4;
    P3OUT &= ~BIT4; 

    //Pino 3.3 para velocidade 1
    P3DIR |= BIT3; 
    P3REN &= ~BIT3;
    P3OUT &= ~BIT3;

    //Pino 1.6 para velocidade 2
    P1DIR |= BIT6; 
    P1REN &= ~BIT6;
    P1OUT &= ~BIT6;

    //Pino 6.6 para velocidade 3
    P6DIR |= BIT6; 
    P6REN &= ~BIT6;
    P6OUT &= ~BIT6;


    
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

static inline void led_on(int led){
    switch (led) {
    case GREEN_LED: P4OUT |= BIT7; break;
    case RED_LED  : P1OUT |= BIT0; break;
    }
}
static inline void led_off(int led){
    switch (led) {
    case GREEN_LED: P4OUT &= ~BIT7; break;
    case RED_LED  : P1OUT &= ~BIT0; break;
    }
}
static inline void led_toggle(int led){
    switch (led){
    case GREEN_LED: P4OUT ^= BIT7; break;
    case RED_LED  : P1OUT ^= BIT0; break;
    }
}

void simple_delay (int x){
    while (x > 0){
        --x;
    }
}


// Rotina de serviço de interrupção para a Porta 1 (botão S2)
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    if (P1IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S2
        __delay_cycles(20000);   // Debounce
        if (!(P1IN & BIT1)) {    // Confirma o pressionamento
            led_toggle(RED_LED); //pisca o led vermelho
            lcd_clear();
            lcd_backlight(1);
            lcd_set_cursor(0,0);
            lcd_print_string("BUTTON A PRESS");
            }
        }
    P1IFG &= ~BIT1;          // Limpa a flag de interrupção
}

// Rotina de serviço de interrupção para a Porta 2 (botão S1)
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
    if (P2IFG & BIT1) {          // Verifica se a interrupção foi causada pelo botão S1
        __delay_cycles(20000);   // Debounce
        if (!(P2IN & BIT1)) {    // Confirma o pressionamento
            led_toggle(GREEN_LED);
            lcd_clear();
            lcd_backlight(0);
            lcd_set_cursor(0,0);
            lcd_print_string("BUTTON B PRESS");
            }
        }
    P2IFG &= ~BIT1;          // Limpa a flag de interrupção
}

void convertZUToHexFast(const char input[8], char hex[3]) {
    uint8_t byte = 0;
    for (i = 0; i < 8; i++) {
        if (input[i] == 'U') byte |= (1 << (7 - i));
    }
    hex[0] = hexTable[byte >> 4];  // High nibble (e.g., 0xF in 0xF1)
    hex[1] = hexTable[byte & 0x0F]; // Low nibble (e.g., 0x1 in 0xF1)
    hex[2] = '\0';
}

// Timer_A1 Interrupt Service Routine
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void) {
    led_toggle(RED_LED);
    if (!signalReady){
        switch (__even_in_range(TA1IV, TA1IV_TAIFG)) { // TA1IV_TAIFG is 0x0E
            case TA1IV_NONE:
                break;
            case TA1IV_TACCR1: // CCR1 Capture
                if (TA1CCR1 < PULSE_ZERO_TICKS){        //sinal 0
                    irPulseBits[irBitCount++] = 'Z';
                    TA1CTL |= TACLR;
                } else if (TA1CCR1 < PULSE_ONE_TICKS){  //sinal 1
                    irPulseBits[irBitCount++] = 'U';
                    TA1CTL |= TACLR;
                } else {                                //sinal Start - recomeça contagem de bits
                    irBitCount = 0;
                    TA1CTL |= TACLR;
                }
                break;
            case TA1IV_TAIFG:  // Timer_A1 overflow - Significa REPEAT
                TA1CTL |= TACLR; //por hora sem lidar com o repeat
                break;
            default:
                break;
        }
    } else {
        TA1CTL |= TACLR;
    }
    if (irBitCount >= 32){      //completou a contagem
        TA1CCTL1 &= ~CCIE;      //suspende temporariamente as interrupções
        signalReady = 1;        //avisa que o sinal esta pronto
    } 
}
