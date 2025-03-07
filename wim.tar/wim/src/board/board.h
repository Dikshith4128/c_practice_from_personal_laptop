#ifndef BOARD_H__
#define BOARD_H__

#include <stdint.h>

/* BOARD EEPROM */
#define BOARD_EEPROM_FILE		"/sys/bus/i2c/drivers/at24/3-0050/eeprom"
#define BOARD_EEPROM_FILE_NAME  "eeprom"
typedef struct __attribute__((packed)) board_eeprom_info_s {
	uint32_t ser_no;
	uint32_t ver_no;
	uint32_t cust_id;
	uint32_t hw_fe;
	uint32_t hw_fe_pop;
	uint32_t hw_fe_ena;
	uint32_t rfu0;
	uint32_t rfu1;
	uint32_t rfu2;
	uint32_t rfu3;
	uint32_t rfu4;
	uint32_t rfu5;
	uint32_t rfu6;
	uint32_t rfu7;
	uint32_t rfu8;
	uint32_t crc;
}board_eeprom_info_t;
extern volatile board_eeprom_info_t board_eeprom_info;

int board_eeprom_init(void);

#endif /* BOARD_H__ */
