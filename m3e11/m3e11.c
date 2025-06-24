#include <msp430.h>

// Frequência do SMCLK (aproximadamente 1.048 MHz)
#define SMCLK_FREQ 1048576
// Frequência de amostragem desejada (ex: 10 kHz)
#define SAMPLING_FREQ 10000
// Período do Timer_A0 para atingir a frequência de amostragem
#define TA0_PERIOD (SMCLK_FREQ / SAMPLING_FREQ)

volatile unsigned int adc_result[100];
volatile unsigned int adc_index = 0;

/**
 * @brief Configura o Timer_A0 para gerar um gatilho PWM para o ADC.
 */
void config_timer_A0(void) {
    // TA0CTL: Seleciona SMCLK, modo Up, limpa o timer
    // TASSEL_2: SMCLK
    // MC_1: Modo Up (conta até TA0CCR0)
    // TACLR: Limpa o contador TA0R
    TA0CTL = TASSEL_2 | MC_1 | TACLR;

    // TA0CCR0: Define o período do PWM (frequência de amostragem)
    TA0CCR0 = TA0_PERIOD - 1;

    // TA0CCTL1: Configura o canal 1 para gerar o pulso de gatilho
    // OUTMOD_7: Modo de saída Reset/Set. A saída (TA0.1) vai para HIGH
    // quando TA0R reseta (em TA0CCR0) e para LOW quando TA0R atinge TA0CCR1.
    TA0CCTL1 = OUTMOD_7;

    // TA0CCR1: Define o duty cycle do pulso.
    // A duração do pulso alto será (TA0CCR0 - TA0CCR1) / SMCLK_FREQ.
    // Escolhemos um valor que garanta um tempo de amostragem > 2.47us.
    // Com um período de ~100 ciclos, 50 ciclos dão uma ampla margem.
    TA0CCR1 = TA0_PERIOD / 2;
}

/**
 * @brief Configura o ADC12 para ser acionado pelo Timer_A0.1.
 */
void config_adc_timer_triggered(void) {
    // Configura o pino P6.0 para a função de entrada analógica A0
    P6SEL |= BIT0;

    // Desabilita o ADC12 para permitir a configuração
    ADC12CTL0 &= ~ADC12ENC;

    // ADC12CTL0: Configura tempo de amostragem e liga o ADC
    ADC12CTL0 = ADC12SHT0_3 | ADC12ON;

    // ADC12CTL1: Configura fonte de trigger, clock e modo de conversão
    // ADC12SHS_1: Trigger pelo Timer_A0, canal 1 (TA0.1)
    // ADC12SHP: NÃO usa o timer de amostragem interno (SAMPCON segue SHI)
    // ADC12SSEL_3: Seleciona SMCLK como fonte de clock para o ADC12
    // ADC12CONSEQ_2: Modo de repetição de conversão em um único canal
    ADC12CTL1 = ADC12SHS_1 | ADC12SSEL_3 | ADC12CONSEQ_2;

    // ADC12CTL2: Configura a resolução da conversão
    ADC12CTL2 = ADC12RES_2;

    // ADC12MCTL0: Configura o canal de conversão e as referências
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

    // Configura os periféricos
    config_timer_A0();
    config_adc_timer_triggered();

    // Nenhuma ação de início é necessária aqui. O Timer_A0
    // começará a acionar o ADC automaticamente.

    while (1) {
        // Aguarda a conclusão da conversão (polling da flag)
        while (!(ADC12IFG & ADC12IFG0));

        // Lê o resultado da conversão
        adc_result[adc_index] = ADC12MEM0;
        ++adc_index;
        if (adc_index > 99){adc_index = 0;}

        // Ponto de interrupção (breakpoint) para observar o valor de 'adc_result'
        __no_operation();
    }

    return 0;
}