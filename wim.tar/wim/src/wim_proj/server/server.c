#define _GNU_SOURCE
#include <config.h>

#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include <stddef.h>
#include <stdlib.h>

#include <setjmp.h>
#include <signal.h>

#include <math.h>

#include <stdio.h>


#include <sys/time.h>
#include <sys/resource.h>

#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/epoll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <alsdk.h>
#include <alsdk_random.h>
#include <alsdk_pthread.h>
#include <alsdk_epoll.h>

#include "device.h"
#include "system.h"

/* WIM Application */
#include "main.h"
#include "project.h"
#include "app_debug.h"
#include "board.h"
#include "license.h"
#include "cfg_active.h"
#include "lt_timer.h"
#include "libwim.h"
#include "weigh.h"
#include "server.h"
#include "wim.h"
#include "adc.h"
#include "axle.h"

#include "guio.h"
#include "guio_char_lcd.h"

#include <zmq.h>
#include <czmq.h>

#define SERVER_TD_QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, SERVER_TD_QUEUE_MAXMSG, SERVER_TD_QUEUE_MSGSIZE, 0, {0}})
//#define SERVER_TD_EPOLL_EVENT_INITIALIZER ((struct epoll_event){EPOLLIN | EPOLLOUT | EPOLLET})
#define SERVER_TD_EPOLL_EVENT_INITIALIZER ((struct epoll_event){EPOLLIN | EPOLLOUT})

alsdk_thread_info_t server_td_tif;
mqd_t server_td_mqd;


/* epoll */
//static int server_td_epfd;
static zmq_pollitem_t server_items[4];
/* network */
static int server_td_sock;

/* Zmq */
/* req/reply*/
void *server_td_zmq_rp_context;
void *server_td_zmq_rp_responder;
int server_td_zmq_rp_sock_fd;

/*adc data  pub/sub*/
void *server_td_zmq_adc_data_ps_context;
void *server_td_zmq_adc_data_ps_socket;
int server_td_zmq_adc_data_ps_sock_fd;

/*adc plaza data pub/sub*/
void *server_td_zmq_plaza_data_ps_context;
void *server_td_zmq_plaza_data_ps_socket;
int server_td_zmq_plaza_data_ps_sock_fd;

/* serv plaza timer */
lt_timer_obj_t srv_timer_plaza_obj = LT_TIMER_OBJ_INITIALIZER;
int srv_raise_init_event(void)
{
	return 0;
}
int srv_raise_plaza_event(void)
{
	//APP_DEBUGF(WIM_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	srv_msg_t msg;
	msg.header.type = SRV_MSG_TYPE_EVENT;
	msg.header.id = SRV_EVENT_PLAZA;
	while(mq_send(server_td_mqd, (char*) &msg, sizeof(wim_msg_header_t) , SERVER_TD_QUEUE_EVENT_PRIO) < 0);

	return 0;
}
void srv_timer_plaza_handler(void * user_data)
{
	//APP_DEBUGF(WIM_EVENT_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	srv_raise_plaza_event();
}


/* loadcell param Req Reply */


/* Server message structures */
/*SRV_RT_REBOOT					0*/
/*SRV_RT_RESTART				1*/
/* SRV_GET_FW_INFO					2 */
/* SRV_INSTALL_FW				3 */
/* SRV_SET_ASSET_ID					4 */
/* SRV_GET_ASSET_ID						5 */
/* SRV_SET_LOAD_CELL_PARAMETERS			6 */
/* SRV_GET_LOAD_CELL_PARAMETERS			7 */
/* SRV_SET_NETWORK_PARAMS				8 */
/* SRV_GET_NETWORK_PARAMS			    9 */
/* SRV_SET_CONFIGURATION_DATA			10 */
/* SRV_GET_CONFIGURATION_DATA			11 */
/* SRV_SET_KNOBS						12 */
/* SRV_GET_KNOBS						13 */
#if 0
/* SRV_SET_SUPPRESS_NAN					14 */
/* SRV_GET_SUPRESS_NAN_STATUS			15 */
#endif
/* SRV_SET_USER_OPTIONS					14 */
/* SRV_GET_USER_OPTIONS					15 */
/* SRV_SET_FILTER						16 */
/* SRV_GET_FILTER						17 */
/* SRV_SET_DATA_RATE					18 */
uint8_t srv_data_rate = 1;
/* SRV_SET_COM_CHANNEL					19 */
/* SRV_GET_COM_CHANNEL					20 */
uint8_t srv_com_channel = 1;
/* SRV_SET_FACTORY_DEFAULT				21 */
uint8_t srv_factory_default;
/* SRV_SET_WIM_MODE						22 */
/* SRV_GET_WIM_MODE						23 */
uint8_t srv_wim_mode = 0;
/* SRV_SET_DEFAULT_WIM_MODE				24 */
uint8_t srv_default_wim_mode = 1;
/* SRV_SET_BP_CFG	 					25 */
/* SRV_GET_BP_CFG						26 */
/* SRV_SET_VS_MODE						27 */
/* SRV_GET_VS_MODE						28 */
/* SRV_SET_SPEED_FACTOR					29 */
/* SRV_GET_SPEED_FACTOR					30 */
/* SRV_SND_OTP							31 */
/* SRV_RCV_OTP							32 */
/* SRV_GEN_OTP							33 */
/* SRV_VAL_OTP							34 */
/* SRV_SET_CAL_TABLE					35 */
/* SRV_GET_CAL_TABLE					36 */

/* SRV_LIVE_DATA_REQUIRED				37 */
volatile uint8_t srv_live_data_flag = 0;
/* SRV_RECEIVE_LIVE_DATA				38 */
/* SRV_SET_ZERO_SETTING_RANGES			38 */
/* to do*/
/* SRV_GET_ADC_CODE					39 */
uint32_t srv_adc_code;
//float weigh_get_inst_value(void);
/* SRV_SET_EN_ZERO_AT_POWER_UP			40 */
/* SRV_GET_EN_ZERO_AT_POWER_UP			41 */
uint8_t srv_en_zero_at_power_up_flag;
/* SRV_EN_AUTO_ZERO						42 */
/* SRV_GET_AUTO_ZERO_STATUS				43 */
uint8_t srv_auto_zero_flag;
/* SRV_GET_AUTO_ZERO_VALUE				44 */
/* SRV_SET_ZERO							45 */
uint8_t srv_zero_flag;
/* SRV_GET_ZERO_VALUE					46 */
float srv_zero_value;
/* SRV_CANCEL_ZERO						47 */
uint8_t srv_cancel_zero_flag;
/* SRV_axle_RECORD_REQUIRED				48 */
uint8_t srv_axle_record_flag;
/* SRV_RECEIVE_axle_RECORD				49 */
/* ADC Data axle record */

