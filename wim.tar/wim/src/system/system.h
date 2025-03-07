#ifndef SYSTEM_H__
#define SYSTEM_H__

#include <stdint.h>

#define WIM_VERSION "1.4.0"

typedef union system_err_u {
	uint16_t bytes;
	struct {
		uint8_t bit0 : 1;
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
}system_err_t;

extern volatile system_err_t sys_err;

int system_hello(void);

int system_gettime(time_t* utc_sec);
int system_settime(time_t utc_sec);


int system_do_chown(const char *file_path, const char *user_name, const char *group_name);
float system_get_1_min_cpu_load_avg(void);
int system_rt_reboot(uint8_t reboot);
int system_rt_restart(uint8_t restart);

#endif /* SYSTEM_H__ */
