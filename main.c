/*
 * File:   main.c
 * Author: markus-klund
 *
 * Created on November 9, 2024, 8:38 AM
 */


#include <xc.h>
#include "usart.h"
#include <util/delay.h>
#include <stdbool.h>



void AC_init(){
    // Set pin PD2 (port D, pin 2) as an input
    PORTD.DIRCLR = PIN2_bm;
    // Disable digital input buffer and pull-up resistor for pin PD2
    PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    // Remaining initialization steps...
    AC0.DACREF = <calculated_DACREF>
}

void VREF_init(void) {
    VREF.ACREF = VREF_REFSEL_1V024_gc;
}

void LED_init() {
    PORTA.DIRSET = PIN2_bm;
}

void set_LED_on(){
    // LED is active low. Set pin LOW to turn LED on
    PORTA.OUTCLR = PIN2_bm;
}
void set_LED_off(){
    // LED is active low. Set pin HIGH to turn LED off
    PORTA.OUTSET = PIN2_bm;
}

bool AC_above_threshold() {
    // Check the output of the Analog Comparator
    if (AC0.STATUS & AC_CMPSTATE_bm) {
        return true;
    } else {
        return false;
    }
}

int main() {
    AC_init();
    VREF_init();
    USART3_Init(); // remove
    LED_init();
    USART3_SendChar('A'); // remove
    
    while (1) {
        // Implement the busy-waiting scheme
        if (AC_above_threshold()) {
            set_LED_on();
        } else {
            set_LED_off();
        }
    }
    
    return 0;
}
