#include <msp430.h>

typedef unsigned char u8;

// Definições da UART
#define MAX_BUFFER 128

char rxBuffer[MAX_BUFFER];
volatile u8 rxIndex = 0;
volatile u8 received = 0;

void uartPrint(char *string)
{
    while (*string)
    {
        while (!(UCA1IFG & UCTXIFG)); // Espera TX estar pronto
        UCA1TXBUF = *string++;        // Envia caractere
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Desliga o watchdog timer

    // Configuração do UART
    UCA1CTL1 = UCSWRST;                // RST a interface
    UCA1CTL1 |= UCSSEL__SMCLK;         // SMCLK (~1.048576 MHz)
    UCA1BRW = 109;                     // Baud rate 9600
    UCA1MCTL = UCBRF_2;                // Ajuste fina
    P4SEL |= BIT4 | BIT5;              // P4.4 e P4.5 como UART
    UCA1CTL1 &= ~UCSWRST;              // Libera o módulo
    UCA1IE |= UCRXIE;                  // Habilita interrupção na recepção

    __enable_interrupt();              // Ativa interrupções globais

    uartPrint("Digite algo no terminal:\n\r");

    while (1)
    {
        while (!received);             // Aguarda dados
        received = 0;

        uartPrint("Voce digitou: ");
        uartPrint(rxBuffer);
        uartPrint("\n\r");
    }
}

// Interrupção UART
#pragma vector=46
__interrupt void uart_isr(void)
{
    char c = UCA1RXBUF;

    if (rxIndex < MAX_BUFFER - 1)
    {
        rxBuffer[rxIndex++] = c;

        if (c == '\r')
        {
            rxBuffer[rxIndex] = '\0';
            received = 1;
            rxIndex = 0;
        }
    }
    else
    {
        rxIndex = 0; // Overflow do buffer
    }
}