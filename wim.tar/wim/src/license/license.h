#ifndef LICENSE_H__
#define LICENSE_H__

#include <stdint.h>
#include "project.h"
/* lisence file */
#define LICENSE_FILE		"/home/arete/license/license.lic"
#define LICENSE_FILE_NAME	"license.lic"

typedef struct license_device_info_s {
	uint32_t board_ser_no;
	uint8_t module_ser_no[9]; /* serial no */
	uint8_t module_eth0_mac[18];  /* mac address */
}license_device_info_t;

extern license_device_info_t license_device_info;

/* machine cfg */
typedef struct __attribute__((__packed__)) license_s {
	uint8_t		digest[16];
	uint32_t	crc;
}license_t;
#define LICENSE_DIGEST "1234567890123456"

int lisence_init(void);

#endif /* LICENSE_H__ */
