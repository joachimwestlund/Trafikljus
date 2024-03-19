#include "uart.h"

void uart1_init(void)
{
    U1CON1bits.ON = 0;                  // Turn of UART1

    // Setup PPS module and pin selection
    // page 278 - 282 in data sheet.
    ANSELBbits.ANSELB4 = 0;             // Digital output on RB4
    ANSELBbits.ANSELB5 = 0;             // Digital input on RB5

    TRISBbits.TRISB4 = 0;               // RB4 as output
    RB4PPS = 0b010011;                  // TX on RB4 pin 0x13 output
    
    TRISBbits.TRISB5 = 1;               // RB5 as input
    U1RXPPSbits.U1RXPPS = 0b00001101;   // page 278 RX input on RB5
    
    // Setup UART1 registers    
    U1CON0bits.TXEN = 1;                // Transmit enable.
    U1CON0bits.RXEN = 1;                // Receiver enable.
    U1CON0bits.BRGS = 1;                // Baud rate generator high speed.
    U1CON0bits.ABDEN = 0;               // No auto baud detect
    U1CON0bits.MODE = 0b0000;           // Asynchronous 8-bit UART mode.
    
    // Baud rate error calculation:
    //
    // Desired baud rate = 115200
    // BRGS = 1
    //
    // Desired baud rate = Fosc / (4(n + 1))
    // n = (Fosc / Desired baud rate / 4) - 1
    // n = (8000000 / 115200 / 4) - 1 =approx 16 (put this is U1BRGL)
    //
    // Calculated baud rate = 8000000 / (4(16 + 1)) = 117647
    //
    // Error = (Calc.Baud rate - Desired baud rate) / Desired baud rate
    // Error = (117647 - 115200) / 115200 = 0.021 = 2.1%    (too much??)
    // it works! :)
    
    U1BRGL = 16;
    U1BRGH = 0;
    
    PIE3bits.U1RXIE = 1;        // Enable RX interrups
   
    U1CON1bits.ON = 1;          // turn on UART1 
}

// not used
uint8_t uart1_read()
{
    while(!PIR3bits.U1RXIF) {}          // PIRx = Peripheral interrupt status registers.
                                        // page 140. U1RXIF = UART1 Reveive interrupt flag bit
    if (U1ERRIRbits.FERIF == 1)         // UART1 error interrupt flag register
    {                                   // FERIF = framing error interrupt flag
        // restart UART1
        U1CON1bits.ON = 0;              // does this work? is it specified as a solution?
        U1CON1bits.ON = 1;
    }
    return U1RXB;
}

void uart1_write(uint8_t tx_data)
{
    while(!PIR3bits.U1TXIF) {}          // wait if busy
    U1TXB = tx_data;
}

void uart1_write_string(uint8_t* s)
{
    for(int i = 0; i < strlen(s); i++)
    {
        uart1_write(s[i]);
    }
}

void __interrupt(irq(IRQ_U1RX),base(8)) isr_uart1_rx(void)
{
    PIR3bits.U1IF = 0;
    uart_rx_buffer[buffer_index] = U1RXB;
    if (uart_rx_buffer[buffer_index] == 0x0d)   // check for '\n' 
    {
        memcpy(command, uart_rx_buffer, buffer_index);
        command[buffer_index] = 0;
        uart_command_flag = 1;
        buffer_index = 0;
    }
    else
    {
        buffer_index++;
        if (buffer_index >= 64)
            buffer_index = 0;
    }
}

void command_exec(void)
{
    // buffer for reading eeprom
    uint8_t s_buffer[8];
    
    // used to cut upp the command
    // eg. "TT05" will be c_cmd = "TT" and c_param = "05";
    uint8_t c_cmd[3];
    uint8_t c_param[3];
    uint8_t value = 0;
    
    if (strlen(command) != 4)
    {
        uart1_write_string("A command has to be 4 characters long.\r\n");
        uart_command_flag = 0;
        return;
    }

    c_cmd[0] = command[0];
    c_cmd[1] = command[1];
    c_cmd[2] = 0;
    c_param[0] = command[2];
    c_param[1] = command[3];
    c_param[2] = 0;

    if (strcmp("LED2", command) == 0)
    {
        LATDbits.LATD2 = ~LATDbits.LATD2;
        uart1_write_string("OK.\r\n");
    }
    else if (strcmp("TT", c_cmd) == 0)
    {
        value = (uint8_t)atoi(c_param);
        if (value == 0)
        {
            uart1_write_string("Not a valid number\r\n");
            uart_command_flag = 0;
            return;
        }
        write_eeprom(TRANSITION_TIME, value);
        s_transition_counter = value;
        uart1_write_string("OK.\r\n");
    }
    else if (strcmp("RT", c_cmd) == 0)
    {
        value = (uint8_t)atoi(c_param);
        if (value == 0)
        {
            uart1_write_string("Not a valid number\r\n");
            uart_command_flag = 0;
            return;
        }
        write_eeprom(RED_LIGHT_TIME, value);
        s_red_counter = value;
        uart1_write_string("OK.\r\n");
    }
    else if (strcmp("BP", c_cmd) == 0)
    {
        value = (uint8_t)atoi(c_param);
        if (value == 0)
        {
            uart1_write_string("Not a valid number\r\n");
            uart_command_flag = 0;
            return;
        }
        write_eeprom(BUTTON_PRESSED_TIME, value);
        s_button_pressed_counter = value;
        uart1_write_string("OK.\r\n");
    }
    else if (strcmp("RERT", command) == 0)
    {
        sprintf(s_buffer, "%d\r\n", read_eeprom(RED_LIGHT_TIME));
        uart1_write_string(s_buffer);
    }
    else if (strcmp("RETT", command) == 0)
    {
        sprintf(s_buffer, "%d\r\n", read_eeprom(TRANSITION_TIME));
        uart1_write_string(s_buffer);
    }
    else if (strcmp("REBP", command) == 0)
    {
        sprintf(s_buffer, "%d\r\n", read_eeprom(BUTTON_PRESSED_TIME));
        uart1_write_string(s_buffer);
    }
    else
    {
        uart1_write_string("COMMANDS:\r\n"
                "LED2    - Toggle LED2\r\n"
                "TTxx    - Transition time, xx = a value 01 to 99 seconds.\r\n"
                "RTxx    - Red light time, xx = a value 01 to 99 seconds.\r\n"
                "BPxx    - Button press to yellow time, xx = a value 01 to 99 seconds.\r\n"
                "RERT    - Read current red light time from EEPROM.\r\n"
                "RETT    - Read current transition time from EEPROM.\r\n"
                "REBP    - Read current button to yellow transition time from EEPROM.\r\n");
    }
    
    uart_command_flag = 0;
}