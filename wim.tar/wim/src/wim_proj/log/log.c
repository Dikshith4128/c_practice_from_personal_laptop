#define _GNU_SOURCE
#include <config.h>
#include <math.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <pthread.h>
#include <sched.h>
#include <sys/epoll.h>

#include <libudev.h>
#include <libudev.h>

#include <alsdk.h>
#include <alsdk_bsema.h>
#include <alsdk_epoll.h>

/* WIM Application */
#include "main.h"
#include "project.h"
#include "app_debug.h"
#include "weigh.h"
#include "libwim.h"
#include "log.h"
#include "log_console.h"
#include "log_sd.h"
#include "cfg_active.h"

log_sd_t log_sd;
uint8_t log_sd_wbuf[LOG_SD_RB_WBUFF_SIZE];

#define LOG_TD_QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, LOG_TD_QUEUE_MAXMSG, LOG_TD_QUEUE_MSGSIZE, 0, {0}})
//#define LOG_TD_EPOLL_EVENT_INITIALIZER ((struct epoll_event){EPOLLIN | EPOLLOUT | EPOLLET})
#define LOG_TD_EPOLL_EVENT_INITIALIZER ((struct epoll_event){EPOLLIN | EPOLLOUT})

alsdk_thread_info_t log_td_tif;
mqd_t log_td_mqd;

static int log_td_epfd;

/* datalogging enable or disable */
static volatile  uint8_t log_en_dis_flag = 1;

uint8_t log_get_log_flag(void)
{
	return log_en_dis_flag;
}




/* udev moniter*/
#define LOG_UDEV_SUBSYSTEM "block"
/* List and monitor USB devices using libudev.
 *
 */
struct udev* log_udev;
struct udev_monitor* log_udev_mon;
int log_udev_mon_fd;

static void print_device(struct udev_device* dev)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	//printf("%s\n", __func__);
    const char* action = udev_device_get_action(dev);
    if(!action)
        action = "exists";

    const char* vendor = udev_device_get_sysattr_value(dev, "idVendor");
    if(!vendor)
        vendor = "0000";

    const char* product = udev_device_get_sysattr_value(dev, "idProduct");
    if(!product)
        product = "0000";

    printf("%s %s %6s %s:%s %s\n",
           udev_device_get_subsystem(dev),
           udev_device_get_devtype(dev),
           action,
           vendor,
           product,
           udev_device_get_devnode(dev));
}

static void process_device(struct udev_device* dev)
{
	//printf("%s\n", __func__);
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
    if(dev) {
        if(udev_device_get_devnode(dev))
            print_device(dev);

        udev_device_unref(dev);
    }
}

int log_udev_mon_init(void)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
     log_udev = udev_new();
     if(!log_udev) {
		 APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "udev_new() failed\n");
         return -1;
     }

    log_udev_mon = udev_monitor_new_from_netlink(log_udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(log_udev_mon, LOG_UDEV_SUBSYSTEM, NULL);
    udev_monitor_enable_receiving(log_udev_mon);

   log_udev_mon_fd = udev_monitor_get_fd(log_udev_mon);
   return 0;

}


/*------ log events start -----*/
int log_raise_init_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_INIT;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int log_raise_dummy_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_DUMMY;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}

int log_raise_log_enable_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_LOG_ENABLE;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int log_raise_log_disable_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_LOG_DISABLE;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}


int log_raise_sd_up_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_SD_UP;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int log_raise_sd_down_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_SD_DOWN;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}



int log_raise_eod_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_EOD;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
int log_raise_sd_event(void)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_EVENT;
	msg.header.id = LOG_EVENT_SD;
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) , LOG_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
/*------ log events end -----*/
/*------ log commands start -----*/

static alsdk_bsem_t log_cmd_sync_bsema;
static pthread_mutex_t log_cmd_seralization_mutex;

