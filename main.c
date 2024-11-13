/*
 * File:   main.c
 * Author: markus-klund
 *
 * Created on November 9, 2024, 8:38 AM
 */

#include <avr/sleep.h> // importing avr library for sleep mode functions

// function for setting up the AC to compare the light sensor output with a reference voltage
void AC_init(){    
    PORTD.DIRCLR = PIN2_bm; // setting pin PD2 (port D, pin 2) as input
    PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc; // disabling digital input buffer and pull up resistor for pin PD2
    AC0.MUXCTRL = AC_MUXNEG_DACREF_gc | AC_MUXPOS_AINP0_gc; // control signals to MUX for setting the negative AC input to DACREF and positive AC input to AINP0/PD2
    AC0.DACREF = 25; // using provided formula and solving for DACREF (DACREF = (V_DACREF*256)/V_REF = (0.1*256)/1.024 = 25)
    AC0.CTRLA = AC_ENABLE_bm | AC_RUNSTDBY_bm; // enabling the AC and making it run in standby mode
}

// function for setting the reference voltage
void VREF_init() {
    VREF.ACREF = VREF_REFSEL_1V024_gc; // setting the reference voltage for the AC to 1.024 volt
}

// function for configuring PA2 as an output to control the LED so that it can be toggled based on the AC output
void LED_init() {
    PORTA.DIRSET = PIN2_bm; // setting PA2 as an output for driving the LED
}

// function for setting up the event system to route the AC output to PA2 (the LED pin) without using CPU
void EVSYS_init() {
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc; // setting up event channel 0 with the AC as event generator
    EVSYS.USEREVSYSEVOUTA = EVSYS_USER_CHANNEL0_gc; // setting the event user to use event channel 0 and forward the event signal to PA2 using EVOUTA on Port A
}

// function for initializing sleep mode
void sleep_init() {
    set_sleep_mode(SLEEP_MODE_STANDBY); // setting the sleep mode to standby
}

// function for disabling the input buffer and enables pull up resistors on unused pins to reduce power consumption
void disable_unused_pins() {
    PORTA.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc | PORT_PULLUPEN_bm; // configuring unused pins to disable digital input buffers and enable pull up resistors

    // applying the configuration to unused pins on each port, except for the active pins PA2 and PD2
    PORTA.PINCTRLUPD = 0xFD; // excluding PA2 for LED
    PORTB.PINCTRLUPD = 0xFF; // all pins unused
    PORTC.PINCTRLUPD = 0xFF; // all pins unused
    PORTD.PINCTRLUPD = 0xFB; // excluding PD2 for sensor input
    PORTE.PINCTRLUPD = 0xFF; // All pins unused
}


int main(){
    VREF_init(); // initializing the voltage reference for the AC
    AC_init(); // initializing the AC
    LED_init(); // setting up PA2 as an output
    EVSYS_init(); // initializing the event system
    disable_unused_pins(); // disabling the unused pins for lower power consumption
    sleep_init(); // setting to standby mode
    
    // entering standby sleep mode while the LED is handled by the event system based on comparator output
    while(1) {
        sleep_mode();
    }
    return 0;
};
