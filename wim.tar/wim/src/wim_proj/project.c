#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

#include <errno.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/resource.h>


#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sched.h>

#include <alsdk.h>
#include <alsdk_environ.h>
#include <alsdk_md5.h>

#include "app_debug.h"
#include "main.h"
#include "board.h"
#include "license.h"
#include "device.h"
#include "project.h"
#include "elf.h"

volatile project_image_info_t project_image_info;
volatile project_m4_info_t    project_m4_info;

volatile project_info_t project_info;


int project_info_image_info_print(project_image_info_t *image_info)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "IMG_VER = %s\n", image_info->version);
	return 0;
}
int project_info_image_info_read(project_image_info_t *image_info)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE*   stream;
	char*   lineptr = NULL;
	size_t  n = 0;
	ssize_t nread;
	char *substr; // the "result"
	int ret;

	if(image_info == NULL)
		return -1;

	stream = fopen(PROJECT_IMAGE_INFO_FILE, "r");
	if(stream != NULL) {
		while((nread = getline(&lineptr, &n, stream)) != -1) {
			if(strstr(lineptr, "BUILD_VERSION=") != NULL) {
				//fwrite(lineptr, nread, 1, stdout);
				lineptr[nread-1] = '\0';
				//printf("nr:%zd:%s\n", nread, lineptr);

				substr = strtok(lineptr, "\""); // find the first double quote
				substr = strtok(NULL, "\"");   // find the second double quote
				if(!substr) {
					//printf("Not found\n");
					ret = -2;
				}else {
					//printf("the thing in between quotes is '%s'\n", substr);
					snprintf(project_image_info.version, 15, "%s", substr);
					ret = 0;
				}
				free(lineptr);
				fclose(stream);
				return  ret;
			}
		}

		free(lineptr);
		fclose(stream);
		return -3;
	}
	return -4;
}

int project_info_image_info_check_exists(void)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;
	/* Check file existence. */
	rval = access(PROJECT_IMAGE_INFO_FILE, F_OK);
	if(rval == 0) {
		APP_DEBUGF(PROJECT_DEBUG|APP_DBG_STATE, "%s exists\n", PROJECT_IMAGE_INFO_FILE);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(PROJECT_DEBUG|APP_DBG_STATE, "%s does not exist\n", PROJECT_IMAGE_INFO_FILE_NAME);
		}else if(errno == EACCES) {
			APP_DEBUGF(PROJECT_DEBUG|APP_DBG_STATE, "%s is not accessible\n", PROJECT_IMAGE_INFO_FILE_NAME);
		}
		return -1;
     }

	return 0;
}
int project_info_image_info_init(void)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	/* Get image info from file */
	if(project_info_image_info_check_exists()) {
		APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "No Image Info File Found\n");
		snprintf(project_image_info.version, 15, "%s", "1.0.0");
		return -1;
	}else {
		if(project_info_image_info_read(&project_image_info)) {
			APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "Image Info File Read Failed\n");
			snprintf(project_image_info.version, 15, "%s", "1.0.0");
			return -1;
		}
		project_info_image_info_print(&project_image_info);
	}
	return 0;
}

int project_print_m4_elf_version_item(FILE* fd, Elf32_Ehdr eh, Elf32_Shdr sh_table[], project_m4_info_t *project_m4_info)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	int i,ret;
	char* sh_str;
	char* buff;

    buff = malloc(sh_table[eh.e_shstrndx].sh_size);
	if(buff != NULL) {
		fseek(fd, sh_table[eh.e_shstrndx].sh_offset, SEEK_SET);
		fread(buff, 1, sh_table[eh.e_shstrndx].sh_size, fd);
	}

	sh_str = buff;
	for(i=0; i<eh.e_shnum; i++) {
		if(!strcmp(".wim_bm_ver", (sh_str + sh_table[i].sh_name))) {
			//printf("Found section\t\".wim_bm_ver\"\n");
			//printf("at offset\t0x%08x\n", (unsigned int)sh_table[i].sh_offset);
			//printf("of size\t\t0x%08x\n", (unsigned int)sh_table[i].sh_size);
			break;
		}
	}

	/*Code to print or store string data*/
	if (i < eh.e_shnum) {
		char *mydata = malloc(sh_table[i].sh_size);
		fseek(fd, sh_table[i].sh_offset, SEEK_SET);
		fread(mydata, 1, sh_table[i].sh_size, fd);
	    strncpy(project_m4_info->version, mydata, 15);
		//puts(mydata);
		//puts(project_m4_info->version);
		ret = 0;
	} else {
		//printf("section not found\n");
	    strncpy(project_m4_info->version, "1.7.2", 15);
		ret = 1;
	}
	free(buff);
	return ret;
}

