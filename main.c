/** 
 * @file main.c
 * @author M71111
 * @date 2024-08-16
 * @brief Main function 
 */

//#include "usart.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdbool.h>

void AC_init(){    
    // Set pin PD2 (port D, pin 2) as an input
    PORTD.DIRCLR = PIN2_bm;
    // Disable digital input buffer and pull-up resistor for pin PD2
    PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    // Select the positive (AINP0 for pin PD2) and negative input (DACREF) source by writing to the MUXPOS and MUXNEG bit fields
    // Hint: AC_MUXPOS_AINP0_gc | AC_MUXNEG_DACREF_gc;
    AC0.MUXCTRL = AC_MUXNEG_DACREF_gc | AC_MUXPOS_AINP0_gc;
    // Calculated DACREF value: DACREF = (V_DACREF*256)/V_REF = (0.1*256)/1.024 = 25
    AC0.DACREF = 25;
    // Enable the AC 
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

// Function to initialize sleep mode
void sleep_init(void) {
    set_sleep_mode(SLEEP_MODE_STANDBY);  // Standby sleep mode
}


void TCA0_init() {
    // Set the period of the timer. PER = period[s] * F_CPU / Prescaler = 0.01s * 4 000 000 Hz / 2
    TCA0.SINGLE.PER = 20000;
    // Enable timer overflow interrupt
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
    // Run timer in standby mode, set prescaler to 2, enable timer
    TCA0.SINGLE.CTRLA = TCA_SINGLE_RUNSTDBY_bm | TCA_SINGLE_CLKSEL_DIV2_gc | TCA_SINGLE_ENABLE_bm;
}

ISR(TCA0_OVF_vect)
{
    // Add your code here 
    if (AC_above_threshold()) {
        set_LED_off(); // Turn LED off when it's bright
    } else {
        set_LED_on();  // Turn LED on when it's dark
    }
    
    // Clear interrupt flag
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main(){
    VREF_init();
    AC_init();
    LED_init();
    // Initialize timer
    TCA0_init();
    // Enable interrupts
    sei();
    sleep_init();
    
    while(1) {
        sleep_mode();
    }
    return 0;
};