/* SRV_RECEIVE_axle_DATA				50 */
/* SRV_GET_HW_STATUS					51 */
srv_hw_status_t srv_hw_status = {1, 1, 30, 0, 1};
/* SRV_RECEIVE_CPU_LOAD					52 */
/* SRV_SET_DATA_LOGGING_FUNCTION		53 */
/* SRV_GET_DATA_LOGGING_FUNCTION		54 */
/* SRV_RECEIVE_SD_STATUS				55 */
srv_sd_status_t srv_sd_status;
/* SRV_SET_PLAZA_UPDATE					56 */
/* SRV_GET_PLAZA_UPDATE					57 */
volatile srv_plaza_update_t srv_plaza_update;

/* SRV_RECEIVE_DATA_LOGGING_STATUS		58 */
uint8_t srv_data_logging_status_flag;
/* SRV_SET_TIME							59 */
/* SRV_RECEIVE_CURRENT_TIME				60 */
/* SRV_GET_DATA_UPDATES					61 */
/* SRV_GET_SETTINGS_UPDATES				62 */
/* SRV_LAST								65 */

//static srv_msg_rp_header_t srv_msg_rp_header;
static srv_msg_rp_t srv_msg_rp;

int server_zmq_tcp_rp_serve_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	return 0;
}

extern volatile uint32_t adc_m4_sample_raw;
extern volatile int32_t adc_m4_sample;
extern volatile double adc_m4_ain;
extern volatile uint32_t m_srno;
extern volatile wim_vehicle_data_t curr_vehicle_data;
extern volatile float axle_hs_spread_max;
extern volatile float axle_hs_spread_min;
extern volatile float axle_hs_spread;
#define ALSDK_MEMBER_SIZE(type, member) sizeof(((type *)0)->member)
int server_zmq_tcp_rp_serve(void)
{
	//APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
		uint32_t tmp_otp;

	uint8_t msg_id;
	uint8_t msg_status = 0;
	int size;

	size = zmq_recv(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_t), 0);
	msg_id = srv_msg_rp.header.id;
	msg_status = srv_msg_rp.header.status;

    //APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "MSGID	 : %u \n", msg_id);
    //APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "MSGSTATUS : %u \n", msg_status);

	switch(msg_id) {
#if 1
	case SRV_GET_DATA_UPDATES:
		//APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_DATA_UPDATES : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;
        srv_msg_rp.payload.data_update.inst_value = weigh_get_inst_value();
        srv_msg_rp.payload.data_update.auto_zero_value = weigh_get_track_zero();
        srv_msg_rp.payload.data_update.zero_value =  weigh_get_zero();

		memcpy(&srv_msg_rp.payload.data_update.hw_status , &srv_hw_status, sizeof(srv_hw_status_t));

		srv_msg_rp.payload.data_update.hw_status.snrs_sts1 = wim_get_s1_status();
		srv_msg_rp.payload.data_update.hw_status.snrs_sts2 = wim_get_s2_status();

		if(axle_queue_isempty(&axle_queue)) {
			//memset(&srv_msg_rp.payload.data_update.axle.record, 0,  sizeof(srv_axle_record_t));
			srv_msg_rp.header.status = 0;
			srv_msg_rp.payload.data_update.u.wim.hs_spread = axle_hs_spread;
			srv_msg_rp.payload.data_update.u.wim.tr_no = curr_vehicle_data.tr_no;
			srv_msg_rp.payload.data_update.u.wim.no_axles = curr_vehicle_data.no_axles;
			srv_msg_rp.payload.data_update.u.wim.weight = curr_vehicle_data.weight;
			srv_msg_rp.payload.data_update.u.wim.exit_status = curr_vehicle_data.exit_status;
			/*ERR*/
			srv_msg_rp.payload.data_update.u.wim.err.dev.bytes = dev_err.bytes;
			srv_msg_rp.payload.data_update.u.wim.err.sys.bytes = sys_err.bytes;
			srv_msg_rp.payload.data_update.u.wim.err.m4.bytes =  m4_err.bytes;
			srv_msg_rp.payload.data_update.u.wim.err.weigh.bytes = weigh_err.bytes;
			srv_msg_rp.payload.data_update.u.wim.err.wim.bytes = wim_err.bytes;

			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_wim_t), 0);
			break;
		}else {
			axle_queue_dequeue(&axle_queue, &srv_msg_rp.payload.data_update.u.axle);
			srv_msg_rp.header.status = 1;
			srv_msg_rp.payload.data_update.u.axle.record.distance = (srv_msg_rp.payload.data_update.u.axle.record.distance/1000);
			//APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "#%-4u\t%-8.6f\t%-8.6f\t%-8.6f\t%-u\n", srv_msg_rp.payload.data_update.axle.record.no,
			//		srv_msg_rp.payload.data_update.axle.record.weight,srv_msg_rp.payload.data_update.axle.record.speed,
			//		srv_msg_rp.payload.data_update.axle.record.distance, srv_msg_rp.payload.data_update.axle.data_len);
			if(!srv_axle_record_flag) {
				zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t), 0);
			}else {
				//zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) + sizeof(srv_axle_data_t), 0);
				zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) +
						(srv_msg_rp.payload.data_update.u.axle.data_len* sizeof(float)), 0);
			}

		}
#if 0
		if(!srv_axle_record_flag) {
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t), 0);
		}else {
			//zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) + sizeof(srv_axle_data_t), 0);
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) +
					(srv_msg_rp.payload.data_update.axle.data_len* sizeof(float)), 0);
		}
#endif
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_SETTINGS_UPDATES:
		//APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_SETTINGS_UPDATES : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.settings_update.wim_mode = weigh_wim_get_wim_mode();
		srv_msg_rp.payload.settings_update.vs_mode = weigh_wim_get_vs_mode();
		srv_msg_rp.payload.settings_update.cal_lock = (uint8_t) io_get_weigh_cal_lock_line_state();
		srv_msg_rp.payload.settings_update.wim_dir = weigh_wim_get_wim_dir();
		srv_msg_rp.payload.settings_update.cpu_load = system_get_1_min_cpu_load_avg();
		srv_msg_rp.payload.settings_update.time_stamp = (int64_t) time(NULL);
		srv_msg_rp.payload.settings_update.rt_flag = false;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp , sizeof(srv_msg_rp_header_t) + sizeof(srv_settings_update_t), 0);
#if WIM_SERV_TRACE_MSG_RP_OUT_DATA
	//	printf("inst_value : %lu \n", srv_msg_rp.payload.mp);
#endif
		break;
#endif

	case SRV_RT_REBOOT:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RT_REBOOT : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, rt_reboot), 0);
		weigh_send_rt_reboot_command(&srv_msg_rp.payload.rt_reboot);
		break;
	case SRV_RT_RESTART:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RT_RESTART : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, rt_restart), 0);
		weigh_send_rt_restart_command(&srv_msg_rp.payload.rt_restart);
		break;
	case SRV_GET_FW_INFO:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_FW_INFO : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;

		weigh_send_get_fw_info_command(&srv_msg_rp.payload.fw_info);
#if 0
		printf("version      = %s\n", srv_msg_rp.payload.fw_info.version);
		printf("title        = %s\n", srv_msg_rp.payload.fw_info.title);
		printf("description  = %s\n", srv_msg_rp.payload.fw_info.description);
