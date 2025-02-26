#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "configData.h"
#include "httpHandler.h"
#include "httpParser.h"
#include "can_parser.h"

void make_json_devinfo(uint8_t * buf, uint16_t * len)
{
	DevConfig *p_dev_config = get_DevConfig_pointer();

	*len = sprintf((char *)buf, "DevinfoCallback({\"ip\":\"%d.%d.%d.%d\","\
												"\"gw\":\"%d.%d.%d.%d\","\
												"\"sub\":\"%d.%d.%d.%d\","\
												"\"dns\":\"%d.%d.%d.%d\","\
												"\"opmode\":\"%d\","\
												"\"lport\":\"%d\","\				
												"\"rip\":\"%d.%d.%d.%d\","\
												"\"rport\":\"%d\","\
												"\"baud\":\"%d\","\
												"\"spec\":\"%d\","\
												"\"fid\":\"%x\","\
												"\"fmask\":\"%x\","\
                                               "});",
		
		p_dev_config->network_config.ip[0], p_dev_config->network_config.ip[1], p_dev_config->network_config.ip[2], p_dev_config->network_config.ip[3],
		p_dev_config->network_config.gateway[0], p_dev_config->network_config.gateway[1], p_dev_config->network_config.gateway[2], p_dev_config->network_config.gateway[3],
		p_dev_config->network_config.subnet[0], p_dev_config->network_config.subnet[1], p_dev_config->network_config.subnet[2], p_dev_config->network_config.subnet[3],
		p_dev_config->network_config.dns_server_ip[0], p_dev_config->network_config.dns_server_ip[1], p_dev_config->network_config.dns_server_ip[2], p_dev_config->network_config.dns_server_ip[3],
		
		p_dev_config->connection_config.working_mode,
		p_dev_config->connection_config.local_port,
		p_dev_config->connection_config.remote_ip[0], p_dev_config->connection_config.remote_ip[1], p_dev_config->connection_config.remote_ip[2], p_dev_config->connection_config.remote_ip[3],
		p_dev_config->connection_config.remote_port,

		p_dev_config->can_config.baudrate,
		p_dev_config->can_config.spec,
		p_dev_config->can_config.filt_id,
		p_dev_config->can_config.filt_mask
    );
}

uint8_t set_devinfo(uint8_t * uri)
{
	uint8_t buf[8];
	uint8_t ret = 0;
	uint8_t * param;
	uint8_t str_size;
	DevConfig *p_dev_config = get_DevConfig_pointer();
	uint8_t uart_sel = 0;

	// Network config
	if((param = get_http_param_value((char *)uri, "ip", (char*)buf)))
	{
		inet_addr_((unsigned char*)param, p_dev_config->network_config.ip);
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "gw", (char*)buf)))
	{
		inet_addr_((unsigned char*)param, p_dev_config->network_config.gateway);
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "sub", (char*)buf)))
	{
		inet_addr_((unsigned char*)param, p_dev_config->network_config.subnet);
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "dns", (char*)buf)))
	{
		inet_addr_((unsigned char*)param, p_dev_config->network_config.dns_server_ip);
		ret = 1;
	}
  
	// Connection config
	if((param = get_http_param_value((char *)uri, "opmode", (char*)buf)))
	{
		uint8_t eth_mode = ATOI(param, 10);

		if(eth_mode < ETH_MODE_MAX)
			p_dev_config->connection_config.working_mode = eth_mode;

		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "lport", (char*)buf)))
	{
		p_dev_config->connection_config.local_port = ATOI(param, 10);

		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "rip", (char*)buf)))
	{
		inet_addr_((unsigned char*)param, p_dev_config->connection_config.remote_ip);
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "rport", (char*)buf)))
	{
		p_dev_config->connection_config.remote_port = ATOI(param, 10);

		ret = 1;
	}

	// Can Config
	if((param = get_http_param_value((char *)uri, "baud", (char*)buf)))
	{
		uint8_t baudrate_idx = ATOI(param, 10);
		if(baudrate_idx >= CAN_BDRATE_MAX) 
			baudrate_idx = CAN_BDRATE_125;

		p_dev_config->can_config.baudrate = baudrate_idx;
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "spec", (char*)buf)))
	{
		uint8_t spec_idx = ATOI(param, 10);
		if(spec_idx >= CAN_SPEC_MAX) 
			spec_idx = CAN_SPEC_A;

		p_dev_config->can_config.spec = spec_idx;
		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "fid", (char*)buf)))
	{
		if(p_dev_config->can_config.spec == CAN_SPEC_A)
		{
			if(strlen(param) > 4)
				p_dev_config->can_config.filt_id = 0;
			else
				p_dev_config->can_config.filt_id = string_to_hex32(param);
		}
		else
		{
			p_dev_config->can_config.filt_id = string_to_hex32(param);
		}

		ret = 1;
	}
	if((param = get_http_param_value((char *)uri, "fmask", (char*)buf)))
	{
		if(p_dev_config->can_config.spec == CAN_SPEC_A)
		{
			if(strlen(param) > 4)
				p_dev_config->can_config.filt_mask = 0;
			else
				p_dev_config->can_config.filt_mask = string_to_hex32(param);
		}
		else
		{
			p_dev_config->can_config.filt_mask = string_to_hex32(param);
		}

		ret = 1;
	}

	if(ret == 1) {
    	save_DevConfig_to_storage();
	}

	return ret;
}
