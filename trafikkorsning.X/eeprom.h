/**
 * EEPROM implementation.
 * 
 * page 206 in datasheet
 * 
 * @date 2023-09-08
 * @author: Joachim Westlund
 */

#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdint.h>
#include <xc.h>

/** Memory address in EEPROM */
#define TRANSITION_TIME 0x00
/** Memory address in EEPROM */
#define RED_LIGHT_TIME  0x01
/** Memory address in EEPROM */
#define BUTTON_PRESSED_TIME 0x02

/**
 * Writes a byte to EEPROM memory
 * @param addr the address to write to
 * @param data the byte to be written
 */
void write_eeprom(uint8_t addr, uint8_t data);


/**
 * Reads a byte from EEPROM
 * @return the byte read.
 */
uint8_t read_eeprom(uint8_t addr);

#endif // __EEPROM_H