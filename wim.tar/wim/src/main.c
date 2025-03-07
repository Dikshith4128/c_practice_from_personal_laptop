#define _GNU_SOURCE
#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
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
#include "weigh.h"
#include "guio_char_lcd.h"

#include <zmq.h>
#include <czmq.h>

sem_t exit_sema;

void sigint(int signo)
{
	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	if(signo == SIGINT) {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_STATE, "Ctrl+C detected !!! \n");
		printf("Ctrl+C detected !!! \n");
		sem_post(&exit_sema);
	}
}


#define CHECK_CONTINUE_ERR 10
int check_continue(void)
{
	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "shlvl   : %d\n", alsdk_env_shlvl_get());
	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "ssh     : %d\n", alsdk_env_ssh_login());
	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "console : %d\n", alsdk_env_console_login());

	if((alsdk_env_shlvl_get() == 1) && (!alsdk_env_console_login()))
		return 0;

	return -1;
}


int main (void)
{
	alsdk_pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);


    /* cpu set 0 */
    cpu_set_t cpuset_0;
    CPU_ZERO(&cpuset_0);
    CPU_SET(0, &cpuset_0);
    pthread_t current_thread = pthread_self();
    alsdk_pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset_0);
	setpriority(PRIO_PROCESS, 0, -12);//0

	app_debug_sync_init();

	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(check_continue()) {
		exit(-CHECK_CONTINUE_ERR);
	}

#if 0
	if(1) {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "project info init Failed!\n");
		/* init glcd */
		guio_char_lcd_t guio_char_lcd;
#define LCD_DEV "/dev/lcd"
		guio_char_lcd_open_raw(&guio_char_lcd, LCD_DEV);
        guio_char_lcd_reset(&guio_char_lcd);

		guio_char_lcd_line0_send_raw(&guio_char_lcd, "11111111111111111111", 20);
		guio_char_lcd_line1_send_raw(&guio_char_lcd, "22222222222222222222", 20);
		guio_char_lcd_line2_send_raw(&guio_char_lcd, "33333333333333333333", 20);
		guio_char_lcd_line3_send_raw(&guio_char_lcd, "44444444444444444444", 20);

		sleep(4);


		guio_char_lcd_line0_send_raw(&guio_char_lcd, "88888888888888888888", 20);
        guio_char_lcd_line1_send_raw(&guio_char_lcd, "88888888888888888888", 20);
        guio_char_lcd_line2_send_raw(&guio_char_lcd, "88888888888888888888", 20);
        guio_char_lcd_line3_send_raw(&guio_char_lcd, "88888888888888888888", 20);


        guio_char_lcd_close_raw(&guio_char_lcd);

		exit(-EXIT_FAILURE);
	}
#endif

#if 1

	/* Read EEPROM */
	if(board_eeprom_init()) {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "BEEPROM Failed!\n");
/* init glcd */
		guio_char_lcd_t guio_char_lcd;
#define LCD_DEV "/dev/lcd"
		guio_char_lcd_open_raw(&guio_char_lcd, LCD_DEV);
		guio_char_lcd_reset(&guio_char_lcd);
		guio_char_lcd_line0_send_raw(&guio_char_lcd, "ERROR : BOARD", 13);
		guio_char_lcd_close_raw(&guio_char_lcd);

		exit(-EXIT_FAILURE);
	}else {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "BEEPROM check Success!\n");
		if(project_info_init()){
			APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "project info init Failed!\n");
/* init glcd */
			guio_char_lcd_t guio_char_lcd;
#define LCD_DEV "/dev/lcd"
			guio_char_lcd_open_raw(&guio_char_lcd, LCD_DEV);
			guio_char_lcd_reset(&guio_char_lcd);
			guio_char_lcd_line0_send_raw(&guio_char_lcd, "ERROR : EEPROM", 14);
			guio_char_lcd_close_raw(&guio_char_lcd);

		exit(-EXIT_FAILURE);
		}else {
			APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "project info init Success!\n");
		}
	}

    /* Lisencing */
	if(license_init()) {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "lisence check failed!\n");
/* init glcd */
		guio_char_lcd_t guio_char_lcd;
#define LCD_DEV "/dev/lcd"
		guio_char_lcd_open_raw(&guio_char_lcd, LCD_DEV);
		guio_char_lcd_reset(&guio_char_lcd);
		guio_char_lcd_line0_send_raw(&guio_char_lcd, "ERROR : LICENCE", 15);
		guio_char_lcd_close_raw(&guio_char_lcd);

		exit(-EXIT_FAILURE);
	}else {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "lisence check Success!\n");
	APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "lisence check success\n");
	}
#endif

	/* Device */
	if(device_init()) {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "Device Init Failed\n");
	}else {
		APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "Device Init Success!\n");
	}

	/* initialize exit sema*/
    alsdk_sem_init(&exit_sema, 0, 0);

    // Block the SIGINT signal. The threads will inherit the signal mask.
    // This will avoid them catching SIGINT instead of this thread.
    sigset_t sigset, oldset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
	//signal(SIGPIPE, SIG_IGN);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);


    // Install the signal handler for SIGINT.
    struct sigaction s;
    s.sa_handler = sigint;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    sigaction(SIGINT, &s, NULL);



	/* Application Start */
	weigh_start();

	// Restore the old signal mask only for this thread.
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);

	alsdk_pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    // Wait for SIGINT to arrive.
    pause();
	while(sem_wait(&exit_sema)!=0) {}

    /* Application End */
	weigh_end();
	sync();

    // Done.
    APP_DEBUGF(MAIN_DEBUG|APP_DBG_TRACE, "Terminated.");
//	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}
