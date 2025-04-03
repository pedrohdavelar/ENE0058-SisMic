#include <msp430.h>

int main(void)
{
  volatile unsigned int i;

  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  P1DIR |= BIT0;                            // P1.0 set as output

  while(1)                                  // continuous loop
  {
    P1OUT ^= BIT0;                          // XOR P1.0
    for(i=50000;i>0;i--);                   // Delay
  }
}