#endif
		uint8_t tmp_buffer[32];

        snprintf(tmp_buffer, 32, "{%s}{%s}", project_image_info.version, project_m4_info.version);
		strncat(srv_msg_rp.payload.fw_info.description, tmp_buffer, 64);
#if 0
		printf("image version  = %s\n", project_image_info.version);
		printf("m4 version     = %s\n", project_m4_info.version);
		printf("1 + 2          = %s\n", tmp_buffer);
		printf("description    = %s\n", srv_msg_rp.payload.fw_info.description);
#endif
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_fw_info_t), 0);
		break;
	case SRV_INSTALL_FW:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_INSTALL_FW : %u\n", msg_id);
		//printf("SRV_INSTALL_FW : %d : %s\n", msg_id, srv_msg_rp.payload.install_fw.name);

		srv_msg_rp.header.id = msg_id;

		srv_msg_rp.header.status = system_install_fw(&srv_msg_rp.payload.install_fw.name);
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_install_fw_t) , 0);

		if(srv_msg_rp.header.status == 0) {
			guio_char_lcd_send(&guio_char_lcd_1, "\x1b[LI\x1b[LC\x1b[Lb\x1b[LN", 16);
			guio_char_lcd_send(&guio_char_lcd_1, "\x1b[Lx0y0;New FW Installed ", 24);
			guio_char_lcd_send(&guio_char_lcd_1, "\x1b[Lx0y1;Restarting...    ", 24);

			exit(EXIT_SUCCESS);
		}
		break;
	case SRV_SET_ASSET_ID:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_ASSET_ID : %u\n", msg_id);
		//printf("SRV_SET_ASSET_ID : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_asset_info_command(&srv_msg_rp.payload.asset_info);

		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_asset_info_t) - (sizeof(srv_module_info_t) + sizeof(srv_board_info_t)), 0);

		break;
	case SRV_GET_ASSET_ID:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_ASSET_ID : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;

		memset(&srv_msg_rp.payload.asset_info, 0, sizeof(srv_asset_info_t));

		weigh_send_get_asset_info_command(&srv_msg_rp.payload.asset_info);

		snprintf(&srv_msg_rp.payload.asset_info.asset_id, sizeof(srv_msg_rp.payload.asset_info.asset_id), "%u-%s", license_device_info.board_ser_no, license_device_info.module_ser_no);
		//printf("asset_id %s", srv_msg_rp.payload.asset_info.asset_id);
		snprintf(&srv_msg_rp.payload.asset_info.system_status, sizeof(srv_msg_rp.payload.asset_info.system_status), "online");
#if 0
		printf("loc_id  = %s\n", srv_msg_rp.payload.asset_info.location_info.location_id);
		printf("lane_id = %s\n", srv_msg_rp.payload.asset_info.location_info.lane_id);
		printf("ass_id  = %s\n", srv_msg_rp.payload.asset_info.asset_id);
		printf("stat_id = %s\n", srv_msg_rp.payload.asset_info.system_status);
#endif
		snprintf(&srv_msg_rp.payload.asset_info.module_info.ser_no, sizeof(srv_msg_rp.payload.asset_info.module_info.ser_no), "%s", license_device_info.module_ser_no);
		snprintf(&srv_msg_rp.payload.asset_info.module_info.mac, sizeof(srv_msg_rp.payload.asset_info.module_info.mac), "%s", license_device_info.module_eth0_mac);
		srv_msg_rp.payload.asset_info.module_info.type = 0;

		/*BOARD EEPROM INFO*/
		memcpy(&srv_msg_rp.payload.asset_info.board_info , &board_eeprom_info, sizeof(srv_board_info_t));


		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_asset_info_t), 0);
		break;
	case SRV_SET_LOAD_CELL_PARAMETERS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_LOAD_CELL_PARAMETERS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_loadcell_parameters_command(&srv_msg_rp.payload.loadcell_params);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_loadcell_params_t), 0);
		break;
	case SRV_GET_LOAD_CELL_PARAMETERS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_LOAD_CELL_PARAMETERS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_loadcell_parameters_command(&srv_msg_rp.payload.loadcell_params);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_loadcell_params_t), 0);
		break;
	case SRV_SET_NETWORK_PARAMS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_NETWORK_PARAMS : %u\n", msg_id);
		//printf("SRV_SET_NETWORK_PARAMS : %u\n", msg_id);

		//printf("MODE:%u\n", srv_msg_rp.payload.network_params.ip_addr_req_mode);
		//printf("IP:%s\n", srv_msg_rp.payload.network_params.ip_addr);
		//printf("MS:%s\n", srv_msg_rp.payload.network_params.ip_addr_subnet_mask);
		//printf("GT:%s\n", srv_msg_rp.payload.network_params.ip_addr_gateway);
		//printf("DN:%s\n", srv_msg_rp.payload.network_params.ip_addr_dns_server);

		srv_msg_rp.header.id = msg_id;
		//weigh_send_set_network_params_command(&srv_msg_rp.payload.network_params);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_network_params_t), 0);
		weigh_send_set_network_params_command(&srv_msg_rp.payload.network_params);
		break;
	case SRV_GET_NETWORK_PARAMS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_NETWORK_PARAMS : %u\n", msg_id);
		//printf("SRV_GET_NETWORK_PARAMS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_network_params_command(&srv_msg_rp.payload.network_params_get);
		srv_msg_rp.header.status = msg_status;
		//printf("MAC:%s\n", srv_msg_rp.payload.network_params_get.ip_eth0_mac);
		//printf("MODE:%u\n", srv_msg_rp.payload.network_params_get.ip_addr_mode);
		//printf("IP:%s\n", srv_msg_rp.payload.network_params_get.ip_addr);
		//printf("MS:%s\n", srv_msg_rp.payload.network_params_get.ip_addr_subnet_mask);
		//printf("GT:%s\n", srv_msg_rp.payload.network_params_get.ip_addr_gateway);
		//printf("DN:%s\n", srv_msg_rp.payload.network_params_get.ip_addr_dns_server);

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_network_params_get_t), 0);
		break;
	case SRV_SET_CONFIGURATION_DATA:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_CONFIGURATION_DATA : %u\n", msg_id);

		weigh_send_set_configuration_data_command(&srv_msg_rp.payload.wim_config_data);

		srv_msg_rp.header.id = msg_id;

		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_wim_config_data_t), 0);
		break;
	case SRV_GET_CONFIGURATION_DATA:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_CONFIGURATION_DATA : %u\n", msg_id);
		weigh_send_get_configuration_data_command(&srv_msg_rp.payload.wim_config_data);
		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_wim_config_data_t), 0);
		break;
	case SRV_SET_KNOBS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_KNOBS : %u\n", msg_id);

		weigh_send_set_wim_knobs_command(&srv_msg_rp.payload.knobs);
		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_knobs_t), 0);
		break;
	case SRV_GET_KNOBS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_KNOBS : %u\n", msg_id);

		weigh_send_get_wim_knobs_command(&srv_msg_rp.payload.knobs);
		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_knobs_t), 0);
		break;
