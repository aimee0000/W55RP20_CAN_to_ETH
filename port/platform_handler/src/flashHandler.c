#include <string.h>
#include <stdlib.h>
#include "port_common.h"
#include "flashHandler.h"

#ifdef _FLASH_DEBUG_
	#include <stdio.h>
#endif

typedef struct {
    bool op_is_erase;
    uint32_t addr;
    uint8_t *data;
    uint32_t data_len;
} mutation_operation_t;

static uint8_t *flash_buf = NULL;

static void flash_mudation_operation(void *param)
{
    const mutation_operation_t *mop = (const mutation_operation_t *)param;
    uint32_t i, access_len;
    uint32_t prev_intr_state = 0;

    if (mop->op_is_erase == 0) {
        if (mop->data_len && ((mop->data_len % FLASH_SECTOR_SIZE) == 0))
        {
            for (i=0; i<mop->data_len; i+=FLASH_SECTOR_SIZE)
            {
                prev_intr_state = save_and_disable_interrupts();
                flash_range_erase(mop->addr + i, FLASH_SECTOR_SIZE);
                restore_interrupts(prev_intr_state);
                memcpy(flash_buf, mop->data, FLASH_SECTOR_SIZE);
                prev_intr_state = save_and_disable_interrupts();
                flash_range_program(mop->addr + i, flash_buf, FLASH_SECTOR_SIZE);
                restore_interrupts(prev_intr_state);
            }
        }
        else
        {
            for (i=0; i<mop->data_len; i+=FLASH_SECTOR_SIZE)
            {
                memset(flash_buf, 0xFF, FLASH_SECTOR_SIZE);
                read_flash(mop->addr + i, flash_buf, FLASH_SECTOR_SIZE);
                prev_intr_state = save_and_disable_interrupts();
                flash_range_erase(mop->addr + i, FLASH_SECTOR_SIZE);
                restore_interrupts(prev_intr_state);
                access_len = MIN(mop->data_len - i, FLASH_SECTOR_SIZE);
                memcpy(flash_buf, mop->data + i, access_len);
                prev_intr_state = save_and_disable_interrupts();
                flash_range_program(mop->addr + i, flash_buf, FLASH_SECTOR_SIZE);
                restore_interrupts(prev_intr_state);
            }
        }
    }
    else
    {
        prev_intr_state = save_and_disable_interrupts();
        flash_range_erase(mop->addr, FLASH_SECTOR_SIZE);
        restore_interrupts(prev_intr_state);
    }
}

void write_flash(uint32_t addr, uint8_t * data, uint32_t data_len)
{
    mutation_operation_t mop;

    mop.op_is_erase = 0;
    mop.addr = addr;
    mop.data = data;
    mop.data_len = data_len;

    flash_buf = malloc(FLASH_SECTOR_SIZE);
    memset(flash_buf, 0x00, FLASH_SECTOR_SIZE);
    flash_mudation_operation(&mop);
    free(flash_buf);
}

void read_flash(uint32_t addr, uint8_t *data, uint32_t data_len)
{
    addr += XIP_BASE;
    memcpy(data, addr, data_len);
}

void erase_flash_sector(uint32_t addr)
{
    mutation_operation_t mop;

    mop.op_is_erase = 1;
    mop.addr = addr;
    flash_mudation_operation(&mop);
}