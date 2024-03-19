/**
 * This file is used for the timeing and control of the traffic lights.
 * 
 * @details For the course processorkunskap
 * @date 2023-09-03
 * @author Joachim Westlund
 */

#ifndef __TRAFFIC_LIGHTS_H
#define __TRAFFIC_LIGHTS_H

#include <stdint.h>
#include <xc.h>

#include "eeprom.h"

/**
 * Initialization of the traffic lights.
 * Timer 1 is set for 1 second interrupt time.
 * 
 * PORT D is used for traffic light LEDs
 * pin 7 = red for cars
 * pin 6 = yellow for cars
 * pin 5 = green for cars
 * pin 4 = red for pedestrians
 * pin 3 = green for pedestrians
 * pin 1 = timer 1 second indicator
 * pin 0 = button pushed by pedestrian
 * 
 * PORT B
 * Pin 0 = digital input for a push button.
 */
void traffic_lights_init(void);
/**
 * This function handles almost all logic for the traffic lights.
 * the main while-loop checks if the pedestrian button is pressed.
 */
void traffic_light_logic(void);

/**
 * Reset the timer and start counting seconds from zero again.
 */
void reset_timer1(void);

/**
 * This is the interrupt service routine for timer1
 * All it does is resetting the counter value for itself and count up our seconds counter,
 * and flipps a LED.
 * This interrupt has high priority (set in main).
 */
void __interrupt(irq(IRQ_TMR1),base(8)) tmr1_isr(void);

/** value to be use to set a 1s counter for timer 1 */
uint16_t timer1_counter_value;

/** traffic lights state enumeration */
typedef enum 
{
    CAR_GREEN,
    CAR_YELLOW,
    CAR_RED,
    BUTTON_PRESSED,
    GREEN_PEDESTRIAN,
    YELLOW_REVERSE_ORDER
} traffic_lights_state;

/** the state variable to check against. */
traffic_lights_state tf_state;

/** Seconds counter */
uint8_t s_counter;

/** setting for how many seconds the transition between lights should be */
uint8_t s_transition_counter;

/** setting for how long the red light should be shown for cars */
uint8_t s_red_counter;

/** setting for how long the time is between button pressed to yellow light */
uint8_t s_button_pressed_counter;

#endif // __TRAFFIC_LIGHTS_H