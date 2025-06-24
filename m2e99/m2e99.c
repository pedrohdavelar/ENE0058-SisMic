#include <msp430.h>

//Lib para GPIO
//--> void    pinWrite(port, bit, value)
//--> uint8_t pinRead(port, bit)
//--> void    pinMode(port, bit, mode){input, output, inPullUp, inPullDown}

int main(void){
    WDTCTL = WDTPW | WDTHOLD;

    return 0;
} 

typedef enum {input, output, inPullUp, inPullDown} pinMode_t;

void pinMode (uint8_t port, uint8_t bit, pinMode_t mode){
    if(mode == output){
        
    }
}