int log_send_start_command(log_cmd_start_params_t* start_params)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_CMD;
	msg.header.id = LOG_CMD_START;
	msg.payload.start_params = start_params;

	alsdk_pthread_mutex_lock(&log_cmd_seralization_mutex);
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) + sizeof(log_cmd_server_t) , LOG_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&log_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&log_cmd_seralization_mutex);

	return 0;
}
int log_send_stop_command(log_cmd_stop_params_t* stop_params)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_CMD;
	msg.header.id = LOG_CMD_STOP;
	msg.payload.stop_params = stop_params;

	alsdk_pthread_mutex_lock(&log_cmd_seralization_mutex);
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) + sizeof(log_cmd_server_t) , LOG_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&log_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&log_cmd_seralization_mutex);

	return 0;
}
int log_send_restart_command(log_cmd_restart_params_t* restart_params)
{
	APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	log_msg_t msg;
	msg.header.type = LOG_MSG_TYPE_CMD;
	msg.header.id = LOG_CMD_RESTART;
	msg.payload.restart_params = restart_params;

	alsdk_pthread_mutex_lock(&log_cmd_seralization_mutex);
	while(mq_send(log_td_mqd, (char*) &msg, sizeof(log_msg_header_t) + sizeof(log_cmd_server_t) , LOG_TD_QUEUE_CMD_PRIO) < 0);
	alsdk_bsem_wait(&log_cmd_sync_bsema);
	alsdk_pthread_mutex_unlock(&log_cmd_seralization_mutex);

	return 0;
}

/*------ log commands end -----*/

/*------ log utils start -----*/
 // buf needs to store 30 characters
static int log_thread_arg;
static void* log_thread_func(void *args)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_setname_np(alsdk_pthread_self(), "LOG");
	setpriority(PRIO_PROCESS, 0, -18);//0

	int n, event_count;
    struct epoll_event events[LOG_TD_EPOLL_MAX_EVENTS];

	weigh_msg_t msg;
	unsigned int msg_prio;
	ssize_t msg_len;

	(void) args; /* Suppress -Wunused-parameter warning. */

	/* wait for wim init barrier */
	weigh_init_barrier_wait(__func__);

    /* wait for wim cfg barrier */
	weigh_cfg_barrier_wait(__func__);

	/* wait for wim cmd barrier */
	weigh_cmd_barrier_wait(__func__);
	log_raise_init_event();

	for(;;) {

		event_count = epoll_wait(log_td_epfd, events, LOG_TD_EPOLL_MAX_EVENTS, -1);
		APP_DEBUGF(LOG_MQ_DEBUG|APP_DBG_TRACE, "\nLOG : %d ready events\n", event_count);
		for(n = 0; n < event_count; n++) {
			if(events[n].data.fd == log_udev_mon_fd) {
				struct udev_device* dev = udev_monitor_receive_device(log_udev_mon);
				process_device(dev);
			}else if(events[n].data.fd == log_td_mqd) {
				APP_DEBUGF(LOG_MQ_DEBUG|APP_DBG_TRACE, "LOG : Reading file descriptor %d\n", events[n].data.fd);
				while((msg_len = mq_receive(log_td_mqd, (char*) &msg, sizeof(log_msg_t), &msg_prio)) > 0) {
					//log_print_msg_hdr(&msg);
					switch(msg.header.type) {
					case LOG_MSG_TYPE_EVENT:
						switch(msg.header.id) {
						case LOG_EVENT_INIT:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_INIT\n");
							break;
						case LOG_EVENT_LOG_ENABLE:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_ENABLE\n");
							log_en_dis_flag = 1;
							break;
						case LOG_EVENT_LOG_DISABLE:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_DISABLE\n");
							log_en_dis_flag = 0;
							break;
						case LOG_EVENT_SD_UP:
							break;
						case LOG_EVENT_SD_DOWN:
							break;
						case LOG_EVENT_EOD:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_EOD\n");
							log_sd_handle_eod(&log_sd);
							break;
						case LOG_EVENT_SD:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_SD\n");
							/* check EOD */
							log_sd_flush_line_from_buff(&log_sd);
							break;
						case LOG_EVENT_DUMMY:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_DUMMY\n");
							break;
						case LOG_EVENT_MAX:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_MAX\n");
							break;
						default:
							APP_DEBUGF(LOG_EVENT_DEBUG|APP_DBG_TRACE, "LOG_EVENT_ILL\n");
							break;
						}
						break;
					case LOG_MSG_TYPE_CMD:
						switch(msg.header.id) {
						case LOG_CMD_START:
							APP_DEBUGF(LOG_CMD_DEBUG|APP_DBG_TRACE, "LOG_CMD_START\n");
							break;
						case LOG_CMD_STOP:
							APP_DEBUGF(LOG_CMD_DEBUG|APP_DBG_TRACE, "LOG_CMD_STOP\n");
							break;
						case LOG_CMD_RESTART:
							APP_DEBUGF(LOG_CMD_DEBUG|APP_DBG_TRACE, "LOG_CMD_RESTART\n");
							break;
						case LOG_CMD_MAX:
							APP_DEBUGF(LOG_CMD_DEBUG|APP_DBG_TRACE, "LOG_CMD_MAX\n");
							break;
						default:
							APP_DEBUGF(LOG_CMD_DEBUG|APP_DBG_TRACE, "LOG_CMD_ILL\n");
							break;
						}
						break;
					default:
						break;
					}
				}
			}else {
				APP_DEBUGF(LOG_MQ_DEBUG|APP_DBG_TRACE, "LOG : Spurious file descriptor\n");
			}
		}
	}
	pthread_exit((void*)0);
}

