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
    //AC0.CTRLA = AC_ENABLE_bm;
    AC0.CTRLA = AC_ENABLE_bm | AC_RUNSTDBY_bm;
    // Enable interrupt on both rising and falling edges
    AC0.INTCTRL = AC_CMP_bm | AC_INTMODE_NORMAL_BOTHEDGE_gc;
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

void EVSYS_init() {
    // Set up Event Channel 0 with Analog Comparator as the event generator
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc;
    
    EVSYS.USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc;
}

// Function to initialize sleep mode
void sleep_init(void) {
    set_sleep_mode(SLEEP_MODE_STANDBY);  // Standby sleep mode
}

void configure_unused_pins(void) {
    // Configure the setting for unused pins: disable digital input buffer, enable internal pull-up
    PORTA.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm;

    // Apply this configuration to all pins on each port except for the ones in use
    // PORTA - Apply to all except PA2 (used for LED)
    PORTA.PINCTRLUPD = 0xFD; // 0b11111101, excludes PA2

    // PORTB - Apply to all pins (assuming none are in use)
    PORTB.PINCTRLUPD = 0xFF; // All pins

    // PORTC - Apply to all pins (assuming none are in use)
    PORTC.PINCTRLUPD = 0xFF; // All pins

    // PORTD - Apply to all except PD2 (used for sensor input)
    PORTD.PINCTRLUPD = 0xFB; // 0b11111011, excludes PD2

    // PORTE - Apply to all pins (assuming none are in use)
    PORTE.PINCTRLUPD = 0xFF; // All pins
}


int main(){
    VREF_init();
    AC_init();
    LED_init();
    EVSYS_init();
    // Configure unused pins to minimize power consumption
    configure_unused_pins();
    
    sleep_init();
    
    while(1) {
        sleep_mode();
    }
    return 0;
};
