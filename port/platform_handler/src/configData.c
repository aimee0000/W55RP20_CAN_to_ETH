#include <string.h>
#include <stdlib.h>

#include "port_common.h"
#include "configData.h"
#include "flashHandler.h"
#include "wizchip_conf.h"

#define CONFIG_DEBUG

static DevConfig dev_config;

#ifdef CONFIG_DEBUG
#include "w5x00_spi.h"
static void print_Devconfig(DevConfig *p_dev_config);
#endif

DevConfig* get_DevConfig_pointer(void)
{
	return &dev_config;
}

void set_DevConfig_to_factory_value(void)
{
    memset(&dev_config, 0x0, sizeof(DevConfig));

    dev_config.packet_size = sizeof(DevConfig);

    dev_config.network_config.ip[0] = 192;
    dev_config.network_config.ip[1] = 168;
    dev_config.network_config.ip[2] = 11;
    dev_config.network_config.ip[3] = 2;

    dev_config.network_config.gateway[0] = 192;
    dev_config.network_config.gateway[1] = 168;
    dev_config.network_config.gateway[2] = 11;
    dev_config.network_config.gateway[3] = 1;

    dev_config.network_config.subnet[0] = 255;
    dev_config.network_config.subnet[1] = 255;
    dev_config.network_config.subnet[2] = 255;
    dev_config.network_config.subnet[3] = 0;

    dev_config.network_config.dns_server_ip[0] = 8; // Default DNS server IP: Google Public DNS (8.8.8.8)
    dev_config.network_config.dns_server_ip[1] = 8;
    dev_config.network_config.dns_server_ip[2] = 8;
    dev_config.network_config.dns_server_ip[3] = 8;

    //dev_config.network_config.dhcp_use = DISABLE;

    dev_config.connection_config.working_mode = ETH_TCP_SERVER;     // TCP Server mode
    dev_config.connection_config.local_port = 4001;    //5000;
    dev_config.connection_config.remote_port = 4001;   //5000;
    dev_config.connection_config.remote_ip[0] = 192;
    dev_config.connection_config.remote_ip[1] = 168;
    dev_config.connection_config.remote_ip[2] = 11;
    dev_config.connection_config.remote_ip[3] = 3;

    dev_config.can_config.baudrate = CAN_BDRATE_125;               // 125kbps
    dev_config.can_config.spec = CAN_SPEC_A;
    dev_config.can_config.filt_id = 0;
    dev_config.can_config.filt_mask = 0;

#ifdef CONFIG_DEBUG
    printf("\tSet Dev Config to factory value ->\n");
    print_Devconfig(&dev_config);
#endif
}

void load_DevConfig_from_storage(void)
{
    // read
    read_flash(FLASH_DEV_INFO_ADDR, &dev_config, sizeof(DevConfig));
    
#ifdef CONFIG_DEBUG
    printf("\tLoad Dev Config from storage ->\n");
    print_Devconfig(&dev_config);
#endif

    // check
    if((dev_config.packet_size == 0) || (dev_config.packet_size == UINT32_MAX) || (dev_config.packet_size != sizeof(DevConfig)))
    {
        set_DevConfig_to_factory_value();
        save_DevConfig_to_storage();
    }

}

void save_DevConfig_to_storage(void)
{
    write_flash(FLASH_DEV_INFO_ADDR, &dev_config, sizeof(DevConfig));

#ifdef CONFIG_DEBUG
    DevConfig tmp_config;

    memset(&tmp_config, 0x0, sizeof(DevConfig));
    read_flash(FLASH_DEV_INFO_ADDR, &tmp_config, sizeof(DevConfig));
    printf("\tAfter Save Dev Config to storage ->\n");
    print_Devconfig(&tmp_config);
#endif

}

