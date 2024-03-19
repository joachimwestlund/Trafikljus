/**
 * Uart implementation on UART1.
 * 
 * Used as a debug tool and will also accepts commands.
 * 115200 bps 8N1
 * 
 * Registers at page 499 in datasheet for PIC18F
 * 
 * @date 2023-08-29
 * @author: Joachim Westlund
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __UART_H
#define	__UART_H

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "eeprom.h"
#include "traffic_lights.h"

/**
 * Setup and initialize the UART1 port.
 * 115200 8N1
 * TX on pin RB4
 * RX on pin RB5
 */
void uart1_init(void);

/**
 * Read from uart.
 * This is not used in this program.
 * @return byte read from uart1
 */
uint8_t uart1_read();

/**
 * Write to uart1.
 * @param tx_data the byte to be written
 */
void uart1_write(uint8_t tx_data);

/**
 * Write a c-string to uart1
 * @param s the c-string to be written.
 */
void uart1_write_string(uint8_t* s);

/**
 * If a commmand was recieved on uart1 this function will handle it.
 */
void command_exec(void);

/**
 * Interrupt service routine for RX on uart1.
 * 
 * It receives to a buffer until a '\n' character is received.
 * When '\n' char is found the uart_command_flag will be set to 1.
 * This flag is cleared the the command_exec() function.
 * It will also copy the memory from the input buffer to a command buffer to be used later.
 * by the command_exec() function.
 * 
 * The buffer is 64 bytes long. If more chars than 64 is received the buffer
 * will continue from the beginning and write over earlier bytes.
 * 
 * low priority interrupt (set in main).
 */
void __interrupt(irq(IRQ_U1RX),base(8)) isr_uart1_rx(void);

/** input receive buffer */
uint8_t uart_rx_buffer[32];
/** index of the position in the input buffer */
uint8_t buffer_index = 0;
/** flag to indicate a command been received */
uint8_t uart_command_flag = 0;
/** command buffer */
uint8_t command[32];

#endif	__UART_H /* __UART_H */

