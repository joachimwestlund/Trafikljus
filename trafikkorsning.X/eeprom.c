
#include "eeprom.h"

void write_eeprom(uint8_t addr, uint8_t data) {
    
    // save the value of global interrutps enable
    uint8_t GIEValue = INTCON0bits.GIE;
    
    NVMADRL = (uint8_t)(addr & 0xff);   // set address
    NVMDAT = data;                      // set data
    
    NVMCON1bits.NVMREG = 0;             // access EEPROM memory locations
    NVMCON1bits.WREN = 1;               // program/erase enable bit
    
    INTCON0bits.GIE = 0;                // disable interrupts
    
    // unlock sequence
    NVMCON2 = 0x55;
    NVMCON2 = 0xaa;
    NVMCON1bits.WR = 1;                 // Write control bit
    
    // wait for write to complete
    while(NVMCON1bits.WR) {}
    
    NVMCON1bits.WREN = 0;               // inhibit program/erase
    
    INTCON0bits.GIE = GIEValue;         // restore interrupts
}

uint8_t read_eeprom(uint8_t addr)
{
    NVMADRL = (uint8_t)(addr & 0xff);   // set address
    NVMCON1bits.NVMREG = 0;             // access EEPROM memory locations
    NVMCON1bits.RD = 1;                 // Read control bit
    NOP();
    NOP();                              // may be required for latancy at high freq.
    
    return ((uint8_t)NVMDAT);
}