int log_init(void)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	/* Initialize the queue attributes */
	struct mq_attr attr = LOG_TD_QUEUE_ATTR_INITIALIZER;

	/* Create the message queue. The queue reader is NONBLOCK. */
	log_td_mqd = alsdk_mq_open(LOG_TD_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, LOG_TD_QUEUE_PERMS, &attr);

   /*log_udev_mon init */
	if(log_udev_mon_init()) {
		alsdk_err_msg("log_udev_mon_init\n");
	}

   /*log_sd init */
	if(log_sd_init(&log_sd, LOG_SD_ROOT, log_sd_wbuf, LOG_SD_RB_WBUFF_SIZE)) {
		alsdk_err_msg("log_sd_init\n");
	}
	if(log_sd_open(&log_sd)) {
		alsdk_err_msg("log_sd_open\n");
    }
   /*log_console init */

   /*log_net init */

	struct epoll_event event = LOG_TD_EPOLL_EVENT_INITIALIZER;
	/* Create epoll interface */
	log_td_epfd = alsdk_epoll_create1(EPOLL_CLOEXEC);
	/* add epoll fds */
	alsdk_epoll_add(log_td_epfd, log_udev_mon_fd, EPOLLIN);
	alsdk_epoll_add(log_td_epfd, log_td_mqd, EPOLLIN);
	//alsdk_epoll_add(log_td_epfd, 0, EPOLLIN);

	/* sync semas and mutex */
	alsdk_bsem_init(&log_cmd_sync_bsema, 0);
	alsdk_pthread_mutex_init(&log_cmd_seralization_mutex, NULL);

	/* cpu set 0 */
    cpu_set_t cpuset_0;
    CPU_ZERO(&cpuset_0);
    CPU_SET(0, &cpuset_0);

	/* Create the thread */
	log_td_tif.args = (void*) &log_thread_arg;
    alsdk_pthread_create(&log_td_tif.thread_id, NULL, log_thread_func, &log_td_tif);
	alsdk_pthread_setaffinity_np(log_td_tif.thread_id, sizeof(cpu_set_t), &cpuset_0);
	return 0;
}
int log_join(void)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_join(log_td_tif.thread_id, NULL);

	return 0;
}

int log_cancel(void)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_pthread_cancel(log_td_tif.thread_id);
	return 0;
}

int log_exit(void)
{
	APP_DEBUGF(LOG_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	alsdk_epoll_del(log_td_epfd, log_udev_mon_fd);
	udev_unref(log_udev);
	alsdk_mq_close(log_td_mqd);
	alsdk_mq_unlink(LOG_TD_QUEUE_NAME);

	log_sd_unmount(&log_sd);

	return 0;
}
