#include <msp430.h>
#include "lcd_pcf8574_hd44780.h" // Include the LCD library header

// Basic delay function for longer delays (adjust for MCLK if not 1MHz)
void delay_ms(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms; i++) {
        // Assuming SMCLK is approximately 1MHz for __delay_cycles.
        // If SMCLK is, for example, 8MHz, then 8000 cycles would be 1ms.
        __delay_cycles(1000); // For 1MHz SMCLK, 1000 cycles = 1ms.
                              // Adjust this value if SMCLK is different.
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // IMPORTANT: The I2C baud rate in i2c_init_msp430() and the duration of
    // __delay_cycles() in the LCD library depend on the SMCLK frequency.
    // By default, MSP430F5529 starts with DCO at ~1.045MHz, which becomes MCLK and SMCLK.
    // If you change clock settings (e.g., to use a crystal or higher DCO speed),
    // ensure I2C baud rate and delay calculations are still valid or adjusted.
    // For this example, default clock settings are assumed.

    lcd_init(); // Initialize the LCD (includes I2C initialization)

    lcd_backlight(1); // Turn backlight ON

    lcd_clear();
    //lcd_set_cursor(0, 0); // Set cursor to Row 0, Column 0
    //lcd_print_string("Hello, MSP430!");
//
    //lcd_set_cursor(1, 2); // Set cursor to Row 1, Column 2
    //lcd_print_string("I2C LCD Test");

    lcd_write("Pedro Henrique Dias Avelar");

    while(1) {
        
    }
    // return 0; // This line is unreachable in an embedded loop
}