#if 0
	case SRV_SET_SUPPRESS_NAN:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_SUPPRESS_NAN : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
        weigh_send_set_supress_nan_command(&srv_msg_rp.payload.supress_nan);

		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, supress_nan), 0);
		break;
	case SRV_GET_SUPRESS_NAN_STATUS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_SUPRESS_NAN_STATUS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
        weigh_send_get_supress_nan_command(&srv_msg_rp.payload.supress_nan);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, supress_nan), 0);
		break;
#endif
	case SRV_SET_USER_OPTIONS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_USER_OPTIONS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_user_options_command(&srv_msg_rp.payload.user_options);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_user_options_t), 0);
		break;
	case SRV_GET_USER_OPTIONS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_USER_OPTIONS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_user_options_command(&srv_msg_rp.payload.user_options);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_user_options_t), 0);
		break;
	case SRV_SET_FILTER:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_FILTER : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_weigh_filter_command(&srv_msg_rp.payload.filter);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_filter_t), 0);
		break;
	case SRV_GET_FILTER:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_FILTER : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_weigh_filter_command(&srv_msg_rp.payload.filter);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_filter_t), 0);
		break;
	case SRV_SET_DATA_RATE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_DATA_RATE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_data_rate = srv_msg_rp.payload.data_rate;
		srv_msg_rp.payload.data_rate = srv_data_rate;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, data_rate), 0);
		break;
	case SRV_SET_COM_CHANNEL:

		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_COM_CHANNEL : %u\n", msg_id);
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "comm_channel : %u\n", srv_msg_rp.payload.com_channel);

		//printf("SRV_SET_COM_CHANNEL : %u\n", msg_id);
		//printf("comm_channel : %u\n", srv_msg_rp.payload.com_channel);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_comm_channel_command(&srv_msg_rp.payload.com_channel);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, com_channel), 0);
		break;
	case SRV_GET_COM_CHANNEL:

		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_COM_CHANNEL : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;
		weigh_send_get_comm_channel_command(&srv_msg_rp.payload.com_channel);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, com_channel), 0);
		break;
	case SRV_SET_FACTORY_DEFAULT:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_FACTORY_DEFAULT : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_factory_default = srv_msg_rp.payload.factory_default;
		srv_msg_rp.payload.factory_default = srv_factory_default;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, factory_default), 0);
		break;
	case SRV_SET_WIM_MODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_WIM_MODE : %u\n", msg_id);
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "size : %lu\n", size);
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "wim_mode : %u\n", srv_msg_rp.payload.wim_mode);

		srv_msg_rp.header.id = msg_id;

		weigh_send_set_wim_mode_command(&srv_msg_rp.payload.wim_mode);

		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, wim_mode), 0);
		break;
	case SRV_GET_WIM_MODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_WIM_MODE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.wim_mode = weigh_wim_get_wim_mode();
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, wim_mode), 0);
		break;
	case SRV_SET_DEFAULT_WIM_MODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_DEFAULT_WIM_MODE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_default_wim_mode_command(&srv_msg_rp.payload.default_wim_mode);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, default_wim_mode), 0);
		break;
	case SRV_SET_BP_CFG:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_BP_CFG : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_bp_cfg_command(&srv_msg_rp.payload.bp_cfg);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, bp_cfg), 0);
		break;
	case SRV_GET_BP_CFG:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_BP_CFG : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_bp_cfg_command(&srv_msg_rp.payload.bp_cfg);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, bp_cfg), 0);
		break;
	case SRV_SET_VS_MODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_VS_MODE : %u\n", msg_id);
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "size : %lu\n", size);
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "vs_mode : %u\n", srv_msg_rp.payload.vs_mode);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_vs_mode_command(&srv_msg_rp.payload.vs_mode);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, vs_mode), 0);
		break;
	case SRV_GET_VS_MODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_VS_MODE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.vs_mode = weigh_wim_get_vs_mode();
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, vs_mode), 0);
		break;
	case SRV_SET_SPEED_FACTOR:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_SPEED_FACTOR : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_speed_factor_command(&srv_msg_rp.payload.speed_factor);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, speed_factor), 0);
		break;
	case SRV_GET_SPEED_FACTOR:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_SPEED_FACTOR : %d\n", msg_id );

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_speed_factor_command(&srv_msg_rp.payload.speed_factor);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, speed_factor), 0);
		break;
	case SRV_SND_OTP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SND_OTP : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = 254;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t), 0);
		break;
	case SRV_RCV_OTP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RCV_OTP : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = 254;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t), 0);
		break;
	case SRV_GEN_OTP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GEN_OTP : %u\n", msg_id);

		//printf("SRV_GEN_OTP:%u otp: %u\n", msg_id, srv_msg_rp.payload.otp.otp);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = 0;

		weigh_send_get_otp_command(&srv_msg_rp.payload.otp.otp);
		//printf("%06u\n", srv_msg_rp.payload.otp.otp);

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_otp_t), 0);
		break;
	case SRV_VAL_OTP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_VAL_OTP : %u\n", msg_id);
		//uint32_t tmp_otp;
		//printf("SRV_VAL_OTP:%d otp: %u\n", msg_id, srv_msg_rp.payload.otp.otp);

		srv_msg_rp.header.id = msg_id;

		tmp_otp = srv_msg_rp.payload.otp.otp;
		weigh_send_val_otp_command(&tmp_otp);

		srv_msg_rp.header.status =(uint8_t) tmp_otp;
		//printf("OTP STATUS0:%u\n", tmp_otp);
		//printf("OTP STATUS1:%u\n", srv_msg_rp.header.status);

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_otp_t), 0);
		break;
	case SRV_SET_CAL_TABLE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_CAL_TABLE : %u\n", msg_id);

		weigh_send_set_cal_table_command(&srv_msg_rp.payload.calibration_data);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp , sizeof(srv_msg_rp_header_t) + sizeof(srv_calibration_data_t), 0);
		//printf("\nSRV_SET_CAL_TABLE : %d\n", msg_id);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_CAL_TABLE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_CAL_TABLE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		//srv_get_calibration_data(&srv_msg_rp.payload.calibration_data);
		weigh_send_get_cal_table_command(&srv_msg_rp.payload.calibration_data);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp , sizeof(srv_msg_rp_header_t) + sizeof(srv_calibration_data_t), 0);
		//printf("\nSRV_GET_CAL_TABLE : %d\n", msg_id);
