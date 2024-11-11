/*
 * File:   main.c
 * Author: markus-klund
 *
 * Created on November 10, 2024, 8:56 AM
 */


#include <xc.h>
//#include "usart.h"
#include <stdbool.h>
#include <util/delay.h>

void AC_init(){
    // Set pin PD2 (port D, pin 2) as an input
    PORTD.DIRCLR = PIN2_bm;
    // Disable digital input buffer and pull-up resistor for pin PD2
    PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    // Remaining initialization steps...
    
    // Set the positive input (PD2) and the negative input (DACREF) in MUXCTRL
    AC0.MUXCTRL = AC_MUXNEG_DACREF_gc | AC_MUXPOS_AINP0_gc;
    //AC0.DACREF = (0.1 * 256) / 1.024;
    AC0.DACREF = 25;
    
    // Enable Analog Comparator
    AC0.CTRLA = AC_ENABLE_bm;
}

void VREF_init(void) {
    VREF.ACREF = VREF_REFSEL_1V024_gc;
}

void LED_init() {
    PORTA.DIRSET = PIN2_bm;
}

void set_LED_on(){
    PORTA.OUTCLR = PIN2_bm; // Set pin LOW to turn LED on
}

void set_LED_off(){
    PORTA.OUTSET = PIN2_bm; // Set pin HIGH to turn LED off
}

bool AC_above_threshold() {
    // Check the output of the Analog Comparator
    if (AC0.STATUS & AC_CMPSTATE_bm) {
        return true;
    } else {
        return false;
    }
}

void main(void) {
    USART3_Init();
    AC_init();
    VREF_init();
    LED_init();    
    
    while (1) {
        if (AC_above_threshold()) {
            set_LED_off(); // Turn LED off when it's bright
        } else {
            set_LED_on();  // Turn LED on when it's dark
        }
    }
    
    return;
}
