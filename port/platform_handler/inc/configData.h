
#ifndef _CONFIGDATA_H_
#define _CONFIGDATA_H_

#include <stdio.h>
#include <stdlib.h>
#include "port_common.h"

// wiz_NetInfo g_net_info =
// {
//         .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
//         .ip = {192, 168, 11, 2},                     // IP address
//         .sn = {255, 255, 255, 0},                    // Subnet Mask
//         .gw = {192, 168, 11, 1},                     // Gateway
//         .dns = {8, 8, 8, 8},                         // DNS server
//         .dhcp = NETINFO_STATIC                       // DHCP enable/disable
// };

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;

typedef enum _ETH_MODE {
  ETH_TCP_SERVER = 0,
  ETH_TCP_CLIENT,

  ETH_MODE_MAX
} ETH_MODE;

typedef enum _CAN_BDRATE {
  CAN_BDRATE_125,
  CAN_BDRATE_250,
  CAN_BDRATE_500,

  CAN_BDRATE_MAX
} CAN_BDRATE;

typedef enum _CAN_SPEC {
  CAN_SPEC_A,   // STD (11bit)
  CAN_SPEC_B,   // EXT (29bit)

  CAN_SPEC_MAX
} CAN_SPEC;

typedef struct _NetworkConfig {
	uint8_t         ip[4];
	uint8_t         gateway[4];
	uint8_t         subnet[4];
  uint8_t         dns_server_ip[4];
  //uint8_t         dhcp_use;
} __attribute__((packed)) NetworkConfig;

typedef struct _ConnectionConfig {
	uint8_t         working_mode;
  uint16_t        local_port;
  uint8_t         remote_ip[4];
  uint16_t        remote_port;
} __attribute__((packed)) ConnectionConfig;

typedef struct _CanConfig {
  CAN_BDRATE      baudrate;
  CAN_SPEC        spec;   // 2.0A or 2.0B
  uint32_t        filt_id;
  uint32_t        filt_mask;
} __attribute__((packed)) CanConfig;

typedef struct _DevConfig {
  uint32_t              packet_size;
  NetworkConfig         network_config;
  ConnectionConfig      connection_config;
  CanConfig             can_config;
} __attribute__((packed)) DevConfig;

DevConfig* get_DevConfig_pointer(void);
void set_DevConfig_to_factory_value(void);
void load_DevConfig_from_storage(void);
void save_DevConfig_to_storage(void);
void Net_Conf(void);

// Static ???
void get_DevConfig_value(void *dest, const void *src, uint16_t size);
void set_DevConfig_value(void *dest, const void *value, const uint16_t size);

#endif /* _CONFIGDATA_H_ */
