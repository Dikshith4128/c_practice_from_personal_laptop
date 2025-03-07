#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>
#include <errno.h>

/* Arete Linux SDK */
#include <alsdk.h>

/* WIM Application */
#include "main.h"
#include "device.h"
#include "watchdog.h"
#include "app_debug.h"
#include "dev_cfg.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

static int api_watchdog_fd = -1;

int device_watchdog_open(const char * watchdog_device)
{
    int ret = -1;
	int timeout = 0;
    if (api_watchdog_fd >= 0) {
        fprintf(stderr, "Watchdog already opened\n");
        return ret;
    }
    api_watchdog_fd = open(watchdog_device, O_RDWR);
    if (api_watchdog_fd < 0) {
        fprintf(stderr, "Could not open %s: %s\n", watchdog_device, strerror(errno));
        return api_watchdog_fd;
    }

	/* WDT0 is counting now,check the default timeout value */
	ret = ioctl(api_watchdog_fd, WDIOC_GETTIMEOUT, &timeout);
	if(ret) {
		fprintf(stderr, "Get watchdog timeout value failed!\n");
		return -1;
	}
	//fprintf(stdout, "Watchdog timeout value: %d\n", timeout);

	/* set new timeout value 8s */
	/* Note the value should be within [1, 8, 32, 128] */
	timeout = 8;
	ret = ioctl(api_watchdog_fd, WDIOC_SETTIMEOUT, &timeout);
	if(ret) {
		fprintf(stderr, "Set watchdog timeout value failed!\n");
		return -1;
	}
	//fprintf(stdout, "New watchdog timeout value: %d\n", timeout);
    return api_watchdog_fd;
}
static int count = 100;

int device_watchdog_hwfeed(void)
{
    int ret = -1;
    if (api_watchdog_fd < 0){
        fprintf(stderr, "Watchdog must be opened first!\n");
        return ret;
    }

	//if(count) {
	ret = ioctl(api_watchdog_fd, WDIOC_KEEPALIVE, NULL);
	if (ret < 0){
		fprintf(stderr, "Could not pat watchdog: %s\n", strerror(errno));
	}
	////count--;
	//printf("pet %d\n", count);
	//}else {
		//printf("pet stop\n", count);
	//}
    return ret;
}

int device_watchdog_init(void)
{
    //printf("Open WatchDog\n");
    int ret = 0;
    ret = device_watchdog_open("/dev/watchdog");
    if(ret < 0){
        fprintf(stderr, "Could not init watchdog: %s\n", strerror(errno));
        return ret;
	}
    ret = device_watchdog_hwfeed();
    return ret;
}
