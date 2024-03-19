/**
 * @mainpage
 * @brief Trafik-korsning
 * @details For the course processorkunskap
 * @date 2023-08-29
 * @author Joachim Westlund
 */

#pragma config FEXTOSC = HS     // page 92 of datasheet
#pragma config RSTOSC = EXTOSC
#pragma config LVP = OFF
#pragma config WDTE = OFF
#pragma config WRTD = OFF       // write protect EEPROM off

#define _XTAL_FREQ  8000000     // 8MHz external crystal clock.

#include "traffic_lights.h"
#include "uart.h"
#include "eeprom.h"

/**
 * Default EEPROM data
 * Byte 0: transition time in seconds for traffic lights
 * Byte 1: red light time in seconds for traffic lights
 * Byte 2: how long after the button is pushed it transitions to yellow light
 * Byte n: Not used
 */
__EEPROM_DATA(4, 16, 11, 0x00, 0x00, 0x00, 0x00, 0x00);   

// this is the failed asm eeprom function declaration
//extern void asm_write_eeprom(uint8_t addr, uint8_t value);

/**
 * Default interrupt for all interrupts not being used.
 * Removes alot a warnings when compiling
 */
__interrupt(irq(default),base(8)) Default_ISR(void)
{}

/**
 * @brief This is the main entry of the program.
 */
void main(void) {
     
    // initialize interrupts and set up interrupt vector table
    INTCON0bits.GIEL = 1;       // Enable low priority interrupts.
    INTCON0bits.GIEH = 1;       // Enable high priority interrupts.
    INTCON0bits.IPEN = 1;       // Enable interrupt priority
    
    uint8_t state = (uint8_t)GIE;
    GIE = 0;
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x00; // unlock IVT
    
    IVTBASEU = 0;
    IVTBASEH = 0;
    IVTBASEL = 8;   // place the vector table at default address. 0x0008
                    // this initialization of vector table is not needed because we
                    // are using the default one. But for future code reuse, i will keep it.
    
    IVTLOCK = 0x55;
    IVTLOCK = 0xAA;
    IVTLOCKbits.IVTLOCKED = 0x01; // lock IVT
    
    GIE = state;
    
    IPR4bits.TMR1IP = 1;    // high priority for timer1
    IPR3bits.U1RXIP = 0;    // low priority for uart1 RX
    
    traffic_lights_init();
    uart1_init();
    
    uart1_write_string("\r\n\r\n");
    uart1_write_string("Trafikkorsning.\r\n\r\n");
    uart1_write_string("Inlämningsuppgift i kursen processorkunskap.\r\n");
    uart1_write_string("Av Joachim Westlund.\r\n\r\n");
    uart1_write_string("UART1 initialized.\r\n");
    uart1_write_string("Traffic lights initialized.\r\n\r\n");
    uart1_write_string("Type \"help\" for commands.\r\n\r\n");
    
    while(1)
    {
        // was the button pressed?
        if (PORTBbits.RB0 == 0 && tf_state == CAR_GREEN)
        {
            tf_state = BUTTON_PRESSED;
        }
        
        traffic_light_logic();
        
        // did we receive a commmand on the uart?
        if (uart_command_flag)
            command_exec();
    }
}
