; This did not work
; i've got it to compile when inlining it into a c-function
; but it dit not work there. It truncates all the addresses like NVMCON1 to 8 bits
; got around that problem by changeing to a newer compiler.
; but utimately it never worked. can't spend days upon days to figure it out.
;    
; https://onlinedocs.microchip.com/pr/%20GUID-BB433107-FD4E-4D28-BB58-9D4A58955B1A-en-US-1/index.html?GUID-E54D9557-D9CF-4669-B97F-80AAC64AB1EF

;#include <xc.inc>
    
;GLOBAL _asm_write_eeprom
;SIGNAT _asm_write_eeprom,4217
    
;PSECT mytext,local,class=CODE,reloc=2

;_asm_write_eeprom:
;    stack_param addr, 1
;    stack_param value, 1
;    alloc_stack
;    clrf NVMCON1                ; clear NVMCON1
;    movf addr_offset      ; move address into wreg/accmulator
;    movwf NVMADRL               ; store it in NVAADRL witch is address to store the byte
;    movf value_offset      ; load the data to be stored into wreg/accmulator
;    movwf NVMDAT                ; move the data to the register that saves it
    
;    bsf NVMCON1, 2              ; enable writes. 2 = WREN
    
;    bcf INTCON0, 7              ; diable interrupts. page 135. 7=GIE
    
    ; unlock sequence
;    movlw 0x55                  ; move literal to wreg/acc
;    movwf NVMCON2               ; set 0x55 into NVMCON2
;    movlw 0xaa
;    movwf NVMCON2               ; set 0xAA into MVMCON2
    
;    bsf NVMCON1, 1              ; set write bit to begin write. WR = 1
    
;    bsf INTCON0, 7              ; enable interrupts
    
    ; wait for write complete. check for WR-bit to clear of NVMCON1 register
;_loop:
;    btfsc NVMCON1, 1            ; WR = bit 1
;    goto _loop
    
;    bcf NVMCON1, 2             ; disable writes. 2 = WREN
;    restore_stack
;    return