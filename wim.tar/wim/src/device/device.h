#ifndef DEVICE_H__
#define DEVICE_H__

#include <stdint.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define DEV_NETWORK_DEFAULT_CFG_FILE  "/etc/systemd/network/10-eth0-def.network"
#define DEV_NETWORK_STATIC_CFG_FILE  "/etc/systemd/network/10-eth0-sta.network"
#define DEV_NETWORK_DYNAMIC_CFG_FILE  "/etc/systemd/network/10-eth0-dyn.network"

typedef union device_err_u {
	uint16_t bytes;
	struct {
		uint8_t sd_p : 1;
		uint8_t bit1 : 1;
		uint8_t bit2 : 1;
		uint8_t bit3 : 1;
		uint8_t bit4 : 1;
		uint8_t bit5 : 1;
		uint8_t bit6 : 1;
		uint8_t bit7 : 1;
		uint8_t bit8 : 1;
		uint8_t bit9 : 1;
		uint8_t bit10 : 1;
		uint8_t bit11 : 1;
		uint8_t bit12 : 1;
		uint8_t bit13 : 1;
		uint8_t bit14 : 1;
		uint8_t bit15 : 1;
	} bits;
}device_err_t;
extern volatile device_err_t dev_err;


/* DEV_SET_NETWORK_PARAMS               8*/
/* DEV_GET_NETWORK_PARAMS               9*/
typedef enum dev_ip_addr_req_mode_e {
    DEV_IP_ADDR_REQ_MODE_STATIC = 0,
    DEV_IP_ADDR_REQ_MODE_DHCP
}dev_ip_addr_req_mode_t;

typedef struct __attribute__((packed)) dev_network_params_s {
    uint8_t  ip_addr_req_mode;
    uint8_t  ip_addr[16];
    uint8_t  ip_addr_subnet_mask[16];
    uint8_t  ip_addr_gateway[16];
    uint8_t  ip_addr_dns_server[16];
}dev_network_params_t;

typedef struct __attribute__((packed)) dev_network_params_get_s {
    uint8_t  ip_eth0_mac[18];  /* mac address */
    uint8_t  ip_addr_mode;
    uint8_t  ip_addr[16];
    uint8_t  ip_addr_subnet_mask[16];
    uint8_t  ip_addr_gateway[16];
    uint8_t  ip_addr_dns_server[16];
}dev_network_params_get_t;

typedef enum dev_ip_addr_req_status_e {
    DEV_IP_ADDR_REQ_STATUS_INVALID = 0,
    DEV_IP_ADDR_REQ_STATUS_INVALID_SUBNET = 0,
    DEV_IP_ADDR_REQ_STATUS_INVALID_DNS,
    DEV_IP_ADDR_REQ_STATUS_INVALID_GATWEAY,
    DEV_IP_ADDR_REQ_STATUS_INVALID_IP
}dev_ip_addr_req_status_t;


int device_hello(void);

int device_get_if_ipaddress(char* ifc, char* ipaddress);
void show_address_info(struct ifaddrs* ifa);
int device_print_ip(void);
int device_get_link_status(char* ifc_name);
int device_get_link_local_ip(char* ifc_name, char* ip_address);
int device_get_ip(char* ifc_name, char* ip_address);
int  device_get_eth0_mac(char* eth0_mac);
int device_get_serial_no(char* ser_no);
int device_get_serial_no(char* ser_no);


#endif /* DEVICE_H__ */
