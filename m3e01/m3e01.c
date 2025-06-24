#include <msp430.h>
#include <stdio.h>
#include <stdint.h>

// Endereços possíveis para o PCF8574
#define PCF8574AT_ADDR 0x3F
#define PCF8574T_ADDR  0x27

// Comandos para retroiluminação
#define BACKLIGHT_ON  0x08
#define BACKLIGHT_OFF 0x00

void initI2C() {
    // Configura pinos P3.0 (SCL) e P3.1 (SDA)
    P3SEL |= BIT0 | BIT1;
    
    // Configura USCI_B0 para modo I2C mestre
    UCB0CTL1 |= UCSWRST;           // Coloca em reset para configuração
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Modo I2C, mestre, síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST; // Usa SMCLK, mantém em reset
    
    // Configura velocidade (assumindo SMCLK = 1MHz)
    // FSCL = SMCLK / (UCBRx * 10) = 1MHz / (10 * 10) = 100kHz
    UCB0BR0 = 10;
    UCB0BR1 = 0;
    
    UCB0CTL1 &= ~UCSWRST;          // Libera o módulo USCI
}

uint8_t i2cSend(uint8_t addr, uint8_t data) {
    // 1. Escreve o endereço do escravo
    UCB0I2CSA = addr;
    
    // 2. Gera condição START como transmissor
    UCB0CTL1 |= UCTR | UCTXSTT;
    
    // 3. Aguarda TXBUF ficar vazio (TXIFG=1)
    while (!(UCB0IFG & UCTXIFG));
    
    // Escreve o dado no buffer de transmissão
    UCB0TXBUF = data;
    
    // 4. Aguarda o ciclo de acknowledge (STT volta para 0)
    while (UCB0CTL1 & UCTXSTT);
    
    // 5. Se houve ACK (NACKIFG=0), aguarda transmissão do dado
    if (!(UCB0IFG & UCNACKIFG)) {
        while (!(UCB0IFG & UCTXIFG));
    }
    
    // 6. Gera condição STOP
    UCB0CTL1 |= UCTXSTP;
    
    // 7. Aguarda STOP ser enviado (STP volta para 0)
    while (UCB0CTL1 & UCTXSTP);
    
    // 8. Retorna status do ACK (0=ACK, 1=NACK)
    return (UCB0IFG & UCNACKIFG) ? 1 : 0;
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // Desabilita watchdog
    __enable_interrupt();          // Habilita interrupções globais
    
    initI2C();                     // Inicializa I2C
    
    uint8_t lcd_addr = PCF8574T_ADDR; // Tenta primeiro com 0x27
    
    // Testa o endereço para ver qual está respondendo
    if (i2cSend(lcd_addr, BACKLIGHT_ON)) {
        lcd_addr = PCF8574AT_ADDR; // Se falhou, tenta com 0x3F
    }
    
    while (1) {
        // Liga retroiluminação
        i2cSend(lcd_addr, BACKLIGHT_ON);
        __delay_cycles(500000);    // Espera ~0.5s (SMCLK=1MHz)
        
        // Desliga retroiluminação
        i2cSend(lcd_addr, BACKLIGHT_OFF);
        __delay_cycles(500000);    // Espera ~0.5s (SMCLK=1MHz)
    }
}