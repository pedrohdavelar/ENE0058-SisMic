#include <msp430.h>
#include <stdint.h>
#include <intrinsics.h>

// Definições para o LCD
#define LCD_ADDR 0x27  // Endereço I2C do PCF8574 (0x27 ou 0x3F)
#define LCD_RS   0x01
#define LCD_RW   0x02
#define LCD_EN   0x04
#define LCD_BL   0x08  // Backlight
#define LCD_D4   0x10
#define LCD_D5   0x20
#define LCD_D6   0x40
#define LCD_D7   0x80

// Protótipos de funções
void initI2C();
uint8_t i2cSend(uint8_t addr, uint8_t data);
void lcdWriteNibble(uint8_t nibble, uint8_t isChar);
void lcdWriteCmd(uint8_t cmd);
void lcdWriteData(uint8_t data);
void lcdInit();
void lcdWriteString(const char *str);

// Implementação da initI2C() que estava faltando
void initI2C() {
    // 1. Configura os pinos I2C
    P3SEL |= BIT0 | BIT1;  // P3.0 (SCL) e P3.1 (SDA) como funções especiais
    
    // 2. Configura o módulo USCI_B0 como I2C Master
    UCB0CTL1 |= UCSWRST;           // Coloca em reset para configuração
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Modo I2C Master, síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST; // Clock source SMCLK, mantém em reset
    
    // 3. Configura a velocidade (100kHz com SMCLK = 1MHz)
    UCB0BR0 = 12;
    UCB0BR1 = 0;
    
    // 4. Libera o módulo USCI
    UCB0CTL1 &= ~UCSWRST;
}

// Implementação da i2cSend()
uint8_t i2cSend(uint8_t addr, uint8_t data) {
    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;
    
    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = data;
    
    while (UCB0CTL1 & UCTXSTT);
    
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);
    
    return (UCB0IFG & UCNACKIFG) ? 1 : 0;
}

// Implementação da lcdWriteNibble()
void lcdWriteNibble(uint8_t nibble, uint8_t isChar) {
    uint8_t data = (nibble << 4) | (isChar ? LCD_RS : 0) | LCD_BL;
    
    i2cSend(LCD_ADDR, data);
    __delay_cycles(50);
    i2cSend(LCD_ADDR, data | LCD_EN);
    __delay_cycles(50);
    i2cSend(LCD_ADDR, data);
    __delay_cycles(200);
}

// Funções para controle do LCD
void lcdWriteCmd(uint8_t cmd) {
    lcdWriteNibble(cmd >> 4, 0);
    lcdWriteNibble(cmd & 0x0F, 0);
    if (cmd == 0x01 || cmd == 0x02) __delay_cycles(2000); // Delay extra para clear e home
}

void lcdWriteData(uint8_t data) {
    lcdWriteNibble(data >> 4, 1);
    lcdWriteNibble(data & 0x0F, 1);
}

void lcdWriteString(const char *str) {
    while (*str) {
        lcdWriteData(*str++);
    }
}

void lcdInit() {
    // Espera inicial mais longa para garantir estabilização
    __delay_cycles(100000); // ~100ms
    
    // Sequência de inicialização mais robusta
    lcdWriteNibble(0x03, 0);
    __delay_cycles(4500);   // Espera 4.5ms
    lcdWriteNibble(0x03, 0);
    __delay_cycles(4500);   // Espera 4.5ms
    lcdWriteNibble(0x03, 0);
    __delay_cycles(150);    // Espera 150μs
    lcdWriteNibble(0x02, 0); // Configura modo 4 bits
    __delay_cycles(150);
    
    // Configuração do display com verificações
    lcdWriteCmd(0x28); // Interface 4 bits, 2 linhas
    __delay_cycles(60);
    lcdWriteCmd(0x08); // Display off
    __delay_cycles(60);
    lcdWriteCmd(0x01); // Clear display
    __delay_cycles(2000); // Espera 2ms
    lcdWriteCmd(0x06); // Modo de entrada
    __delay_cycles(60);
    lcdWriteCmd(0x0C); // Display on, cursor off
    __delay_cycles(60);
}

// Função main() obrigatória
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // Desabilita watchdog
    PM5CTL0 &= ~LOCKLPM5;         // Desbloqueia pinos
    
    initI2C();                    // Inicializa I2C
    lcdInit();                    // Inicializa LCD
    
    // Teste do display
    lcdWriteCmd(0x80);            // Linha 1, posição 0
    lcdWriteString("MSP430 I2C");
    
    lcdWriteCmd(0xC0);            // Linha 2, posição 0
    lcdWriteString("LCD Funciona!");
    
    while(1) {
        __no_operation();
    }
}