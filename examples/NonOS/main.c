/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "w5x00_gpio_irq.h"

// #include "socket.h"
// #include "can.h" 
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/irq.h"
#include "hardware/structs/pio.h"

#include "web_page.h"
#include "configData.h"

#include "httpServer.h"
#include "can_to_eth.h"
#include "canHandler.h"
#include "flashHandler.h"
/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define HTTP_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define HTTP_SOCKET_MAX_NUM 4


/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 11, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 11, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

/* HTTP */
static uint8_t g_http_send_buf[HTTP_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_recv_buf[HTTP_BUF_MAX_SIZE] = {
    0,
};

uint8_t g_tx_buf[DATA_BUF_MAX_SIZE] = {
    0,
};
uint8_t g_rx_buf[DATA_BUF_MAX_SIZE] = {
    0,
};

static uint8_t g_http_socket_num_list[HTTP_SOCKET_MAX_NUM] = {4, 5, 6, 7};

static uint8_t destip[4] = {192, 168, 11, 3};
static uint16_t destport = 5000;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);
static void device_raw_reboot(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
    /* Initialize */
    uint8_t i = 0;
    int retval = 0;
    DevConfig *p_dev_config;

    set_clock_khz();

    stdio_init_all();

    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();
    
    load_DevConfig_from_storage();
    Net_Conf();

    p_dev_config = get_DevConfig_pointer();

    if(can_initialize(&p_dev_config->can_config) < 0)
    {
        printf("CAN setup failed...\n");
        while(1){}
    }

    set_can_filter(p_dev_config->can_config.spec, p_dev_config->can_config.filt_id, p_dev_config->can_config.filt_mask);

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);
    reg_httpServer_cbfunc(device_raw_reboot, NULL);

    /* Register web page */
    reg_httpServer_webContent("index.html", _acindex);

    /* Infinite loop */
    while (1)
    {
        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++)
        {
            httpServer_run(i);
        }
        
        if ((retval = can_to_eth(p_dev_config->connection_config.working_mode)) < 0)
        {
           printf(" CAN to Eth loopback error : %d\n", retval);

           while (1)
                ;
        }
    }

}


/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

static void device_raw_reboot(void)
{
    watchdog_reboot(0, SRAM_END, 10);
    while(1);
}

