#ifndef DEV_CFG_H__
#define DEV_CFG_H__

#include <stdint.h>

#define DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE			"/etc/systemd/network/10-eth0-dhcp.network"
#define DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE_NAME		"10-eth0-dhcp.network"

#define DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE				"/etc/systemd/network/10-eth0-static.network"
#define DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME		"10-eth0-static.network"

#define DEV_CFG_NET_CFG_FILE_STATIC     0
#define DEV_CFG_NET_CFG_FILE_DYNAMIC    1

typedef struct __attribute__((packed)) dev_cfg_net_cfg_info_s {
	uint8_t  ip_eth0_mac[18];  /* mac address */
	uint8_t  ip_addr_mode;
	uint8_t  ip_addr[16];
	uint8_t  ip_addr_subnet_mask[16];
	uint8_t  ip_addr_gateway[16];
	uint8_t  ip_addr_dns_server[16];
}dev_cfg_net_cfg_info_t;

typedef struct __attribute__((packed)) dev_cfg_net_cfg_s {
	uint8_t  ip_addr_mode;
	uint8_t  ip_addr[16];
	uint8_t  ip_addr_subnet_mask[16];
	uint8_t  ip_addr_gateway[16];
	uint8_t  ip_addr_dns_server[16];
}dev_cfg_net_cfg_t;
extern volatile dev_cfg_net_cfg_info_t  dev_cfg_net_cfg_info;

#endif /* DEV_CFG_H__ */
