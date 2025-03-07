#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>
#include <errno.h>

/* Arete Linux SDK */
#include <alsdk.h>
#include <alsdk_crc.h>

/* WIM Application */
#include "main.h"
#include "board.h"
#include "app_debug.h"

#include <unistd.h>

static uint32_t crc32(const char *s,size_t n)
{
	uint32_t crc=0xFFFFFFFF;

	for(size_t i=0;i<n;i++) {
		char ch=s[i];
		for(size_t j=0;j<8;j++) {
			uint32_t b=(ch^crc)&1;
			crc>>=1;
			if(b) crc=crc^0xEDB88320;
			ch>>=1;
		}
	}

	return ~crc;
}


volatile board_eeprom_info_t board_eeprom_info;
static void board_print_eeprom_buf(const unsigned char *buf, size_t buf_len)
{
    size_t i = 0;
    for(i = 0; i < buf_len; ++i) {
		APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "%02X%s", buf[i],  (i + 1) % 16 == 0 ? "\r\n" : " " );
	}
}
int board_eeprom_print(board_eeprom_info_t* board_eeprom_info)
{
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	if(board_eeprom_info == NULL)
         return -1;

     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "---------------------------------\n");
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "ser_no         : %u\n", board_eeprom_info->ser_no);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "ver_no         : %u\n", board_eeprom_info->ver_no);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "cust_id        : %u\n", board_eeprom_info->cust_id);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "hw_fe	       : %u\n", board_eeprom_info->hw_fe);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "hw_fe_pop      : %u\n", board_eeprom_info->hw_fe_pop);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "hw_fe_ena      : %u\n", board_eeprom_info->hw_fe_ena);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu0           : %u\n", board_eeprom_info->rfu0);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu1           : %u\n", board_eeprom_info->rfu1);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu2           : %u\n", board_eeprom_info->rfu2);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu3           : %u\n", board_eeprom_info->rfu3);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu4           : %u\n", board_eeprom_info->rfu4);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu5           : %u\n", board_eeprom_info->rfu5);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu6           : %u\n", board_eeprom_info->rfu6);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu7           : %u\n", board_eeprom_info->rfu7);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "rfu8           : %u\n", board_eeprom_info->rfu8);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "crc            : %#010x\n", board_eeprom_info->crc);
     APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "---------------------------------\n");

	 return 0;
}
int board_eeprom_check_exists(void)
{
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;
	/* Check file existence. */
	rval = access(BOARD_EEPROM_FILE, F_OK);
	if(rval == 0) {
		APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "%s exists\n", BOARD_EEPROM_FILE_NAME);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "%s does not exist\n", BOARD_EEPROM_FILE_NAME);
		}else if(errno == EACCES) {
			APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "%s is not accessible\n", BOARD_EEPROM_FILE_NAME);
		}
		return -1;
	}

	return 0;
}
int board_eeprom_read(board_eeprom_info_t* board_eeprom_info)
{
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	FILE *fp;
	size_t ret;

	fp = fopen(BOARD_EEPROM_FILE, "rb");
	if(fp == NULL) {
		APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "Error opening file %s\n", BOARD_EEPROM_FILE_NAME);
		return -1;
	}

	memset(board_eeprom_info, 0, sizeof(board_eeprom_info_t));
	ret = fread(board_eeprom_info, sizeof(board_eeprom_info_t), 1, fp);
	if(ret != 1) {
		APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "Error reading file %s : %zu\n", BOARD_EEPROM_FILE_NAME, ret);
		return -1;
	}
	fclose(fp);

	return 0;
}
int board_eeprom_verify(board_eeprom_info_t* board_eeprom_info)
{
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(board_eeprom_info == NULL)
		return -1;

	uint32_t crc = crc32((uint8_t*)board_eeprom_info, sizeof(board_eeprom_info_t) - sizeof(board_eeprom_info->crc));
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_STATE, "crc            : %#010x\n", crc);


	if(crc != board_eeprom_info->crc)
		return -1;

	return 0;
}

int board_eeprom_init(void)
{
	APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(board_eeprom_check_exists()) {
		APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "No BEEPROM File Found\n");
        return -2;
     }else {
         if(board_eeprom_read(&board_eeprom_info)) {
             APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "BEEPROM Read Failed\n");
             return -3;
         }
		 board_print_eeprom_buf(&board_eeprom_info, sizeof(board_eeprom_info_t));
         board_eeprom_print(&board_eeprom_info);
         if(board_eeprom_verify(&board_eeprom_info)) {
             APP_DEBUGF(BOARD_DEBUG|APP_DBG_TRACE, "BEEPROM CRC verify Failed\n");
             return  -4;
         }
     }
     return 0;
 }

