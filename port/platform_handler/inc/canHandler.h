#ifndef _CANHANDLER_H_
#define _CANHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include "port_common.h"
#include "can.h"
#include "configData.h"
#include "wizchip_conf.h"

#define CAN_PIO_INDEX       0 
#define CAN_RX_PIN          4
#define CAN_TX_PIN          5

#define DATA_BUF_MAX_SIZE (1024 * 2)

#define CAN_ERR_FORMAT          -1
#define CAN_ERR_ID_OVERFLOW     -2
#define CAN_ERR_FILTER          -3

typedef struct _CAN_FILTER 
{
    CAN_SPEC        spec;   // 2.0A or 2.0B
    uint32_t        filter_id;
    uint32_t        filter_mask;
} CAN_FILTER;

typedef struct _CAN_RX_RBUF
{
    bool            rx_flag;
    uint32_t        push_idx;
    uint32_t        pop_idx;
    struct can2040_msg msg_buf[DATA_BUF_MAX_SIZE];
} CAN_RX_RBUF;

void set_can_filter(CAN_SPEC spec, uint32_t filter_id, uint32_t mask);
void push_rbuf(struct can2040_msg *msg);
int pop_rbuf(struct can2040_msg *msg);

int can_initialize(CanConfig *p_can_config);
int can_restart(CanConfig *p_can_config);
void PIOx_IRQHandler(void);
int send_can_msg(char *msg_buf);

#endif /* _CANHANDLER_H_ */