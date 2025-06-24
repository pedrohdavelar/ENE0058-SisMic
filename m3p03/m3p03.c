#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <msp430.h>

//Pinos do display: SDA - 3.0; SCL 3.1
//Alimentação em 5V
#include "lcd_pcf8574_hd44780.h" 

typedef unsigned char u8;
typedef unsigned int u16;

#define BAUD_RATE 19200
#define MAX_CHANNELS 8
#define SAMPLE_RATE_BASE 200 // 200 amostras/segundo base

volatile u8 num_channels = 1; // Default: 1 canal
volatile u16 adc_values[MAX_CHANNELS];
volatile u8 sampling_flag = 0;

void init_uart(void);
void uartPrint(char *string);
void init_adc(void);
void init_timer(int channels);
void send_data(void);



#define GREEN_LED 0
#define RED_LED 1

void init_onboard_peripherals();
void led_on(int led);
void led_off(int led);
void led_toggle(int led);
void simple_delay (int x);


void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    init_onboard_peripherals();
    
    
    lcd_init(); // Initialize the LCD (includes I2C initialization)

    lcd_backlight(1); // Turn backlight ON

    lcd_clear();
    lcd_set_cursor(0, 0); // Set cursor to Row 0, Column 0
    lcd_print_string("Hello, MSP430!");

    lcd_set_cursor(1, 2); // Set cursor to Row 1, Column 2
    lcd_print_string("I2C LCD Test");    
    
    __delay_cycles(1000);
    lcd_clear();
    __enable_interrupt();   // Enable global interrupts (GIE)
    

    while (1) {
        
        lcd_set_cursor(0, 0);
        lcd_print_string("Main Loop");
        led_toggle(GREEN_LED);
        __delay_cycles(5000);
        led_toggle(RED_LED);
        __delay_cycles(15000);
    }
}




void init_onboard_peripherals(){
    //Configuração do LED vermelho (P1.0)
    P1DIR |= BIT0;   // Define P1.0 como saída
    P1REN &= ~BIT0;  // Pinos de saída não utilizam o resistor interno
    P1OUT |= BIT0;  // Inicializa o led apagado

    //Configuração do LED verde (P4.7)
    P4DIR |= BIT7;  // Define P4.7 como saída
    P4REN &= ~BIT7; // Para pinos de saída, desabilitamos o resistor interno
    P4OUT |= BIT7; // Inicializa o LED verde como apagado- led apagado em baixa e acesso em alta

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

void init_uart(){
    // Configuração do UART
    UCA1CTL1 = UCSWRST;                // RST a interface
    UCA1CTL1 |= UCSSEL__SMCLK;         // SMCLK (~1.048576 MHz)
    UCA1BRW = 54;                      // Baud rate 19200
    UCA1MCTL = UCBRF_9 + UCBSRS_2;     // Ajuste fino
    P4SEL |= BIT4 | BIT5;              // P4.4 e P4.5 como UART
    UCA1CTL1 &= ~UCSWRST;              // Libera o módulo
    UCA1IE |= UCRXIE;                  // Habilita interrupção na recepção
}
