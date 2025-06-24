#include <msp430.h>

// Variável global para armazenar o resultado da conversão
volatile unsigned int adc_result;

/**
 * @brief Configura o módulo ADC12 para uma única conversão no canal A0.
 */
void config_adc(void) {
    // Configura o pino P6.0 para a função de entrada analógica A0
    P6SEL |= BIT0;

    // Desabilita o ADC12 para permitir a configuração
    // ADC12CTL0_H contém a senha para os registradores FCTL
    ADC12CTL0 &= ~ADC12ENC;

    // ADC12CTL0: Configura tempo de amostragem e liga o ADC
    // ADC12SHT0_3: 32 ciclos de ADC12CLK para o tempo de amostragem (SHT0)
    // ADC12ON: Liga o módulo ADC12
    ADC12CTL0 = ADC12SHT0_3 | ADC12ON;

    // ADC12CTL1: Configura fonte de trigger, clock e modo de conversão
    // ADC12SHS_0: Trigger pela borda de subida do bit ADC12SC (software)
    // ADC12SHP: SAMPCON é derivado do pulso do timer de amostragem
    // ADC12SSEL_3: Seleciona SMCLK como fonte de clock para o ADC12
    // ADC12CONSEQ_0: Modo de conversão única em um único canal
    ADC12CTL1 = ADC12SHP | ADC12SHS_0 | ADC12SSEL_3 | ADC12CONSEQ_0;

    // ADC12CTL2: Configura a resolução da conversão
    // ADC12RES_2: Resolução de 12 bits
    ADC12CTL2 = ADC12RES_2;

    // ADC12MCTL0: Configura o canal de conversão e as referências
    // ADC12INCH_0: Seleciona o canal de entrada A0 (P6.0)
    // ADC12SREF_0: Referências VR+ = AVCC e VR- = AVSS
    ADC12MCTL0 = ADC12INCH_0 | ADC12SREF_0;

    // Habilita o ADC12 para iniciar as conversões
    ADC12CTL0 |= ADC12ENC;
}

/**
 * @brief Função principal
 */
int main(void) {
    // Para o Watchdog Timer
    WDTCTL = WDTPW | WDTHOLD;

    // Chama a função de configuração do ADC
    config_adc();

    while (1) {
        // Inicia a conversão A/D
        // O bit ADC12SC é automaticamente zerado pelo hardware
        ADC12CTL0 |= ADC12SC;

        // Aguarda a conclusão da conversão (polling da flag)
        while (!(ADC12IFG & ADC12IFG0));

        // Lê o resultado da conversão do registrador de memória 0
        adc_result = ADC12MEM0;

        // Ponto de interrupção (breakpoint) para observar o valor de 'adc_result'
        __no_operation();
    }

    return 0;
}