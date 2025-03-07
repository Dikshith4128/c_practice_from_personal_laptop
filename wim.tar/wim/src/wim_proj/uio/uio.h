#ifndef UIO_H__
#define UIO_H__

#include <pthread.h>
#include <semaphore.h>   /* Posix semaphores */
#include <mqueue.h>
#include <alsdk_pthread.h>

#include "project.h"
#include "uio_serial.h"
#include "uio_net.h"

#define UIO_TD_QUEUE_NAME  "/uio-td-queue" /* Queue name. */
#define UIO_TD_QUEUE_PERMS   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
									/* default permissions for new files */
#define UIO_TD_QUEUE_MAXMSG  10 /* Maximum number of messages. */
#define UIO_TD_QUEUE_MSGSIZE 16 /* Length of message. */

#define UIO_TD_QUEUE_EVENT_PRIO ((int)(3))
#define UIO_TD_QUEUE_CMD_PRIO ((int)(0))

#define UIO_TD_EPOLL_MAX_EVENTS 16


/* Serial Port settings */

#define UIO_SERIAL_PORT1 "/dev/ttymxc2"
//#define UIO_SERIAL_PORT1 "/dev/ttyUSB1"
//#define UIO_SERIAL_PORT1 "/dev/ttyUSB0"
#define UIO_SERIAL_PORT1_BAUD 115200
#define UIO_SERIAL_PORT1_MODE "raw"

#define UIO_SERIAL_PORT1_RB_RBUFF_SIZE 2048 /* read buffer size */
#define UIO_SERIAL_PORT1_RB_WBUFF_SIZE 2048 /* write buffer size */
#define UIO_SERIAL_PORT1_PBUFF_SIZE 2048 /* parse buffer size */

#define UIO_SERIAL_READ_MAX_SIZE 2048

extern uio_serial_t uio_serial_1;
/* net settings */

extern uio_net_t uio_net_1;
#define UIO_NET1_IFC "eth0"

#if (WIM_CUST == WIM_CUST_ARETE)
#warning WIM_CUST_ARETE
#define UIO_NET1_IFC_PORT 4766
#elif (WIM_CUST == WIM_CUST_ASPEKT)
#warning WIM_CUST_ASPEKT
#define UIO_NET1_IFC_PORT 4799
#elif (WIM_CUST == WIM_CUST_ESSAE)
#warning WIM_CUST_ESSAE
#define UIO_NET1_IFC_PORT 4755
#elif (WIM_CUST == WIM_CUST_LANDT)
#warning WIM_CUST_LANDT
#define UIO_NET1_IFC_PORT 4755
#elif (WIM_CUST == WIM_CUST_MERIT)
#warning WIM_CUST_MERIT
#define UIO_NET1_IFC_PORT 4744
#elif (WIM_CUST == WIM_CUST_RAJDEEP)
#warning WIM_CUST_RAJDEEP
#define UIO_NET1_IFC_PORT 4788
#elif (WIM_CUST == WIM_CUST_VAAAN)
#warning WIM_CUST_VAAAN
#define UIO_NET1_IFC_PORT 4765
#elif (WIM_CUST == WIM_CUST_TULAMAN)
#warning WIM_CUST_TULAMAN
#define UIO_NET1_IFC_PORT 4733
#else
#error UNKNOW WIM_CUST $WIM_CUST
#endif


#define UIO_NET1_IFC_LISTENQ 4
#define UIO_NET1_IFC_RB_RBUFF_SIZE 2048 /* read buffer size */
#define UIO_NET1_IFC_RB_WBUFF_SIZE 2048 /* write buffer size */
#define UIO_NET1_IFC_PBUFF_SIZE 1024 /* parse buffer size */

#define UIO_NET_READ_MAX_SIZE 1024


extern alsdk_thread_info_t tif_uio_td;
extern mqd_t mqd_uio_td;

int uio_init(void);
int uio_cancel(void);
int uio_join(void);
int uio_exit(void);

typedef enum uio_cmd_e {
	UIO_CMD_START = 0,
	UIO_CMD_STOP,
	UIO_CMD_RESTART,
	UIO_CMD_SERIAL_BAUD,
	UIO_CMD_MAX
}uio_cmd_t;

typedef enum uio_event_e {
	UIO_EVENT_INIT = 0,
	UIO_EVENT_SERIAL,
	UIO_EVENT_NET_ACK_TIMEOUT,
	UIO_EVENT_NET,
	UIO_EVENT_NET_CONN_RESET,
	UIO_EVENT_NET_ACK_ENABLE,
	UIO_EVENT_NET_ACK_DISABLE,
	UIO_EVENT_DUMMY,
	UIO_EVENT_MAX
}uio_event_t;

typedef enum uio_msg_type_e {
    UIO_MSG_TYPE_EVENT = 0,
    UIO_MSG_TYPE_CMD ,
    UIO_MSG_TYPE_MAX
}uio_msg_type_t;

typedef struct __attribute__((packed)) uio_cmd_dummy_params_s {
     int32_t data1;
     int32_t data2;
     int32_t data3;
}uio_cmd_dummy_params_t;

typedef struct __attribute__((packed)) uio_cmd_start_params_s {
	int32_t info;
}uio_cmd_start_params_t;
typedef struct __attribute__((packed)) uio_cmd_stop_params_s {
	int32_t info;
}uio_cmd_stop_params_t;
typedef struct __attribute__((packed)) uio_cmd_restart_params_s {
	int32_t info;
}uio_cmd_restart_params_t;

typedef struct __attribute__((packed)) uio_cmd_serial_baud_params_s {
	uint8_t baud;
}uio_cmd_serial_baud_params_t;

typedef struct __attribute__((packed)) uio_cmd_server_s {
    void* ptr;
}uio_cmd_server_t;

typedef union __attribute__((packed)) uio_msg_payload_u {
    uio_cmd_dummy_params_t dummy;
    uio_cmd_server_t cmd_server;
	uio_cmd_start_params_t* start_params;
	uio_cmd_stop_params_t* stop_params;
	uio_cmd_restart_params_t* restart_params;
	uio_cmd_serial_baud_params_t* serial_baud_params;
}uio_msg_payload_t;

typedef struct __attribute__((packed)) uio_msg_header_s {
     int16_t type;
     int16_t id;
}uio_msg_header_t;

typedef struct __attribute__((packed)) uio_msg_s {
     uio_msg_header_t header;
     uio_msg_payload_t payload;
}uio_msg_t;

int uio_raise_init_event(void);
int uio_raise_dummy_event(void);
int uio_raise_serial_event(void);
int uio_raise_net_ack_timeout_event(void);
int uio_raise_net_event(void);
int uio_raise_net_conn_reset_event(void);
int uio_raise_net_ack_enable_event(void);
int uio_raise_net_ack_disable_event(void);

#endif /* UIO_H__ */
