#ifndef LOG_H__
#define LOG_H__

#include <pthread.h>
#include <semaphore.h>   /* Posix semaphores */
#include <mqueue.h>
#include <alsdk_pthread.h>

#include "log_sd.h"
/* weigh barriers */
#define LOG_INIT_BARRIER_COUNT  8

#define LOG_TD_QUEUE_NAME  "/log-td-queue" /* Queue name. */
#define LOG_TD_QUEUE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
									/* default permissions for new files */
#define LOG_TD_QUEUE_MAXMSG  10 /* Maximum number of messages. */
#define LOG_TD_QUEUE_MSGSIZE 16 /* Length of message. */

#define LOG_TD_QUEUE_EVENT_PRIO ((int)(3))
#define LOG_TD_QUEUE_CMD_PRIO ((int)(0))

#define LOG_TD_EPOLL_MAX_EVENTS 16



/* SD card Settings */

#define LOG_SD_DEV "/dev/mmcblk1p1"
#define LOG_SD_ROOT "/home/sdcard/sd_card"
//#define LOG_SD_USER_NAME "sdcard"
#define LOG_SD_USER_NAME "arete"
#define LOG_SD_RB_WBUFF_SIZE 2048 /* write buffer size */

extern log_sd_t log_sd;

/* Serial Port settings */
#define LOG_SERIAL_PORT "/dev/ttymxc2"
#define LOG_SERIAL_PORT_BAUD 115200
#define LOG_SERIAL_PORT_MODE "cannonical"

/* net settings */
/* lcd settings */



extern alsdk_thread_info_t tif_log_td;
extern mqd_t mqd_log_td;

int log_init(void);
int log_cancel(void);
int log_join(void);
int log_exit(void);

typedef enum log_cmd_e {
	LOG_CMD_START = 0,
	LOG_CMD_STOP,
	LOG_CMD_RESTART,
	LOG_CMD_MAX
}log_cmd_t;

typedef enum log_event_e {
	LOG_EVENT_INIT = 0,
	LOG_EVENT_LOG_ENABLE,
	LOG_EVENT_LOG_DISABLE,
	LOG_EVENT_SD_UP,
	LOG_EVENT_SD_DOWN,
	LOG_EVENT_EOD,
	LOG_EVENT_SD,
	LOG_EVENT_DUMMY,
	LOG_EVENT_MAX
}log_event_t;

typedef enum log_msg_type_e {
    LOG_MSG_TYPE_EVENT = 0,
    LOG_MSG_TYPE_CMD ,
    LOG_MSG_TYPE_MAX
}log_msg_type_t;

typedef struct __attribute__((packed)) log_cmd_dummy_params_s {
     int32_t data1;
     int32_t data2;
     int32_t data3;
}log_cmd_dummy_params_t;

typedef struct __attribute__((packed)) log_cmd_start_params_s {
	int32_t info;
}log_cmd_start_params_t;
typedef struct __attribute__((packed)) log_cmd_stop_params_s {
	int32_t info;
}log_cmd_stop_params_t;
typedef struct __attribute__((packed)) log_cmd_restart_params_s {
	int32_t info;
}log_cmd_restart_params_t;

typedef struct __attribute__((packed)) log_cmd_server_s {
    void* ptr;
}log_cmd_server_t;

typedef union __attribute__((packed)) log_msg_payload_u {
    log_cmd_dummy_params_t dummy;
    log_cmd_server_t cmd_server;
	log_cmd_start_params_t* start_params;
	log_cmd_stop_params_t* stop_params;
	log_cmd_restart_params_t* restart_params;
}log_msg_payload_t;

typedef struct __attribute__((packed)) log_msg_header_s {
     int16_t type;
     int16_t id;
}log_msg_header_t;

typedef struct __attribute__((packed)) log_msg_s {
     log_msg_header_t header;
     log_msg_payload_t payload;
}log_msg_t;

int log_raise_init_event(void);
int log_raise_dummy_event(void);
int log_raise_sd_new_file_event(void);
int log_raise_sd_event(void);


#endif /* LOG_H__ */