#if WIM_SERV_TRACE_MSG_RP_DATA
#endif
		break;
	case SRV_LIVE_DATA_REQUIRED:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_LIVE_DATA_REQUIRED : %u\n", msg_id);
		//printf("srv_live_data_flag : %d\n", srv_msg_rp.payload.live_data_flag);

        srv_msg_rp.header.id = msg_id;
        srv_live_data_flag = srv_msg_rp.payload.live_data_flag;
        srv_msg_rp.payload.live_data_flag = srv_live_data_flag;
        srv_msg_rp.header.status = msg_status;
        zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, live_data_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	//case SRV_RECEIVE_LIVE_DATA:
	case SRV_SET_ZERO_SETTING_RANGES:

		/* TODO */
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_ZERO_SETTING_RANGES: %u\n", msg_id);

		weigh_send_set_zero_setting_ranges_command(&srv_msg_rp.payload.zero_setting_ranges);

		srv_msg_rp.header.id = msg_id;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_zero_setting_ranges_t), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_ADC_CODE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_QUERY_INST_VALUE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.adc_code = adc_m4_sample_raw;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, adc_code), 0);
		break;
	case SRV_SET_EN_ZERO_AT_POWER_UP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_EN_ZERO_AT_POWER_UP : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_ini_zero_flag_command(&srv_msg_rp.payload.en_zero_at_power_up_flag);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, en_zero_at_power_up_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_EN_ZERO_AT_POWER_UP:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_EN_ZERO_AT_POWER_UP : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_ini_zero_flag_command(&srv_msg_rp.payload.en_zero_at_power_up_flag);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, en_zero_at_power_up_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_EN_AUTO_ZERO:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_EN_AUTO_ZERO : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_track_zero_flag_command(&srv_msg_rp.payload.auto_zero_flag);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, auto_zero_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_AUTO_ZERO_STATUS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_AUTO_ZERO_STATUS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_track_zero_flag_command(&srv_msg_rp.payload.auto_zero_flag);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, auto_zero_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_AUTO_ZERO_VALUE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_AUTO_ZERO_VALUE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.auto_zero_value = weigh_get_track_zero();
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, auto_zero_value), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_SET_ZERO:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_ZERO : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_zero_command(&srv_msg_rp.payload.zero_value);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, zero_value), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	//case SRV_GET_ZERO_VALUE:
	case SRV_GET_INI_ZERO_VALUE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_INI_ZERO_VALUE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.ini_zero_value = weigh_get_ini_zero();
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, ini_zero_value), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_CANCEL_ZERO:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_CANCEL_ZERO : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_cancel_zero_command(&srv_msg_rp.payload.cancel_zero_value);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, cancel_zero_value), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_AXLE_RECORD_REQUIRED:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_AXLE_RECORD_REQUIRED : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_axle_record_flag = srv_msg_rp.payload.axle_record_flag;
		srv_msg_rp.payload.axle_record_flag = srv_axle_record_flag;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, axle_record_flag), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	//case SRV_RECEIVE_AXLE_RECORD:
	case SRV_GET_ZERO_SETTING_RANGES:
		/* TODO */
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_ZERO_SETTING_RANGES: %u\n", msg_id);

		weigh_send_get_zero_setting_ranges_command(&srv_msg_rp.payload.zero_setting_ranges);
		//cfg_active_weigh_field_params_print(&srv_msg_rp.payload.zero_setting_ranges);
		srv_msg_rp.header.id = msg_id;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_zero_setting_ranges_t), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_RECEIVE_AXLE_DATA:
		/* TODO */
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RECEIVE_AXLE_DATA : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;

		if(axle_queue_isempty(&axle_queue)) {
			memset(&srv_msg_rp.payload.axle.record, 0,  sizeof(srv_axle_record_t));
			srv_msg_rp.header.status = 0;
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_axle_record_t), 0);
			break;
		}else {
			axle_queue_dequeue(&axle_queue, &srv_msg_rp.payload.axle);
			srv_msg_rp.header.status = 1;
			APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "#%-4u\t%-8.6f\t%-8.6f\t%-8.6f\t-%u\n", srv_msg_rp.payload.axle.record.no,
					srv_msg_rp.payload.axle.record.weight,srv_msg_rp.payload.axle.record.speed, srv_msg_rp.payload.axle_record.distance, srv_msg_rp.payload.axle.data_len);
		}

		if(srv_axle_record_flag) {
			//zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_axle_record_t) + sizeof(srv_axle_record_t), 0);
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_axle_t) , 0);
		}else {
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_axle_record_t), 0);
		}
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_HW_STATUS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_HW_STATUS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		memcpy(&srv_msg_rp.payload.hw_status , &srv_hw_status, sizeof(srv_hw_status_t));
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_hw_status_t), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_RECEIVE_CPU_LOAD:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RECEIVE_CPU_LOAD : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.cpu_load = system_get_1_min_cpu_load_avg();
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, cpu_load), 0);
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_SET_DATA_LOGGING_FUNCTION:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_DATA_LOGGING_FUNCTION : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
        weigh_send_set_log_function_command(&srv_msg_rp.payload.data_logging_function_flag);

		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, data_logging_function_flag), 0);
		break;
	case SRV_GET_DATA_LOGGING_FUNCTION:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_DATA_LOGGING_FUNCTION : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
        weigh_send_get_log_function_command(&srv_msg_rp.payload.data_logging_function_flag);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, data_logging_function_flag), 0);
		break;
	case SRV_RECEIVE_SD_STATUS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RECEIVE_SD_STATUS : %u\n", msg_id);
		//printf("SRV_RECEIVE_SD_STATUS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_log_sd_status_command(&srv_msg_rp.payload.sd_status);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_sd_status_t), 0);
		break;
	case SRV_SET_PLAZA_UPDATE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_PLAZA_UPDATE : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;

		//printf("en_dis %d\n", srv_msg_rp.payload.srv_plaza_update.en_dis);
		//printf("Pident %d\n", srv_msg_rp.payload.srv_plaza_update.pident);

		if(srv_msg_rp.payload.srv_plaza_update.en_dis) {
			if(srv_plaza_update.en_dis) {
			}else {
				srv_plaza_update.en_dis = 1;
				lt_timer_start(&srv_timer_plaza_obj);
			}
			srv_plaza_update.pident = srv_msg_rp.payload.srv_plaza_update.pident;
		}else {
			if(srv_plaza_update.en_dis) {
				srv_plaza_update.en_dis = 0;
				lt_timer_stop(&srv_timer_plaza_obj);
			}else {

			}
			srv_plaza_update.pident = 0;
		}
		srv_msg_rp.payload.srv_plaza_update = srv_plaza_update;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, srv_plaza_update), 0);
		break;
	case SRV_GET_PLAZA_UPDATE:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_PLAZA_UPDATE : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.srv_plaza_update = srv_plaza_update;
		srv_msg_rp.header.status = msg_status;
		//printf("en_dis %u\n", srv_plaza_update.en_dis);
		//printf("Pident %u\n", srv_plaza_update.pident);
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, srv_plaza_update), 0);
		break;
	case SRV_RECEIVE_DATA_LOGGING_STATUS:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RECEIVE_DATA_LOGGING_STATUS : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.data_logging_status_flag = srv_data_logging_status_flag;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, data_logging_status_flag), 0);
		break;
	case SRV_SET_TIME:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_SET_TIME : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_set_time_command(&srv_msg_rp.payload.time);
		srv_msg_rp.header.status = msg_status;

		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, time), 0);
		break;
	case SRV_RECEIVE_CURRENT_TIME:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_RECEIVE_CURRENT_TIME : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		weigh_send_get_time_command(&srv_msg_rp.payload.time);
		//srv_msg_rp.payload.time = srv_time = time(NULL);
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + ALSDK_MEMBER_SIZE(srv_msg_req_payload_t, time), 0);
		break;