void Net_Conf(void)
{
    DevConfig *p_dev_config = get_DevConfig_pointer();
    wiz_NetInfo gWIZNETINFO;

    gWIZNETINFO.mac[0] = 0x00;
    gWIZNETINFO.mac[1] = 0x08;
    gWIZNETINFO.mac[2] = 0xDC;
    gWIZNETINFO.mac[3] = 0x12;
    gWIZNETINFO.mac[4] = 0x34;
    gWIZNETINFO.mac[5] = 0x56;

    get_DevConfig_value(gWIZNETINFO.ip, p_dev_config->network_config.ip, sizeof(gWIZNETINFO.ip[0]) * 4);
    get_DevConfig_value(gWIZNETINFO.gw, p_dev_config->network_config.gateway, sizeof(gWIZNETINFO.gw[0]) * 4);
    get_DevConfig_value(gWIZNETINFO.sn, p_dev_config->network_config.subnet, sizeof(gWIZNETINFO.sn[0]) * 4);
    get_DevConfig_value(gWIZNETINFO.dns, p_dev_config->network_config.dns_server_ip, sizeof(gWIZNETINFO.dns));
    
    //if(p_dev_config->network_config.dhcp_use)
    //    gWIZNETINFO.dhcp = NETINFO_DHCP;
    //else
        gWIZNETINFO.dhcp = NETINFO_STATIC;

    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

#ifdef CONFIG_DEBUG
    print_network_information(gWIZNETINFO);
#endif
}

void get_DevConfig_value(void *dest, const void *src, uint16_t size)
{
    memcpy(dest, src, size);
}

void set_DevConfig_value(void *dest, const void *value, const uint16_t size)
{
    memcpy(dest, value, size);
}

#ifdef CONFIG_DEBUG
static void print_Devconfig(DevConfig *p_dev_config)
{
    printf("\n-------------------------------------\n");
    printf("         Dev Config           \n\n");
    printf("packet size : %lu\n", p_dev_config->packet_size);

    printf("<Network Config>\n");
    printf("\tIP : %d.%d.%d.%d\n", p_dev_config->network_config.ip[0], p_dev_config->network_config.ip[1], \
                                        p_dev_config->network_config.ip[2], p_dev_config->network_config.ip[3]);
    printf("\tGW : %d.%d.%d.%d\n", p_dev_config->network_config.gateway[0], p_dev_config->network_config.gateway[1], \
                                        p_dev_config->network_config.gateway[2], p_dev_config->network_config.gateway[3]);
    printf("\tSubnet : %d.%d.%d.%d\n", p_dev_config->network_config.subnet[0], p_dev_config->network_config.subnet[1], \
                                        p_dev_config->network_config.subnet[2], p_dev_config->network_config.subnet[3]);
    printf("\tDNS : %d.%d.%d.%d\n", p_dev_config->network_config.dns_server_ip[0], p_dev_config->network_config.dns_server_ip[1], \
                                        p_dev_config->network_config.dns_server_ip[2], p_dev_config->network_config.dns_server_ip[3]);

    printf("<Network Connection>\n");
    printf("\tmode : %u\n", p_dev_config->connection_config.working_mode);
    switch(p_dev_config->connection_config.working_mode)
    {
        case ETH_TCP_SERVER:
        printf("\tmode : TCP Server\n");
        break;
        case ETH_TCP_CLIENT:
        printf("\tmode : TCP Client\n");
        break;
        default:
        break;
    }

    printf("\tlocal port : %u\n", p_dev_config->connection_config.local_port);
    printf("\tremote port : %u\n", p_dev_config->connection_config.remote_port);
    printf("\tremote IP : %d.%d.%d.%d\n", p_dev_config->connection_config.remote_ip[0], p_dev_config->connection_config.remote_ip[1], \
                                        p_dev_config->connection_config.remote_ip[2], p_dev_config->connection_config.remote_ip[3]);


    printf("<CAN Config>\n");
    switch (p_dev_config->can_config.baudrate)
    {
        case CAN_BDRATE_125:
            printf("\tbaudrate : 125kbps\n");
            break;
        case CAN_BDRATE_250:
            printf("\tbaudrate : 250kbps\n");
            break;
        case CAN_BDRATE_500:
            printf("\tbaudrate : 500kbps\n");
            break;
        default:
            printf("\tErr) baudrate : %u\n", p_dev_config->can_config.baudrate);
            break;
    }
    switch (p_dev_config->can_config.spec)
    {
        case CAN_SPEC_A:
            printf("\tspec : 2.0 A\n");
            break;
        case CAN_SPEC_B:
            printf("\tspec : 2.0 B\n");
            break;
        default:
            printf("\tErr) spec : %u\n", p_dev_config->can_config.spec);
            break;
    }
    printf("\tfilter ID : 0x%x\n", p_dev_config->can_config.filt_id);
    printf("\tfilter Mask : 0x%x\n", p_dev_config->can_config.filt_mask);

    printf("-------------------------------------\n\n");
}
#endif