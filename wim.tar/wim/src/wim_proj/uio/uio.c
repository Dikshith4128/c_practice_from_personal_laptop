#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>



#include <sys/time.h>
#include <sys/resource.h>

#include <pthread.h>
#include <sched.h>
#include <sys/epoll.h>

/* Arete Linux SDK */
#include <alsdk.h>
#include <alsdk_bsema.h>
#include <alsdk_epoll.h>

/* WIM Application */
#include "main.h"
#include "project.h"
#include "app_debug.h"
#include "rt_timer.h"
#include "lt_timer.h"
#include "weigh.h"
#include "libwim.h"
#include "uio.h"
#include "uio_serial.h"
#include "uio_net.h"

#include "wim.h"
#include "cfg.h"
#include "cfg_active.h"


/* UIO serial port1 */
uio_serial_t uio_serial_1;
char uio_serial1_rbuf[UIO_SERIAL_PORT1_RB_RBUFF_SIZE];
char uio_serial1_wbuf[UIO_SERIAL_PORT1_RB_WBUFF_SIZE];
char uio_serial1_pbuf[UIO_SERIAL_PORT1_PBUFF_SIZE];

/* UIO net  ifc */
uio_net_t uio_net_1;
char uio_net1_rbuf[UIO_NET1_IFC_RB_RBUFF_SIZE];
char uio_net1_wbuf[UIO_NET1_IFC_RB_WBUFF_SIZE];
char uio_net1_pbuf[UIO_NET1_IFC_PBUFF_SIZE];

extern volatile uint8_t uio_net1_ifc_enable_ack;
extern volatile uint8_t uio_net1_ifc_rb_wbuff_msg_count; /*no of msgs to store*/

#define UIO_TD_QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, UIO_TD_QUEUE_MAXMSG, UIO_TD_QUEUE_MSGSIZE, 0, {0}})
#define UIO_TD_EPOLL_EVENT_INITIALIZER ((struct epoll_event){EPOLLIN | EPOLLOUT})

alsdk_thread_info_t uio_td_tif;
mqd_t uio_td_mqd;

static int uio_td_epfd;
static int uio_td_sfd;

/*------ uio events start -----*/
int uio_raise_init_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_INIT;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_dummy_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_DUMMY;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_serial_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_SERIAL;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_net_ack_timeout_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_NET_ACK_TIMEOUT;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_net_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_NET;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_net_conn_reset_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_NET_CONN_RESET;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_net_ack_enable_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_NET_ACK_ENABLE;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int uio_raise_net_ack_disable_event(void)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_EVENT;
	msg.header.id = UIO_EVENT_NET_ACK_DISABLE;
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) , UIO_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
/*------ uio events end -----*/
/*------ uio commands start -----*/

static alsdk_bsem_t uio_cmd_sync_bsema;
static pthread_mutex_t uio_cmd_seralization_mutex;