#if 0
	case SRV_GET_DATA_UPDATES:
		//APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_DATA_UPDATES : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;
        srv_msg_rp.payload.data_update.inst_value = weigh_get_inst_value();
        srv_msg_rp.payload.data_update.auto_zero_value = weigh_get_track_zero();
        srv_msg_rp.payload.data_update.zero_value =  weigh_get_zero();

		memcpy(&srv_msg_rp.payload.data_update.hw_status , &srv_hw_status, sizeof(srv_hw_status_t));

		srv_msg_rp.payload.data_update.hw_status.snrs_sts1 = wim_get_s1_status();
		srv_msg_rp.payload.data_update.hw_status.snrs_sts2 = wim_get_s2_status();


		if(axle_queue_isempty(&axle_queue)) {
			memset(&srv_msg_rp.payload.data_update.axle.record, 0,  sizeof(srv_axle_record_t));
			srv_msg_rp.header.status = 0;
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t), 0);
			break;
		}else {
			axle_queue_dequeue(&axle_queue, &srv_msg_rp.payload.data_update.axle);
			srv_msg_rp.header.status = 1;
			//APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "#%-4u\t%-8.6f\t%-8.6f\t%-8.6f\t%-u\n", srv_msg_rp.payload.data_update.axle.record.no,
			//		srv_msg_rp.payload.data_update.axle.record.weight,srv_msg_rp.payload.data_update.axle.record.speed,
			//		srv_msg_rp.payload.data_update.axle.record.distance, srv_msg_rp.payload.data_update.axle.data_len);
		}

		if(!srv_axle_record_flag) {
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t), 0);
		}else {
			//zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) + sizeof(srv_axle_data_t), 0);
			zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + 12 + sizeof(srv_hw_status_t) + sizeof(srv_axle_record_t) +
					(srv_msg_rp.payload.data_update.axle.data_len* sizeof(float)), 0);
		}
#if WIM_SRV_TRACE_MSG_RP_OUT_DATA
#endif
		break;
	case SRV_GET_SETTINGS_UPDATES:
		//APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SRV_GET_SETTINGS_UPDATES : %u\n", msg_id);
		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.payload.settings_update.wim_mode = weigh_wim_get_wim_mode();
		srv_msg_rp.payload.settings_update.vs_mode = weigh_wim_get_vs_mode();
		srv_msg_rp.payload.settings_update.cal_lock = (uint8_t) io_get_weigh_cal_lock_line_state();
		srv_msg_rp.payload.settings_update.wim_dir = weigh_wim_get_wim_dir();
		srv_msg_rp.payload.settings_update.cpu_load = system_get_1_min_cpu_load_avg();
		srv_msg_rp.payload.settings_update.time_stamp = (int64_t) time(NULL);
		srv_msg_rp.payload.settings_update.rt_flag = false;
		srv_msg_rp.header.status = msg_status;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp , sizeof(srv_msg_rp_header_t) + sizeof(srv_settings_update_t), 0);
#if WIM_SERV_TRACE_MSG_RP_OUT_DATA
	//	printf("inst_value : %lu \n", srv_msg_rp.payload.mp);
#endif
		break;
#endif
	default:
		APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "DEFAULT : %u\n", msg_id);

		srv_msg_rp.header.id = msg_id;
		srv_msg_rp.header.status = 255;
		zmq_send(server_td_zmq_rp_responder, (void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t), 0);
		break;
	}

	return 0;
}

int server_zmq_adc_data_ps_serve_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	return 0;
}

int server_zmq_adc_data_ps_serve_publish(void *buf, size_t len, int flags)
{
	zmq_send(server_td_zmq_adc_data_ps_socket, buf, len, flags);
	return 0;
}

int server_zmq_plaza_data_ps_serve_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	return 0;
}

int server_zmq_plaza_ps_serve_publish(void *buf, size_t len, int flags)
{
	zmq_send(server_td_zmq_plaza_data_ps_socket, buf, len, flags);
	return 0;
}

#define WIM_SERV_TRACE_THREAD		0
#define WIM_SERV_TRACE_THREA_EVENTS	0
#define WIM_SERV_TRACE_THREAD_DATA	0
#define WIM_SERV_TRACE_THREAD_FUNC	0



static int server_thread_arg;
static void* server_thread_func(void *args)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	alsdk_pthread_setname_np(alsdk_pthread_self(), "SERV");
	setpriority(PRIO_PROCESS, 0, -12); //0

	int n;
    struct epoll_event events[ADC_TD_EPOLL_MAX_EVENTS];

	int rc;
	//int event_count;
	//int zevents;
	//size_t zevents_len;
	//
	srv_msg_t msg;
    unsigned int msg_prio;
    ssize_t msg_len;

	(void) args; /* Suppress -Wunused-parameter warning. */

	/* wait for wim init barrier */
	weigh_init_barrier_wait(__func__);


	/* wait for wim cfg barrier */
	weigh_cfg_barrier_wait(__func__);
	lt_timer_obj_init(&srv_timer_plaza_obj, srv_timer_plaza_handler , LT_TIMER_PERIODIC, NULL, 1000);

	/* wait for wim cmd barrier */
	weigh_cmd_barrier_wait(__func__);
#if 0
	srv_plaza_update.en_dis = 1;
	srv_plaza_update.pident = 1;
	lt_timer_start(&srv_timer_plaza_obj);
