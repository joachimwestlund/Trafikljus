
#include "traffic_lights.h"

void traffic_lights_init(void)
{
    // start in green state
    tf_state = CAR_GREEN;
    s_counter = 0;
    
    // leds
    TRISD = 0x00;
    ANSELD = 0x00;
    LATD = 0x00;
    
    s_transition_counter = read_eeprom(TRANSITION_TIME);
    if (s_transition_counter == 0 || s_transition_counter == 0xff)
        s_transition_counter = 2;
    
    s_red_counter = read_eeprom(RED_LIGHT_TIME);
    if (s_red_counter == 0 || s_red_counter == 0xff)
        s_red_counter = 8;
    
    s_button_pressed_counter = read_eeprom(BUTTON_PRESSED_TIME);
    if (s_button_pressed_counter == 0 || s_button_pressed_counter == 0xff)
        s_button_pressed_counter = 2;
    
    // button
    TRISBbits.TRISB0 = 1;           // the button on RB0 set as input
    ANSELBbits.ANSELB0 = 0;         // set as digital pin
    
    // Setup timer1 to get an accurate one second delay
    // timer counter value calculated with a clock source of 32KHz
    // (32000^-1) * 0x85ee = 1.071s
    // this setting is very exact for 1s. Checked with oscilloscope.
    timer1_counter_value = 0x85ee;
    
    // Timer1 gate control register. Page 315.
    T1GCON = 0x00;                  
    
    // Select clock source
    T1GATE = 0x00;
    
    // timer 1 clock register. MFINTOSC clock source 32KHz
    T1CLK = 0x06;
        
    TMR1H = (uint8_t)((timer1_counter_value >> 8) & 0xff); // set the timer values to count to
    TMR1L = (uint8_t)((timer1_counter_value) & 0xff);      // before interrupt.
    
    PIR4bits.TMR1IF = 0;            // clear the IF flag before enable interrupts
    
    PIE4bits.TMR1IE = 1;            // Enable timer1 interrupt
    
    T1CONbits.RD16 = 1;             // 16-bit operation
    T1CONbits.CKPS = 0b000;         // 1:1 prescale value
    T1CONbits.NOT_SYNC = 0;         // Sync external clock input with system clock
    T1CONbits.ON = 1;               // start timer
}

void __interrupt(irq(IRQ_TMR1),base(8)) tmr1_isr(void)
{
    // clear the interrupt flag
    PIR4bits.TMR1IF = 0;
    
    // reload the timer
    if (T1CONbits.NOT_SYNC == 1)
    {
        // stop the timer
        T1CONbits.TMR1ON = 0;

        TMR1H = (uint8_t)((timer1_counter_value >> 8) & 0xff); // set the timer values to count to
        TMR1L = (uint8_t)((timer1_counter_value) & 0xff);      // before interrupt.
        // start the timer again
        T1CONbits.TMR1ON = 1;
    }
    else
    {
        // no need to stop and restart timer here
        TMR1H = (uint8_t)((timer1_counter_value >> 8) & 0xff); // set the timer values to count to
        TMR1L = (uint8_t)((timer1_counter_value) & 0xff);      // before interrupt.
    }
    
    LATDbits.LATD1 = ~LATDbits.LATD1;
    s_counter++;
}

void reset_timer1(void)
{
        // stop the timer
        T1CONbits.TMR1ON = 0;

        TMR1H = (uint8_t)((timer1_counter_value >> 8) & 0xff); // set the timer values to count to
        TMR1L = (uint8_t)((timer1_counter_value) & 0xff);      // before interrupt.
        
        // start the timer again
        T1CONbits.TMR1ON = 1;
        
        s_counter = 0;
}

void traffic_light_logic(void)
{
    switch(tf_state)
    {
        case CAR_GREEN:
            LATDbits.LATD5 = 1;     // green for cars
            LATDbits.LATD6 = 0;     // yellow for cars
            LATDbits.LATD7 = 0;     // red for cars
            LATDbits.LATD4 = 1;     // red for pedestrians
            LATDbits.LATD3 = 0;     // green for pedestrians
            LATDbits.LATD0 = 0;     // pedestrian indicator
            break;
        case CAR_YELLOW:
            if (s_counter == s_button_pressed_counter)     // wait time before changeing to yellow.
            {
                LATDbits.LATD5 = 0;     // green for cars
                LATDbits.LATD6 = 1;     // yellow for cars
                LATDbits.LATD7 = 0;     // red for cars
                LATDbits.LATD4 = 1;     // red for pedestrians
                LATDbits.LATD3 = 0;     // green for pedestrians
                reset_timer1();
                tf_state = CAR_RED;
            }
            break;
        case CAR_RED:
            if (s_counter == s_transition_counter)
            {
                LATDbits.LATD5 = 0;     // green for cars
                LATDbits.LATD6 = 0;     // yellow for cars
                LATDbits.LATD7 = 1;     // red for cars
                LATDbits.LATD4 = 0;     // red for pedestrians
                LATDbits.LATD3 = 1;     // green for pedestrians
                reset_timer1();
                tf_state = GREEN_PEDESTRIAN;
            }
            break;
        case GREEN_PEDESTRIAN:
            if (s_counter == s_red_counter)
            {
                LATDbits.LATD5 = 0;     // green for cars
                LATDbits.LATD6 = 1;     // yellow for cars
                LATDbits.LATD7 = 0;     // red for cars
                LATDbits.LATD4 = 1;     // red for pedestrians
                LATDbits.LATD3 = 0;     // green for pedestrians
                reset_timer1();
                tf_state = YELLOW_REVERSE_ORDER;
            }
            break;
        case YELLOW_REVERSE_ORDER:
            if (s_counter == s_transition_counter)
            {
                reset_timer1();
                tf_state = CAR_GREEN;
            }
            break;
        case BUTTON_PRESSED:
            LATDbits.LATD0 = 1;     // button pressed indicator
            reset_timer1();
            tf_state = CAR_YELLOW;
            break;
    }
    
}