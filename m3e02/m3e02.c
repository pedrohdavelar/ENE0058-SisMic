#include <msp430.h>
#include <stdint.h>
#include <intrinsics.h>

// Função para inicializar I2C
void initI2C() {
    P3SEL |= BIT0 | BIT1;          // Configura P3.0 (SCL) e P3.1 (SDA)
    UCB0CTL1 |= UCSWRST;           // Coloca em reset para configuração
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // I2C mestre, síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST; // Usa SMCLK, mantém em reset
    UCB0BR0 = 10;                  // FSCL = SMCLK/10 = 100kHz (SMCLK=1MHz)
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;          // Libera o módulo USCI
}

// Função para verificar um endereço I2C
uint8_t i2cCheckAddress(uint8_t addr) {
    UCB0I2CSA = addr;              // Define endereço do escravo
    UCB0CTL1 |= UCTR | UCTXSTT;    // Inicia transmissão como mestre
    
    // Aguarda fim da condição START ou timeout
    uint16_t timeout = 1000;
    while ((UCB0CTL1 & UCTXSTT) && timeout--) {
        __delay_cycles(10);
    }
    
    // Se timeout ocorreu, aborta
    if (!timeout) {
        UCB0CTL1 |= UCTXSTP;       // Gera condição STOP
        while (UCB0CTL1 & UCTXSTP); // Aguarda STOP ser enviado
        return 0;
    }
    
    // Verifica se houve ACK
    uint8_t ack = !(UCB0IFG & UCNACKIFG);
    
    // Gera condição STOP
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);    // Aguarda STOP ser enviado
    
    return ack;
}

// Função principal do scanner
uint8_t i2cScan(uint8_t *addrs) {
    uint8_t count = 0;
    uint8_t addr;  // Declaração movida para fora do for
    
    // Varre todos os endereços possíveis (0x08 a 0x77)
    for (addr = 0x08; addr < 0x78; addr++) {
        if (i2cCheckAddress(addr)) {
            addrs[count++] = addr;
        }
        __delay_cycles(1000);      // Pequeno delay entre verificações
    }
    
    return count;                  // Retorna número de dispositivos encontrados
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // Desabilita watchdog
    PM5CTL0 &= ~LOCKLPM5;         // Desbloqueia pinos (para MSP430FRxx)
    
    P1DIR |= BIT0;                // Configura P1.0 como saída (LED)
    
    initI2C();                    // Inicializa interface I2C
    
    uint8_t devices[16];          // Vetor para endereços encontrados
    uint8_t num_devices = 0;
    
    while (1) {
        P1OUT ^= BIT0;            // Pisca LED para indicar atividade
        
        num_devices = i2cScan(devices);
        
        __delay_cycles(1000000);  // Espera 1s entre varreduras
    }
}