#endif
	for(;;) {
		//rc = zmq_poll (server_items, 3, -1);
		rc = zmq_poll(server_items, 2, -1);
		assert (rc >= 0);

		if(rc > 0) {
			// event occurred
			//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : event occurred\n");

		    if(server_items[0].revents != 0) {
				//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : event occurred on queue\n");
				if(server_items[0].revents & ZMQ_POLLIN) {
					//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : Reading file descriptor %d : queue\n", server_items[0].fd);
					while((msg_len = mq_receive(server_td_mqd, (char*) &msg, sizeof(srv_msg_t), &msg_prio)) > 0) {
						switch(msg.header.type) {
						case SRV_MSG_TYPE_EVENT:
							switch(msg.header.id) {
							case SRV_EVENT_INIT:
								APP_DEBUGF(SERVER_EVENT_DEBUG|APP_DBG_TRACE, "SERVER_EVENT_INIT\n");
							break;
							case SRV_EVENT_PLAZA:
								APP_DEBUGF(SERVER_EVENT_DEBUG|APP_DBG_TRACE, "SERVER_EVENT_PLAZA\n");
								//printf("SERVER_EVENT_PLAZA\n");
								srv_msg_rp.header.id = 64;
								srv_msg_rp.payload.srv_plaza_data.pident = srv_plaza_update.pident;
								/*SRNO*/
								srv_msg_rp.payload.srv_plaza_data.srno.b_srno = license_device_info.board_ser_no;
								//srv_msg_rp.payload.srv_plaza_data.srno.m_srno = license_device_info.module_ser_no;
								srv_msg_rp.payload.srv_plaza_data.srno.m_srno = m_srno;

								/*SW*/
								/*WIM*/
								srv_msg_rp.payload.srv_plaza_data.wim.wim_mode = weigh_wim_get_wim_mode();
								srv_msg_rp.payload.srv_plaza_data.wim.vs_mode = weigh_wim_get_vs_mode();
								srv_msg_rp.payload.srv_plaza_data.wim.comm_channel = cfg_active_wim_core.comm_channel;
							    srv_msg_rp.payload.srv_plaza_data.wim.baud_rate = cfg_active_wim_core.baud_rate;
							    srv_msg_rp.payload.srv_plaza_data.wim.sd_log = cfg_active_wim_core.sd_log;
							    srv_msg_rp.payload.srv_plaza_data.wim.sline = cfg_active_wim_core.sline;
							    srv_msg_rp.payload.srv_plaza_data.wim.noack = cfg_active_wim_core.noack;

								srv_msg_rp.payload.srv_plaza_data.wim.tr_no = curr_vehicle_data.tr_no;
								srv_msg_rp.payload.srv_plaza_data.wim. no_axles= curr_vehicle_data.no_axles;
								srv_msg_rp.payload.srv_plaza_data.wim.weight = curr_vehicle_data.weight;
								srv_msg_rp.payload.srv_plaza_data.wim.exit_status = curr_vehicle_data.exit_status;

								/*WEIGH*/
								srv_msg_rp.payload.srv_plaza_data.weigh.inst_value = weigh_get_inst_value();
								srv_msg_rp.payload.srv_plaza_data.weigh.hs_spread = axle_hs_spread;

								/*DIAG*/
								srv_msg_rp.payload.srv_plaza_data.diag.snrs_sts1 = wim_get_s1_status();
								srv_msg_rp.payload.srv_plaza_data.diag.snrs_sts2 = wim_get_s2_status();
								srv_msg_rp.payload.srv_plaza_data.diag.sd_log = cfg_active_wim_core.sd_log;
								/*ERR*/
								srv_msg_rp.payload.srv_plaza_data.err.dev.bytes = dev_err.bytes;
								srv_msg_rp.payload.srv_plaza_data.err.sys.bytes = sys_err.bytes;
								srv_msg_rp.payload.srv_plaza_data.err.m4.bytes = m4_err.bytes;
								srv_msg_rp.payload.srv_plaza_data.err.weigh.bytes = weigh_err.bytes;
								srv_msg_rp.payload.srv_plaza_data.err.wim.bytes = wim_err.bytes;

								/*IDENT*/
								snprintf(&srv_msg_rp.payload.srv_plaza_data.ident.fw_ver, sizeof(srv_msg_rp.payload.srv_plaza_data.ident.fw_ver), "%s", weigh_fw.info.version);
								snprintf(&srv_msg_rp.payload.srv_plaza_data.ident.loc_id, sizeof(srv_msg_rp.payload.srv_plaza_data.ident.loc_id), "%s", cfg_active_machine.location_id);
								snprintf(&srv_msg_rp.payload.srv_plaza_data.ident.lane_id, sizeof(srv_msg_rp.payload.srv_plaza_data.ident.lane_id), "%s", cfg_active_machine.lane_id);
								snprintf(&srv_msg_rp.payload.srv_plaza_data.ident.buf, sizeof(srv_msg_rp.payload.srv_plaza_data.ident.buf), "");

								srv_msg_rp.header.status = 0 ;
								zmq_send(server_td_zmq_plaza_data_ps_socket, (void*)&srv_msg_rp , sizeof(srv_msg_rp_header_t) + sizeof(srv_plaza_data_t), 0);
								//server_zmq_plaza_ps_serve_publish((void*)&srv_msg_rp, sizeof(srv_msg_rp_header_t) + sizeof(srv_plaza_data_t), 0);
								//server_zmq_plaza_ps_serve_publish("Hello World!", 12, 0);
								//printf("%u - %d - %lf\n", adc_m4_sample_raw, adc_m4_sample, adc_m4_ain);
								//printf("%f- %f\n",axle_hs_spread_min, axle_hs_spread_max);
								//printf("%f- %f\n",srv_msg_rp.payload.srv_plaza_data.weigh.inst_value, axle_hs_spread);
							break;
							case SRV_EVENT_DUMMY:
								APP_DEBUGF(SERVER_EVENT_DEBUG|APP_DBG_TRACE, "SERVER_EVENT_DUMMY\n");
								break;
							case SRV_EVENT_MAX:
								APP_DEBUGF(SERVER_EVENT_DEBUG|APP_DBG_TRACE, "SERVER_EVENT_MAX\n");
								break;
							default:
								APP_DEBUGF(SERVER_EVENT_DEBUG|APP_DBG_TRACE, "SERVER_EVENT_ILL\n");
								break;
							}
						case SRV_MSG_TYPE_CMD:
							switch(msg.header.id) {
							case SRV_CMD_START:
								APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SERVER_CMD_START\n");
								break;
							case SRV_CMD_STOP:
								APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SERVER_CMD_STOP\n");
								break;
							case SRV_CMD_RESTART:
								APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SERVER_CMD_RESTART\n");
								break;
							case SRV_CMD_MAX:
								APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SERVER_CMD_MAX\n");
								break;
							default:
								APP_DEBUGF(SERVER_CMD_DEBUG|APP_DBG_TRACE, "SERVER_CMD_ILL\n");
								break;
							}
						}
					}
				}
				if(server_items[0].revents & ZMQ_POLLOUT) {
					//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : Writing file descriptor %d : queue\n", server_items[0].fd);
				}
			}

			if(server_items[1].revents != 0) {
				//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : event occurred on zmqrp\n");
				if(server_items[1].revents & ZMQ_POLLIN) {
					//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : RW  file descriptor %d : zmq_rp\n", server_items[1].fd);
					server_zmq_tcp_rp_serve();
				}
				if(server_items[1].revents & ZMQ_POLLOUT) {
					//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : RW  file descriptor %d : zmq_rp\n", server_items[1].fd);
					//APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : Writing file descriptor %d : zmqrp\n", server_items[1].fd);
				}

			}

			if(server_items[2].revents != 0) {
				APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : event occurred on stdin\n");
				if(server_items[2].revents & ZMQ_POLLIN) {
					#define READ_SIZE 10
					ssize_t bytes_read;
					char read_buffer[READ_SIZE + 1];
					printf("srv : Reading file descriptor %d : stdin\n", server_items[1].fd);
					bytes_read = read(events[n].data.fd, read_buffer, READ_SIZE);
					printf("srv : %zd bytes read.\n", bytes_read);
					read_buffer[bytes_read] = '\0';
					printf("srv : Read %s\n", read_buffer);
				}
				if(server_items[2].revents & ZMQ_POLLOUT) {
					APP_DEBUGF(SERVER_MQ_DEBUG|APP_DBG_TRACE, "srv : Writing file descriptor %d : stdin\n", server_items[1].fd);
				}
			}

		}else if(rc == 0) {
			// timeout expired, do something
			printf("srv : timeout expired , do something\n");
		}else {
			// error occurred, handle it
			printf("srv : error occurred, handle it\n");
		}
		//alsdk_msleep(10);
	}
	pthread_exit((void*)0);
}

