#ifndef __FLASHHANDLER_H__
#define __FLASHHANDLER_H__

#include <stdint.h>
//#define _FLASH_DEBUG_

#include "pico/stdlib.h"
#include "pico/flash.h"
#include "hardware/flash.h"

// RP2040
/*
 * Flash
 *  - Main flash size: 2Mbytes
 *  
 *
 Top Flash Memory address /-------------------------------------------\  0x10200000
                          |                                           |
                          |                                           |
                          |                                           |
                          |              Reserved (448KB)             |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |-------------------------------------------|  0x10090000
                          |                                           |
                          |              Parameters (64KB)            |
                          |                                           |
                          |-------------------------------------------|  0x10080000
    Page   1 (256B)       |                                           |
                          |                                           |
                          |           Application Bank (512KB)        |  
    Page   0 (256B)       |                                           |
                          |                                           |
                          \-------------------------------------------/  0x10000000
*/

#define FLASH_SIZE                0x00200000
#define FLASH_PARAMETER_SIZE      0x10000       // 64KB
#define FLASH_APP_BANK_SIZE       0x80000       //512KB

#define FLASH_DEV_INFO_ADDR FLASH_APP_BANK_SIZE

void write_flash(uint32_t addr, uint8_t * data, uint32_t data_len);
void read_flash(uint32_t addr, uint8_t *data, uint32_t data_len);
void erase_flash_sector(uint32_t addr);
void Copy_Interrupt_VectorTable(uint32_t vtor);


#endif