int uio_send_start_command(uio_cmd_start_params_t* start_params)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_CMD;
	msg.header.id = UIO_CMD_START;
	msg.payload.start_params = start_params;

	alsdk_pthread_mutex_lock(&uio_cmd_seralization_mutex);
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) + sizeof(uio_cmd_server_t) , UIO_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&uio_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&uio_cmd_seralization_mutex);

	return 0;
}
int uio_send_stop_command(uio_cmd_stop_params_t* stop_params)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_CMD;
	msg.header.id = UIO_CMD_STOP;
	msg.payload.stop_params = stop_params;

	alsdk_pthread_mutex_lock(&uio_cmd_seralization_mutex);
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) + sizeof(uio_cmd_server_t) , UIO_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&uio_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&uio_cmd_seralization_mutex);

	return 0;
}
int uio_send_restart_command(uio_cmd_restart_params_t* restart_params)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_CMD;
	msg.header.id = UIO_CMD_RESTART;
	msg.payload.restart_params = restart_params;

	alsdk_pthread_mutex_lock(&uio_cmd_seralization_mutex);
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) + sizeof(uio_cmd_server_t) , UIO_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&uio_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&uio_cmd_seralization_mutex);

	return 0;
}
int uio_send_serial_baud_command(uio_cmd_serial_baud_params_t* serial_baud_params)
{
	APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	uio_msg_t msg;
	msg.header.type = UIO_MSG_TYPE_CMD;
	msg.header.id = UIO_CMD_SERIAL_BAUD;
	msg.payload.serial_baud_params = serial_baud_params;

	alsdk_pthread_mutex_lock(&uio_cmd_seralization_mutex);
	while(mq_send(uio_td_mqd, (char*) &msg, sizeof(uio_msg_header_t) + sizeof(uio_cmd_server_t) , UIO_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&uio_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&uio_cmd_seralization_mutex);

	return 0;
}

/*------ uio commands end -----*/

/*------ uio timers start -----*/
void uio_net_ack_timer_handler(void * user_data)
{
    uio_raise_net_ack_timeout_event();
	//printf("t1\n");
}
lt_timer_obj_t uio_net_ack_timer = LT_TIMER_OBJ_INITIALIZER;


/*------ uio timers end -----*/

static int uio_thread_arg;
static void* uio_thread_func(void *args)
{
    APP_DEBUGF(UIO_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_setname_np(alsdk_pthread_self(), "UIO");
	setpriority(PRIO_PROCESS, 0, -18);//0

	int n, event_count;
    struct epoll_event events[UIO_TD_EPOLL_MAX_EVENTS];

    uio_msg_t msg;
    unsigned int msg_prio;
    ssize_t msg_len;


	(void) args; /* Suppress -Wunused-parameter warning. */

	/* wait for wim init barrier */
	weigh_init_barrier_wait(__func__);

    /* wait for wim cfg barrier */
	weigh_cfg_barrier_wait(__func__);
	lt_timer_obj_init(&uio_net_ack_timer, uio_net_ack_timer_handler, LT_TIMER_SINGLE_SHOT, NULL, 1500);
	//lt_timer_obj_init(&uio_net_ack_timer, uio_net_ack_timer_handler, LT_TIMER_SINGLE_SHOT, NULL, 5000);

	uio_net_listen(&uio_net_1);
	/* wait for wim cmd barrier */
	weigh_cmd_barrier_wait(__func__);

	for(;;) {

		event_count = epoll_wait(uio_td_epfd, events, UIO_TD_EPOLL_MAX_EVENTS, -1);
		//APP_DEBUGF(UIO_MQ_DEBUG|APP_DBG_TRACE, "\nUIO : %d ready events\n", event_count);
		////printf("state= %d\n", uio_net_1.state);

		for(n = 0; n < event_count; n++) {
			if(events[n].data.fd == uio_td_mqd) {
				APP_DEBUGF(UIO_MQ_DEBUG|APP_DBG_TRACE, "UIO : Reading file descriptor %d\n", events[n].data.fd);
				while((msg_len = mq_receive(uio_td_mqd, (char*) &msg, sizeof(uio_msg_t), &msg_prio)) > 0) {
					switch(msg.header.type) {
					case UIO_MSG_TYPE_EVENT:
						switch(msg.header.id) {
						case UIO_EVENT_INIT:
							APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_INIT\n");
						//	uio_net_listen(&uio_net_1);
							break;
						case UIO_EVENT_SERIAL:
							//APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_SERIAL\n");
							uio_serial_flush_line_from_buff(&uio_serial_1);
							break;
						case UIO_EVENT_NET_ACK_TIMEOUT:
							//APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_NET_ACK_TIMEOUT\n");
							uio_net_handle_ack_timeout(&uio_net_1);
							break;
						case UIO_EVENT_NET:
							//APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_NET\n");
							//printf("UIO_EVENT_NET\n");
							uio_net_flush_line_from_buff(&uio_net_1);
							break;
						case UIO_EVENT_NET_CONN_RESET:
							//APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_NET\n");
							uio_net_conn_reset(&uio_net_1);
							uio_net_reset_send_buff(&uio_net_1);
							break;
						case UIO_EVENT_NET_ACK_ENABLE:
							uio_net_enable_ack(&uio_net_1);
							break;
						case UIO_EVENT_NET_ACK_DISABLE:
							uio_net_disable_ack(&uio_net_1);
							break;
						case UIO_EVENT_DUMMY:
							APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_DUMMY\n");
							break;
						case UIO_EVENT_MAX:
							APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_MAX\n");
							break;
						default:
							APP_DEBUGF(UIO_EVENT_DEBUG|APP_DBG_TRACE, "UIO_EVENT_ILL\n");
							break;
						}
						break;
					case UIO_MSG_TYPE_CMD:
						switch(msg.header.id) {
						case UIO_CMD_START:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_START\n");
							break;
						case UIO_CMD_STOP:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_STOP\n");
							break;
						case UIO_CMD_RESTART:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_RESTART\n");
							break;
						case UIO_CMD_SERIAL_BAUD:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_BAUD\n");
							uio_serial_set_baudrate(&uio_serial_1, msg.payload.serial_baud_params->baud);
							alsdk_bsem_post(&uio_cmd_sync_bsema);
							break;
						case UIO_CMD_MAX:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_MAX\n");
							break;
						default:
							APP_DEBUGF(UIO_CMD_DEBUG|APP_DBG_TRACE, "UIO_CMD_ILL\n");
							break;
						}
						break;
					default:
						break;
					}
				}
			}else if(events[n].data.fd == uio_serial_1.fd) {
				//printf("UIO : Reading serial file descriptor %d\n ", events[n].data.fd);
				if(wim_runtime_cfg.type == WIM_TYPE_AXLE) {
					uio_serial_do_read_axle(&uio_serial_1);
				}else{
					uio_serial_do_read(&uio_serial_1);
				}
			}else if(events[n].data.fd == uio_net_1.fd) {
				//APP_DEBUGF(UIO_MQ_NET_EVENT_DBG_ON|APP_DBG_TRACE, "UIO_NET_FD_EVENT\n");
				if(wim_get_comm()== WIM_COMM_NET) {
					uio_net_handle_accept(&uio_net_1);
				}else {
					uio_net_handle_accept_serial(&uio_net_1);
				}
			}else if((uio_net_1.state == UIO_NET_CONNECTED) && (events[n].data.fd == uio_net_1.open_fd)) {
				//APP_DEBUGF(UIO_MQ_NET_EVENT_DBG_ON|APP_DBG_TRACE, "UIO_NET_OPEN_FD_EVENT\n");
				/*TODO : BROKEN NET change \r\n to \0 to input */
				if(events[n].events & EPOLLIN) {
					if(wim_runtime_cfg.type == WIM_TYPE_AXLE) {
					}else {
						uio_net_do_read(&uio_net_1);
					}
				}else if(events[n].events & EPOLLOUT) {
					//uio_net_do_write(&uio_net_1, net1_buf);
				}
			}else if(events[n].data.fd == 0) {
#define READ_SIZE 10
				size_t bytes_read;
				char read_buffer[READ_SIZE + 1];
                bytes_read = read(events[n].data.fd, read_buffer, READ_SIZE);
                read_buffer[bytes_read] = '\0';
                printf("UIO %s\n", read_buffer);
			 }else {
				 APP_DEBUGF(UIO_MQ_DEBUG|APP_DBG_TRACE, "UIO : Spurious file descriptor\n");
			 }
		}
	}

	pthread_exit((void*)0);
}

int uio_init(void)
{
    APP_DEBUGF(UIO_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	/* Initialize the queue attributes */
	struct mq_attr attr = UIO_TD_QUEUE_ATTR_INITIALIZER;


	/* Create the message queue. The queue reader is NONBLOCK. */
	uio_td_mqd = alsdk_mq_open(UIO_TD_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, UIO_TD_QUEUE_PERMS, &attr);

	struct epoll_event event = UIO_TD_EPOLL_EVENT_INITIALIZER;
	/* Create epoll interface */
	uio_td_epfd = alsdk_epoll_create1(EPOLL_CLOEXEC);

	int baud = uio_serial_get_baud_from_benum(cfg_active_wim_core.baud_rate);

	if(uio_serial_init(&uio_serial_1, UIO_SERIAL_PORT1, baud, UIO_SERIAL_RDWR,
				uio_serial1_rbuf , UIO_SERIAL_PORT1_RB_RBUFF_SIZE,
				uio_serial1_wbuf , UIO_SERIAL_PORT1_RB_WBUFF_SIZE,
				uio_serial1_pbuf , UIO_SERIAL_PORT1_PBUFF_SIZE) ) {


		alsdk_err_msg("uio_serial1_init\n");
	}
	if(uio_serial_open(&uio_serial_1)) {
		alsdk_err_msg("uio_serial1_open\n");

	}

	if(cfg_active_wim_core.noack == 0) {
		uio_net1_ifc_enable_ack = 1;
		uio_net1_ifc_rb_wbuff_msg_count = 5; /*no of msgs to store*/
	}else {
		uio_net1_ifc_enable_ack = 0;
		uio_net1_ifc_rb_wbuff_msg_count = 1; /*no of msgs to store*/
	}


	/*Net Init */
	if(uio_net_init(&uio_net_1, uio_td_epfd, &uio_net_ack_timer, UIO_NET1_IFC, project_info.cust.net_io_port,
				uio_net1_rbuf , UIO_NET1_IFC_RB_RBUFF_SIZE,
				uio_net1_wbuf , UIO_NET1_IFC_RB_WBUFF_SIZE,
				uio_net1_pbuf , UIO_NET1_IFC_PBUFF_SIZE) ) {

		alsdk_err_msg("uio_net1_init\n");
	}
	if(uio_net_open(&uio_net_1)) {
		alsdk_err_msg("uio_net1_open\n");

	}


	//uio_serial_send(&uio_serial_1, "TR#\r\n", 5);

	/* add epoll fds */
	alsdk_epoll_add(uio_td_epfd, uio_td_mqd, EPOLLIN);
	alsdk_epoll_add(uio_td_epfd, uio_serial_1.fd, EPOLLIN);
	alsdk_epoll_add(uio_td_epfd, uio_net_1.fd, EPOLLIN);
	alsdk_epoll_add(uio_td_epfd, 0, EPOLLIN);

	/* sync semas and mutex */
	alsdk_bsem_init(&uio_cmd_sync_bsema, 0);
	alsdk_pthread_mutex_init(&uio_cmd_seralization_mutex, NULL);

	/* cpu set 0 */
    cpu_set_t cpuset_0;
    CPU_ZERO(&cpuset_0);
    CPU_SET(0, &cpuset_0);

	/* Create the thread */
	uio_td_tif.args = (void*) &uio_thread_arg;
    alsdk_pthread_create(&uio_td_tif.thread_id, NULL, uio_thread_func, &uio_td_tif);
	alsdk_pthread_setaffinity_np(uio_td_tif.thread_id, sizeof(cpu_set_t), &cpuset_0);
	return 0;
}

int uio_join(void)
{
    APP_DEBUGF(UIO_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_join(uio_td_tif.thread_id, NULL);

	return 0;
}

int uio_cancel(void)
{
    APP_DEBUGF(UIO_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_cancel(uio_td_tif.thread_id);
	return 0;
}

int uio_exit(void)
{
    APP_DEBUGF(UIO_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_epoll_del(uio_td_epfd, uio_serial_1.fd);
	uio_serial_close(&uio_serial_1);
	alsdk_epoll_del(uio_td_epfd, uio_net_1.fd);
	close(uio_net_1.fd);
	close(uio_net_1.open_fd);
	//alsdk_epoll_del(uio_td_epfd, 0);
	alsdk_mq_close(uio_td_mqd);
	alsdk_mq_unlink(UIO_TD_QUEUE_NAME);

	return 0;
}