int server_network_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	/* create socket */
	if((server_td_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "Socket creation error\n");
		return -1;
	}

	return 0;
}

int server_network_connect(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	//struct sockaddr_in address;
	struct sockaddr_in serv_addr;

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = (sa_family_t)AF_INET;
	//htons converts from host byte order to network byte order
	serv_addr.sin_port = htons(SERVER_TD_PORT);
	// Convert IPv4 and IPv6 addresses from text to binary form
	//if(inet_pton(AF_INET, "192.168.0.90", &serv_addr.sin_addr)<=0) {
	if(inet_pton(AF_INET, SERVER_TD_SRV_IP, &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
	/*Connects to server on the given PORT*/
	printf("connectin to server  %s port %u\n", SERVER_TD_SRV_IP, SERVER_TD_SRV_PORT);
	if(connect(server_td_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	return 0;
}


int server_zmq_rp_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rc;
	char server_port_str[32];
    //  Socket to talk to clients
    server_td_zmq_rp_context = zmq_ctx_new();
    server_td_zmq_rp_responder = zmq_socket(server_td_zmq_rp_context, ZMQ_REP);
    //rc = zmq_bind(server_td_zmq_rp_responder, "tcp://*:5555");
	snprintf(server_port_str, 32,"tcp://*:%u", project_info.cust.ui_port);
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "server port [%s]\n", server_port_str);

    rc = zmq_bind(server_td_zmq_rp_responder, server_port_str);
    assert (rc == 0);

    size_t size = sizeof(server_td_zmq_rp_sock_fd);
    rc = zmq_getsockopt(server_td_zmq_rp_responder, ZMQ_FD, &server_td_zmq_rp_sock_fd, &size);
    assert (rc == 0);

	server_zmq_tcp_rp_serve_init();

	return 0;
}

int server_zmq_adc_data_ps_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rc;

    //  Socket to talk to clients
    server_td_zmq_adc_data_ps_context = zmq_ctx_new();
    server_td_zmq_adc_data_ps_socket = zmq_socket(server_td_zmq_adc_data_ps_context, ZMQ_PUB);
    rc = zmq_bind(server_td_zmq_adc_data_ps_socket, "tcp://*:5556");
    assert (rc == 0);

    size_t size = sizeof(server_td_zmq_adc_data_ps_sock_fd);
    rc = zmq_getsockopt(server_td_zmq_adc_data_ps_socket, ZMQ_FD, &server_td_zmq_adc_data_ps_sock_fd, &size);
    assert (rc == 0);

	server_zmq_adc_data_ps_serve_init();

	return 0;
}

int server_zmq_plaza_data_ps_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rc;
    //  Socket to talk to clients
    server_td_zmq_plaza_data_ps_context = zmq_ctx_new();
    server_td_zmq_plaza_data_ps_socket = zmq_socket(server_td_zmq_plaza_data_ps_context, ZMQ_PUB);
    rc = zmq_bind(server_td_zmq_plaza_data_ps_socket, "tcp://*:5557");
    assert (rc == 0);

    size_t size = sizeof(server_td_zmq_plaza_data_ps_sock_fd);
    rc = zmq_getsockopt(server_td_zmq_plaza_data_ps_socket, ZMQ_FD, &server_td_zmq_plaza_data_ps_sock_fd, &size);
    assert (rc == 0);

	server_zmq_plaza_data_ps_serve_init();

	return 0;
}

int server_init(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	/* Initialize the queue attributes */
	struct mq_attr attr = SERVER_TD_QUEUE_ATTR_INITIALIZER;

    mq_unlink(SERVER_TD_QUEUE_NAME);
	/* Create the message queue. The queue reader is NONBLOCK. */
	server_td_mqd = alsdk_mq_open(SERVER_TD_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, SERVER_TD_QUEUE_PERMS, &attr);

	struct epoll_event event = SERVER_TD_EPOLL_EVENT_INITIALIZER;
#if 0
	/* Create epoll interface */
	//server_td_epfd = alsdk_epoll_create1(0);

	/* add epoll fds */
	//alsdk_epoll_add(server_td_epfd, server_td_mqd, EPOLLIN);
	//alsdk_epoll_add(server_td_epfd, 0, EPOLLIN);
#endif
	/* initialize server network */
	server_network_init();
	/* initialize server zmq rp network */
	server_zmq_rp_init();
#if 0
	//alsdk_epoll_add(server_td_epfd, server_td_zmq_rp_sock_fd, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP);
#endif

	/* first item refers to msgqueue */
	server_items[0].socket = NULL;
	server_items[0].fd = server_td_mqd;
	server_items[0].events = ZMQ_POLLIN;
	/* second item refers to ØMQ socket 'socket' */
	server_items[1].socket = server_td_zmq_rp_responder;
	server_items[1].fd = server_td_zmq_rp_sock_fd;
	server_items[1].events = ZMQ_POLLIN;
	/* first item refers to stdin */
	server_items[2].socket = NULL;
	server_items[2].fd = 0;
	server_items[2].events = ZMQ_POLLIN;

	/* init adc data publish */
	server_zmq_adc_data_ps_init();
#if 1
	/* init plaza data publish */
	server_zmq_plaza_data_ps_init();
#endif

	/* cpu set 0 */
	cpu_set_t cpuset_0;
    CPU_ZERO(&cpuset_0);
    CPU_SET(0, &cpuset_0);

	/* Create the thread */
	server_td_tif.args = (void*) &server_thread_arg;
    alsdk_pthread_create(&server_td_tif.thread_id, NULL, server_thread_func, &server_td_tif);
	alsdk_pthread_setaffinity_np(server_td_tif.thread_id, sizeof(cpu_set_t), &cpuset_0);

	return 0;
}

int server_join(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	alsdk_pthread_join(server_td_tif.thread_id, NULL);

	return 0;
}

int server_cancel(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	alsdk_pthread_cancel(server_td_tif.thread_id);

	return 0;
}

int server_exit(void)
{
	APP_DEBUGF(SERVER_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	alsdk_mq_close(server_td_mqd);
	alsdk_mq_unlink(SERVER_TD_QUEUE_NAME);

	/* adc data publish */
	zmq_close(server_td_zmq_adc_data_ps_socket);
    zmq_ctx_destroy(server_td_zmq_adc_data_ps_context);

	/* adc data publish */
	zmq_close(server_td_zmq_plaza_data_ps_socket);
    zmq_ctx_destroy(server_td_zmq_plaza_data_ps_context);

	/* server zmq rp network */
	zmq_close(server_td_zmq_rp_responder);
    zmq_ctx_destroy(server_td_zmq_rp_context);

	return 0;
}
