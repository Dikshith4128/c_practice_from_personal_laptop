#define _GNU_SOURCE
#include <config.h>
#include <cfg.h>
#include <stdio.h>

/* Arete Linux SDK */
#include <alsdk.h>
#include <alsdk_crc.h>

#include <pthread.h>
#include <sched.h>
#include <sys/epoll.h>

#include <unistd.h>

/* WIM Application */
#include "main.h"
#include "project.h"
#include "app_debug.h"
#include "board.h"
#include "device.h"
#include "license.h"
#include <alsdk_md5.h>

/* lisencing  */
void alsdk_print_bytes(void* p, ssize_t length)
{
	ssize_t i;
	uint8_t* pp = (uint8_t*)p;

	for(i = 0; i < length; i++) {
		if(i && !(i % 16)) {
			printf("\n");
		}
		printf("%02X ", pp[i]);
	}
	printf("\n");
}
void alsdk_print_hash(char* p)
{
	int i;

	for(i = 0; i < 16; i++) {
		printf("%02x", p[i]);
	}
	printf("\n");
}
license_device_info_t license_device_info;
static license_t	  license_gen;
int license_get_lisence_info(void)
{
	char lic_seed[65] = "arete";
	char lic_string[1024];
    alsdk_md5_context ctx;

	license_device_info.board_ser_no = board_eeprom_info.ser_no;

	if(device_get_serial_no(license_device_info.module_ser_no)) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "get serial fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s\n", license_device_info.module_ser_no);
	}
	if(device_get_eth0_mac(license_device_info.module_eth0_mac)) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "get mac fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s\n", license_device_info.module_eth0_mac);
	}

	snprintf(lic_string, sizeof(lic_string), "%u%s%s%s", license_device_info.board_ser_no, license_device_info.module_ser_no, license_device_info.module_eth0_mac, lic_seed);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "lic_string:%s\n", lic_string);
	//printf("lic_string:%s\n", lic_string);

    alsdk_md5_init(&ctx);
    alsdk_md5_update(&ctx, (uint8_t*) lic_string, strlen(lic_string));
    alsdk_md5_finalize(&ctx);
#if  0
	alsdk_print_hash((char*)ctx.digest);
#endif

    memcpy(license_gen.digest, ctx.digest, 16);
#if 0
	alsdk_print_hash((char*)ctx.digest);
	alsdk_print_hash((char*)license_gen.digest);
#endif
	license_gen.crc = alsdk_crc32((uint8_t*)&license_gen, sizeof(license_t) - sizeof(license_gen.crc), 0xFFFFFFFFL);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "license_gen.crc         : %X\n", license_gen.crc);
	//printf("license_gen.crc         : %X\n", license_gen.crc);

	return 0;
}


volatile license_t lic_license;
int license_print(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "---------------------------------\n");
	//APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "license->digest      : %s\n", license->digest);
	//alsdk_print_hash((char*)license->digest);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "licence->crc         : %u\n", license->crc);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "---------------------------------\n");

	return 0;
}
int license_verify(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	uint32_t crc;

	crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	if(crc != license->crc)
		return -1;

	return 0;
}
int license_validate(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	if(memcmp(license_gen.digest, license->digest, 16)){
		return -1;
	}

	return 0;
}
int license_generate(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	memset(license, 0, sizeof(license_t));

	strncpy((char*)&license->digest, LICENSE_DIGEST, sizeof(license->digest));

	license->crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	return 0;
}
int license_update(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	license->crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	return 0;
}
int license_check_exists(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;
	/* Check file existence. */
	rval = access(LICENSE_FILE, F_OK);
	if(rval == 0) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s exists\n", LICENSE_FILE);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s does not exist\n", LICENSE_FILE_NAME);
		}else if(errno == EACCES) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is not accessible\n", LICENSE_FILE_NAME);
		}
		return -1;
     }

	return 0;
}
int license_check_r(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;

	/* Check read write access. */
	rval = access(LICENSE_FILE, R_OK);
	if(rval == 0) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is r\n", LICENSE_FILE_NAME);
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is not r (access denied)\n", LICENSE_FILE_NAME);
	}

	return 0;
}
int license_create(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;

	fp = fopen(LICENSE_FILE, "wrb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error creating file %s\n", LICENSE_FILE_NAME);
		return -1;
	}

	fclose(fp);

	return 0;
}
int license_open(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	return 0;
}
int license_write(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	if(license == NULL)
		return -1;

	fp = fopen(LICENSE_FILE, "wb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error opening file %s\n", CFG_MACHINE_FILE_NAME);
		return -1;
     }

	ret = fwrite(license, sizeof(license_t), 1, fp);
	if(ret != 1) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error writing file %s : %zu\n", CFG_MACHINE_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}
int license_read(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	fp = fopen(LICENSE_FILE, "rb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error opening file %s\n", CFG_MACHINE_FILE_NAME);
		return -1;
	}

	memset(license, 0, sizeof(license_t));

	ret = fread(license, sizeof(license_t), 1, fp);
	if(ret != 1) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error reading file %s : %zu\n", CFG_MACHINE_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}
int license_init(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(license_get_lisence_info()) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "license_get_lisence_info Fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "license_get_lisence_info Succ\n");
	}
	/* Get Local License */
	if(license_check_exists()) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "No License File Found\n");
#if 0
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "Exists Genetating\n");
		license_write(&license_gen);
		return  0;
#endif
		return -2;
	}else {
		if(license_read(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License Read Failed\n");
#if 0
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "Read Genetating\n");
		license_write(&license_gen);
		return  0;
#endif
			return -3;
		}
		if(license_verify(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License CRC verify Failed\n");
			return  -4;
		}
		license_print(&lic_license);
		if(license_validate(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License Validation Failed\n");
			return  -5;
		}
	}
	return 0;
}