int project_info_m4_info_get(project_m4_info_t *project_m4_info)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
    FILE* fp = NULL; //Pointer used to access current file
    char* program_name;
    Elf32_Ehdr elf_header; //Elf header
    Elf32_Shdr* sh_table; //Elf symbol table

    program_name = "/home/arete/m4_fw/wim_bm_7190.elf";
    fp = fopen(program_name, "r");

    fseek(fp, 0, SEEK_SET);
    fread(&elf_header, 1, sizeof(Elf32_Ehdr), fp);
    sh_table = malloc(elf_header.e_shentsize*elf_header.e_shnum);

    fseek(fp, elf_header.e_shoff, SEEK_SET);
    fread(sh_table, 1, elf_header.e_shentsize*elf_header.e_shnum, fp);

    project_print_m4_elf_version_item(fp, elf_header, sh_table, project_m4_info);
	free(sh_table);
    fclose(fp);

    return 0;
}
int project_info_m4_info_init()
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	project_info_m4_info_get(&project_m4_info);
    return 0;
}

int project_info_init(void)
{
	APP_DEBUGF(PROJECT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	/*Customer_Info*/
	if(board_eeprom_info.cust_id >= WIM_CUST_ARETE && board_eeprom_info.cust_id < WIM_CUST_MAX) {
		project_info.cust.cust_id = board_eeprom_info.cust_id;
	}else {
		project_info.cust.cust_id = WIM_CUST_ARETE;
	}

	switch(board_eeprom_info.cust_id) {
		case WIM_CUST_ARETE:
			project_info.cust.ui_port	  = 55566;
			project_info.cust.net_io_port = 4766;
			/* TEMP */
			//project_info.cust.ui_port = 55577;
			//project_info.cust.net_io_port = 4765;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			//project_info.wim.arb  = WIM_ARB_LANDT;
			break;
		case WIM_CUST_ASPEKT:
			project_info.cust.ui_port = 55599;
			project_info.cust.net_io_port = 4799;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_ESSAE:
			project_info.cust.ui_port = 5555;
			project_info.cust.net_io_port = 4755;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_LANDT:
			project_info.cust.ui_port = 55411;
			project_info.cust.net_io_port = 4611;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_LANDT;
			break;
		case WIM_CUST_MERIT:
			project_info.cust.ui_port = 55544;
			project_info.cust.net_io_port = 4744;
			project_info.wim.axle = WIM_AXLE_MERIT;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_RAJDEEP:
			project_info.cust.ui_port = 55588;
			project_info.cust.net_io_port = 4788;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_VAAAN:
			project_info.cust.ui_port = 55577;
			project_info.cust.net_io_port = 4765;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_TULAMAN:
			project_info.cust.ui_port = 55533;
			project_info.cust.net_io_port = 4733;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_NAGARJUN:
			project_info.cust.ui_port = 55511;
			project_info.cust.net_io_port = 4711;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_FCS:
			project_info.cust.ui_port = 55522;
			project_info.cust.net_io_port = 4722;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_JWS:
			project_info.cust.ui_port = 55500;
			project_info.cust.net_io_port = 4700;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_EUROTECH:
			project_info.cust.ui_port = 55400;
			project_info.cust.net_io_port = 4600;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		case WIM_CUST_MCUBE:
			project_info.cust.ui_port = 55422;
			project_info.cust.net_io_port = 4622;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
		default:
			project_info.cust.ui_port = 55566;
			project_info.cust.net_io_port = 4766;
			project_info.wim.axle = WIM_AXLE_ARETE;
			project_info.wim.sep  = WIM_SEP_ARETE;
			project_info.wim.arb  = WIM_ARB_ARETE;
			break;
	}

	//printf("CUST_ID: %u\n", board_eeprom_info.cust_id);
	//printf("ui_port: %u\n", project_info.cust.ui_port);
	//printf("net_io_port: %u\n", project_info.cust.net_io_port);


	/*HW_Info*/
	project_info.hw.ser_no = board_eeprom_info.ser_no;
	project_info.hw.ver_no = board_eeprom_info.ver_no;

     /*ADC*/
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_1255_MASK) {
		project_info.hw.adc.type = WIM_ADC_ADS1255;
		project_info.hw.adc.sample_rate = 7500;
		project_info.hw.adc.pga = 64.0f;
		project_info.hw.adc.f1_size = 16;
		project_info.hw.adc.f2_size = 4;
		project_info.hw.adc.npkts_phs = 30;
		project_info.hw.adc.axle_ndata = (5*7500);
	}else if(board_eeprom_info.hw_fe_ena & P_HW_INFO_7190_MASK) {
		project_info.hw.adc.type = WIM_ADC_AD7190;
		project_info.hw.adc.sample_rate = 4800;
		project_info.hw.adc.pga = 128.0f;
		project_info.hw.adc.f1_size = 24;
		project_info.hw.adc.f2_size = 4;
		project_info.hw.adc.npkts_phs = 20;
		project_info.hw.adc.axle_ndata = (5*4800);
	}else {
		project_info.hw.adc.type = WIM_ADC_AD7190;
		project_info.hw.adc.sample_rate = 4800;
		project_info.hw.adc.pga = 128.0f;
		project_info.hw.adc.f1_size = 24;
		project_info.hw.adc.f2_size = 4;
		project_info.hw.adc.npkts_phs = 20;
		project_info.hw.adc.axle_ndata = (5*4800);
	}

	/*IO*/
	project_info.hw.io.rs232 = 1;
	project_info.hw.io.eth = 1;
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_RS232_MASK) {
		project_info.hw.io.rs232 = 1;
	}
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_ETH_MASK) {
		project_info.hw.io.eth = 1;
	}
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_USB_MASK) {
		project_info.hw.io.usb = 1;
	}
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_RS485_MASK) {
		project_info.hw.io.rs485 = 1;
	}


	/*LCD*/
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_LCD_MASK) {
		/* 4*20 LCD */
		project_info.hw.lcd.type = WIM_CLCD_HD44780;
		project_info.hw.lcd.res = WIM_CLCD_RES_4_20;
		project_info.hw.lcd.nrows = 4;
		project_info.hw.lcd.ncols = 20;
	}else {
		/* 2*16 LCD */
		project_info.hw.lcd.type = WIM_CLCD_HD44780;
		project_info.hw.lcd.res = WIM_CLCD_RES_2_16;
		project_info.hw.lcd.nrows = 2;
		project_info.hw.lcd.ncols = 16;
	}

	/*KEYPAD*/
	if(board_eeprom_info.hw_fe_ena & P_HW_INFO_MKEY_BIT) {
		project_info.hw.mkeypad.type = 0;
		project_info.hw.mkeypad.nrows = 4;
		project_info.hw.mkeypad.ncols = 4;
	}else {
		project_info.hw.mkeypad.type = 0;
		project_info.hw.mkeypad.nrows = 4;
		project_info.hw.mkeypad.ncols = 4;
	}

	project_info.hw.sdcard.use = 1;


	project_info_image_info_init();
    project_info_m4_info_init();

	return 0 ;

}
