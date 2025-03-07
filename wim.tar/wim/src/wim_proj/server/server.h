#ifndef SERVER_H__
#define SERVER_H__

#include <pthread.h>
#include <semaphore.h>   /* Posix semaphores */
#include <mqueue.h>
#include <alsdk_pthread.h>

#include "project.h"
#include "cfg.h"
#include "adc.h"
#include "axle.h"

#define SERVER_TD_QUEUE_NAME  "/server-td-queue" /* Queue name. */
#define SERVER_TD_QUEUE_PERMS   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
									/* default permissions for new files */
#define SERVER_TD_QUEUE_MAXMSG  10 /* Maximum number of messages. */
#define SERVER_TD_QUEUE_MSGSIZE 16 /* Length of message. */

#define SERVER_TD_QUEUE_EVENT_PRIO ((int)(3))
#define SERVER_TD_QUEUE_CMD_PRIO ((int)(0))


#define SERVER_TD_EPOLL_MAX_EVENTS 16

#define SERVER_TD_SRV_IP "192.168.0.30"
#define SERVER_TD_SRV_PORT 8000
#define SERVER_TD_PORT 8000


#if (WIM_CUST == WIM_CUST_ARETE)
#warning WIM_CUST_ARETE
#define SERVER_RP_PORT "tcp://*:55566"
#elif (WIM_CUST == WIM_CUST_ASPEKT)
#warning WIM_CUST_ASPEKT
#define SERVER_RP_PORT "tcp://*:55599"
#elif (WIM_CUST == WIM_CUST_ESSAE)
#warning WIM_CUST_ESSAE
#define SERVER_RP_PORT "tcp://*:5555"
#elif (WIM_CUST == WIM_CUST_LANDT)
#warning WIM_CUST_LANDT
#define SERVER_RP_PORT "tcp://*:5555"
#elif (WIM_CUST == WIM_CUST_MERIT)
#warning WIM_CUST_MERIT
#define SERVER_RP_PORT "tcp://*:55544"
#elif (WIM_CUST == WIM_CUST_RAJDEEP)
#warning WIM_CUST_RAJDEEP
#define SERVER_RP_PORT "tcp://*:55588"
#elif (WIM_CUST == WIM_CUST_VAAAN)
#warning WIM_CUST_VAAAN
#define SERVER_RP_PORT "tcp://*:55577"
#elif (WIM_CUST == WIM_CUST_TULAMAN)
#warning WIM_CUST_TULAMAN
#define SERVER_RP_PORT "tcp://*:55533"
#else
#error UNKNOW WIM_CUST $WIM_CUST
#endif




/* Logging*/
#define WIM_SRV_TRACE_MSG_RP		0
#define WIM_SRV_TRACE_MSG_RP_ID		0
#define WIM_SRV_TRACE_MSG_RP_IN_DATA	0
#define WIM_SRV_TRACE_MSG_RP_OUT_DATA	0
#define WIM_SRV_TRACE_MSG_RP_FUNC	0
#define WIM_SRV_TRACE_MSG_RP_FUNC_DATA	0

/* Server message IDs */
#define SRV_RT_REBOOT						0
#define SRV_RT_RESTART						1
#define SRV_GET_FW_INFO					    2
#define SRV_INSTALL_FW						3
#define SRV_SET_ASSET_ID					4
#define SRV_GET_ASSET_ID					5
#define SRV_SET_LOAD_CELL_PARAMETERS		6
#define SRV_GET_LOAD_CELL_PARAMETERS		7

#define SRV_SET_NETWORK_PARAMS				8
#define SRV_GET_NETWORK_PARAMS				9

#define SRV_SET_CONFIGURATION_DATA			10
#define SRV_GET_CONFIGURATION_DATA			11
#define SRV_SET_KNOBS						12
#define SRV_GET_KNOBS						13
#if 0
#define SRV_SET_SUPPRESS_NAN				14
#define SRV_GET_SUPRESS_NAN_STATUS			15
#endif
#define SRV_SET_USER_OPTIONS				14
#define SRV_GET_USER_OPTIONS	 			15
#define SRV_SET_FILTER						16
#define SRV_GET_FILTER						17
#define SRV_SET_DATA_RATE					18
#define SRV_SET_COM_CHANNEL					19
#define SRV_GET_COM_CHANNEL					20
#define SRV_SET_FACTORY_DEFAULT				21
#define SRV_SET_WIM_MODE					22
#define SRV_GET_WIM_MODE					23
#define SRV_SET_DEFAULT_WIM_MODE			24
#define SRV_SET_BP_CFG						25
#define SRV_GET_BP_CFG						26
#define SRV_SET_VS_MODE						27
#define SRV_GET_VS_MODE						28

#define SRV_SET_SPEED_FACTOR				29
#define SRV_GET_SPEED_FACTOR				30

#define SRV_SND_OTP							31
#define SRV_RCV_OTP							32

#define SRV_GEN_OTP							33
#define SRV_VAL_OTP							34


#define SRV_SET_CAL_TABLE					35
#define SRV_GET_CAL_TABLE					36
#define SRV_LIVE_DATA_REQUIRED				37
//#define SRV_RECEIVE_LIVE_DATA				38
#define SRV_SET_ZERO_SETTING_RANGES			38
#define SRV_GET_ADC_CODE	 				39
#define SRV_SET_EN_ZERO_AT_POWER_UP			40
#define SRV_GET_EN_ZERO_AT_POWER_UP			41
#define SRV_EN_AUTO_ZERO					42
#define SRV_GET_AUTO_ZERO_STATUS			43
#define SRV_GET_AUTO_ZERO_VALUE				44
#define SRV_SET_ZERO						45
//#define SRV_GET_ZERO_VALUE				46
#define SRV_GET_INI_ZERO_VALUE				46
#define SRV_CANCEL_ZERO						47
#define SRV_AXLE_RECORD_REQUIRED			48
//#define SRV_RECEIVE_AXLE_RECORD			49
#define	SRV_GET_ZERO_SETTING_RANGES			49
#define SRV_RECEIVE_AXLE_DATA				50
#define SRV_GET_HW_STATUS					51
#define SRV_RECEIVE_CPU_LOAD				52
#define SRV_SET_DATA_LOGGING_FUNCTION		53
#define SRV_GET_DATA_LOGGING_FUNCTION		54
#define SRV_RECEIVE_SD_STATUS				55
#define SRV_SET_PLAZA_UPDATE				56
#define SRV_GET_PLAZA_UPDATE	 			57
#define SRV_RECEIVE_DATA_LOGGING_STATUS		58
#define SRV_SET_TIME						59
#define SRV_RECEIVE_CURRENT_TIME			60
#define SRV_GET_DATA_UPDATES				61
#define SRV_GET_SETTINGS_UPDATES			62


extern alsdk_thread_info_t tif_server_td;
extern mqd_t mqd_server_td;

int server_init(void);
int server_cancel(void);
int server_join(void);
int server_exit(void);

int server_zmq_adc_data_ps_serve_publish(void *buf, size_t len, int flags);
int server_zmq_adc_axle_data_ps_serve_publish(void *buf, size_t len, int flags);




typedef enum srv_cmd_e {
	SRV_CMD_START = 0,
	SRV_CMD_STOP,
	SRV_CMD_RESTART,
	SRV_CMD_MAX
}srv_cmd_t;
typedef enum srv_event_e {
	SRV_EVENT_INIT = 0,
	SRV_EVENT_PLAZA ,
	SRV_EVENT_DUMMY,
	SRV_EVENT_MAX
}srv_event_t;
typedef enum srv_msg_type_e {
	SRV_MSG_TYPE_EVENT = 0,
	SRV_MSG_TYPE_CMD ,
	SRV_MSG_TYPE_MAX
}srv_msg_type_t;

typedef struct __attribute__((packed)) srv_cmd_dummy_params_s {
     int32_t data1;
     int32_t data2;
     int32_t data3;
}srv_cmd_dummy_params_t;


typedef union __attribute__((packed)) srv_msg_payload_u {
	srv_cmd_dummy_params_t dummy;
}srv_msg_payload_t;

typedef struct __attribute__((packed)) srv_msg_header_s {
     int16_t type;
     int16_t id;
}srv_msg_header_t;

typedef struct __attribute__((packed)) srv_msg_s {
     srv_msg_header_t header;
     srv_msg_payload_t payload;
}srv_msg_t;

int srv_raise_init_event(void);
int srv_raise_plaza_event(void);


/* Server message structures */
/*SRV_RT_REBOOT					0*/
/*SRV_RT_RESTART				1*/

/* SRV_GET_FW_INFO					2 */
typedef struct __attribute__((packed)) srv_fw_info_s {
	uint8_t  version[32];
	uint8_t  title[32];
	uint8_t  description[1024];
}srv_fw_info_t;
extern srv_fw_info_t srv_fw_info;
/* SRV_INSTALL_FW						3 */
typedef struct __attribute__((packed)) srv_install_fw_s {
	uint8_t name[32];
}srv_install_fw_t;
/* SRV_SET_ASSET_ID						4 */
typedef struct __attribute__((packed)) srv_location_info_s {
	uint8_t  location_id[32];
	uint8_t  lane_id[32];
}srv_location_info_t;
typedef struct __attribute__((packed)) srv_module_info_s {
	uint8_t  ser_no[32];
	uint8_t  mac[32];
	uint8_t  type;
	uint8_t  rfu0;
	uint8_t  rfu1;
	uint8_t  rfu2;
}srv_module_info_t;
typedef struct __attribute__((packed)) srv_board_info_s {
	uint32_t ser_no;
	uint32_t ver_no;
	uint32_t cust_id;
	uint32_t hw_fe;
	uint32_t hw_fe_pop;
	uint32_t hw_fe_ena;
	uint32_t rfu0;
	uint32_t rfu1;
	uint32_t rfu2;
	uint32_t rfu3;
	uint32_t rfu4;
	uint32_t rfu5;
	uint32_t rfu6;
	uint32_t rfu7;
	uint32_t rfu8;
}srv_board_info_t;
typedef struct __attribute__((packed)) srv_asset_info_s {
	srv_location_info_t location_info;
	uint8_t  asset_id[32];
	uint8_t  system_status[32];
	srv_module_info_t module_info;
	srv_board_info_t  board_info;
}srv_asset_info_t;
/* SRV_GET_ASSET_ID						5 */
srv_asset_info_t svr_asset_info;
/* SRV_SET_LOAD_CELL_PARAMETERS			6 */
/* SRV_GET_LOAD_CELL_PARAMETERS			7 */
typedef struct __attribute__((packed)) srv_loadcell_params_s {
	float capacity;
	float sensitivity;
	float resolution;
}srv_loadcell_params_t;
extern srv_loadcell_params_t srv_loadcell_params;
/* SRV_SET_NETWORK_PARAMS				8*/
/* SRV_GET_NETWORK_PARAMS				9*/
typedef enum svr_ip_addr_req_mode_e {
	SVR_IP_ADDR_REQ_MODE_STATIC = 0,
	SVR_IP_ADDR_REQ_MODE_DHCP
}srv_ip_addr_req_mode_t;

typedef struct __attribute__((packed)) srv_network_params_s {
	uint8_t  ip_addr_req_mode;
	uint8_t  ip_addr[16];
	uint8_t  ip_addr_subnet_mask[16];
	uint8_t  ip_addr_gateway[16];
	uint8_t  ip_addr_dns_server[16];
}srv_network_params_t;

typedef struct __attribute__((packed)) srv_network_params_get_s {
	uint8_t  ip_eth0_mac[18];  /* mac address */
	uint8_t  ip_addr_mode;
	uint8_t  ip_addr[16];
	uint8_t  ip_addr_subnet_mask[16];
	uint8_t  ip_addr_gateway[16];
	uint8_t  ip_addr_dns_server[16];
}srv_network_params_get_t;

typedef enum svr_ip_addr_req_status_e {
	SVR_IP_ADDR_REQ_STATUS_INVALID = 0,
	SVR_IP_ADDR_REQ_STATUS_INVALID_SUBNET = 0,
	SVR_IP_ADDR_REQ_STATUS_INVALID_DNS,
	SVR_IP_ADDR_REQ_STATUS_INVALID_GATWEAY,
	SVR_IP_ADDR_REQ_STATUS_INVALID_IP
}srv_ip_addr_req_status_t;

typedef struct __attribute__((packed)) srv_wim_related_s {
	float speed_limit;
	float threshold;
	float scale_width;
}srv_wim_related_t;
/* SRV_SET_CONFIGURATION_DATA			10 */
/* SRV_GET_CONFIGURATION_DATA			11 */
typedef struct __attribute__((packed)) srv_wim_config_data_s {
	srv_wim_related_t wim_related;
	srv_loadcell_params_t loadcell_params;
	uint8_t baudrate;
}srv_wim_config_data_t;
/* SRV_SET_KNOBS						12 */
/* SRV_GET_KNOBS						13 */
typedef struct __attribute__((packed)) srv_knobs_s {
	float LWCT;
	float LWWT;
	uint8_t OC;
}srv_knobs_t;
#if 0
/* SRV_SET_SUPPRESS_NAN					14 */
/* SRV_GET_SUPRESS_NAN_STATUS			15 */
#endif
typedef struct __attribute__((packed)) srv_user_options_s {
	uint8_t  supress_nan;
	uint8_t  sline;
	uint8_t  noack;
	uint8_t  dwm;
}srv_user_options_t;
/* SRV_SET_USER_OPTIONS					14 */
/* SRV_GET_USER_OPTIONS					15 */
/* SRV_SET_FILTER    					16 */
/* SRV_GET_FILTER	    				17 */
typedef struct __attribute__((packed)) srv_filter_s {
	uint8_t enable;
	uint16_t f1_size;
	uint16_t f2_size;
}srv_filter_t;
/* SRV_SET_DATA_RATE					18 */
extern uint8_t srv_data_rate;
/* SRV_SET_COM_CHANNEL					19 */
/* SRV_GET_COM_CHANNEL					20 */
extern uint8_t srv_com_channel;
/* SRV_SET_FACTORY_DEFAULT				21 */
extern uint8_t srv_factory_default;
/* SRV_SET_WIM_MODE						22 */
/* SRV_GET_WIM_MODE						23 */
extern uint8_t srv_wim_mode;
/* SRV_SET_DEFAULT_WIM_MODE				24 */
extern uint8_t srv_default_wim_mode;
/* SRV_SET_WIM_DIR						25 */
/* SRV_GET_WIM_DIR						26 */
/* SRV_SET_VS_MODE						27 */
/* SRV_GET_VS_MODE						28 */
/* SRV_SET_SPEED_FACTOR					29 */
/* SRV_GET_SPEED_FACTOR					30 */
/* SRV_SND_OTP							31 */
/* SRV_RCV_OTP							32 */
typedef struct __attribute__((packed)) srv_otp_s {
	uint32_t otp;
}srv_otp_t;
/* SRV_GEN_OTP							33 */
/* SRV_VAL_OTP							34 */
/* SRV_SET_CAL_TABLE					35 */
/* SRV_GET_CAL_TABLE					36 */
#define WIM_DYNAMIC_GAIN_COEFF_INTERPOLATED_DATA_ROWS 41
#define WIM_DYNAMIC_GAIN_COEFF_INTERPOLATED_DATA_COLUMNS 2
typedef struct __attribute__((packed)) srv_dynamic_gain_coeff_s {
	float capacity;
	float dummy1;
	float dummy2;
	float interpolated_data[WIM_DYNAMIC_GAIN_COEFF_INTERPOLATED_DATA_ROWS] [WIM_DYNAMIC_GAIN_COEFF_INTERPOLATED_DATA_COLUMNS];
}srv_dynamic_gain_coeff_t;
#define WIM_CALIBRATION_DATA_MAX_ARR_CONDITIONS 32
#define WIM_CALIBRATION_DATA_MAX_DYNAMIC_COEFF  25
typedef struct __attribute__((packed)) srv_calibration_data_s {
	int64_t time_stamp;
	float static_gain_coeff;
	float system_offset;
	float dummy;
	srv_dynamic_gain_coeff_t dynamic_gain_coeff[WIM_CALIBRATION_DATA_MAX_DYNAMIC_COEFF];
	uint16_t counter;
	uint32_t dummy0;
	uint8_t conditions[WIM_CALIBRATION_DATA_MAX_ARR_CONDITIONS];
}srv_calibration_data_t;
extern srv_calibration_data_t srv_calibration_data;
/* SRV_LIVE_DATA_REQUIRED				37 */
extern uint8_t volatile srv_live_data_flag;
/* SRV_RECEIVE_LIVE_DATA				38 */
/* SRV_SET_ZERO_SETTING_RANGES			38 */
 typedef struct __attribute__((__packed__)) srv_zero_setting_ranges_s {
     float      zero_limit;
     float      track_zero_limit;
     float      ini_zero_limit;
 }srv_zero_setting_ranges_t;
 extern srv_zero_setting_ranges_t srv_zero_setting_ranges;

 #define WIM_ADC_S_LEN (123)
 #define WIM_ADC_S_LEN_BYTES (492)
 typedef union srv_live_data_s {
	 uint8_t byte[WIM_ADC_S_LEN_BYTES];
	 float word[WIM_ADC_S_LEN];
 }srv_live_data_t;
 extern srv_live_data_t srv_live_data;
/* Get ADC Code					39 */
extern uint32_t srv_adc_code;
/* SRV_SET_EN_ZERO_AT_POWER_UP			40 */
/* SRV_GET_EN_ZERO_AT_POWER_UP			41 */
extern uint8_t srv_en_zero_at_power_up_flag;
/* SRV_EN_AUTO_ZERO						42 */
/* SRV_GET_AUTO_ZERO_STATUS				43 */
extern uint8_t srv_auto_zero_flag;
/* SRV_GET_AUTO_ZERO_VALUE				44 */
extern float srv_auto_zero_value;
/* SRV_SET_ZERO							45 */
extern uint8_t srv_zero_flag;
/* SRV_GET_ZERO_VALUE					46 */
/* SRV_GET_INI_ZERO_VALUE				46 */
extern float srv_ini_zero_value;
/* SRV_CANCEL_ZERO						47 */
extern uint8_t srv_cancel_zero_flag;
/* SRV_AXLE_RECORD_REQUIRED				48 */
extern uint8_t srv_axle_record_flag;
/*	SRV_GET_ZERO_SETTING_RANGES			49 */
/* ADC Data axle record */
typedef struct __attribute__((packed)) srv_axle_record_s {
	uint32_t no;
	float   weight;
	float   speed;
	float distance;
}srv_axle_record_t;

#define SRV_AXLE_DATA_MAX_ITEMS  AXLE_AXLE_DATA_MAX_ITEMS

typedef struct __attribute__((packed)) srv_axle_data_s {
	float sample[SRV_AXLE_DATA_MAX_ITEMS];
}srv_axle_data_t;

typedef union srv_err_u {
    uint16_t bytes;
    struct {
		uint8_t low;
		uint8_t high;
	}byte;
    struct {
        uint8_t bit0 : 1;
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1;
        uint8_t bit8 : 1;
        uint8_t bit9 : 1;
        uint8_t bit10 : 1;
        uint8_t bit11 : 1;
        uint8_t bit12 : 1;
        uint8_t bit13 : 1;
        uint8_t bit14 : 1;
        uint8_t bit15 : 1;
    } bits;
}srv_err_t;
typedef struct __attribute__((packed)) srv_err_data_s {
    srv_err_t dev;
    srv_err_t sys;
    srv_err_t m4;
    srv_err_t weigh;
    srv_err_t wim;
}srv_err_data_t;
typedef struct __attribute__((packed)) srv_wim_s {
	float hs_spread;
	uint32_t tr_no;
	uint32_t no_axles;
    float    weight;
	uint8_t	 exit_status;
	srv_err_data_t err;
}srv_wim_t;
typedef struct __attribute__((packed)) srv_axle_s {
	srv_axle_record_t record;
	srv_axle_data_t   data;
	uint32_t          data_len;
}srv_axle_t;


/* SRV_GET_HW_STATUS					51 */
typedef struct __attribute__((packed)) srv_hw_status_s {
	uint8_t ERRB_AVDD_LDO;
	uint8_t ERRB_DVDD_LDO;
	int16_t chassis_temperature;
	uint8_t snrs_sts1;
	uint8_t snrs_sts2;
}srv_hw_status_t;
extern srv_hw_status_t srv_hw_status;
/* SRV_RECEIVE_CPU_LOAD					52 */
/* SRV_SET_DATA_LOGGING_FUNCTION		53 */
/* SRV_GET_DATA_LOGGING_FUNCTION		54 */
extern uint8_t srv_data_logging_function_flag;
/* SRV_RECEIVE_SD_STATUS				55 */
typedef struct __attribute__((packed)) srv_sd_status_s {
	uint8_t present;
	uint64_t size;
	uint64_t free_size;
	float free_perc;
}srv_sd_status_t;
extern srv_sd_status_t srv_sd_status;
/* SRV_SET_PLAZA_UPDATE				56 */
/* SRV_GET_PLAZA_UPDATE	 			57 */
typedef struct __attribute__((packed)) srv_plaza_update_s {
	uint8_t en_dis;
	uint8_t pident;
}srv_plaza_update_t;

extern volatile srv_plaza_update_t srv_plaza_update;

/* SRV_RECEIVE_DATA_LOGGING_STATUS		58 */
extern uint8_t srv_data_logging_status_flag;
/* SRV_SET_TIME							59 */
extern int64_t srv_time;
/* SRV_RECEIVE_CURRENT_TIME				60 */
/* SRV_GET_DATA_UPDATES					61 */
typedef struct __attribute__((packed)) srv_data_update_s {
	float inst_value;
	float auto_zero_value;
	float zero_value;
	srv_hw_status_t hw_status;
	union u {
		srv_wim_t wim;
		srv_axle_t axle;
	} u;
}srv_data_update_t;
extern srv_data_update_t srv_data_update;
/* SRV_GET_SETTINGS_UPDATES				62 */
typedef struct __attribute__((packed)) srv_settings_update_s {
	uint8_t wim_mode;
	uint8_t vs_mode;
	uint8_t cal_lock;
	uint8_t wim_dir;
	float   cpu_load;
	int64_t time_stamp;
	uint8_t rt_flag;
}srv_settings_update_t;
extern srv_settings_update_t srv_settings_update;


#if 1
typedef struct __attribute__((packed)) srv_plaza_srno_s {
	uint32_t b_srno;
	uint32_t m_srno;
}srv_plaza_data_srno_t;
typedef struct __attribute__((packed)) srv_plaza_wim_s {
	uint8_t wim_mode;
	uint8_t vs_mode;

    uint8_t comm_channel;
    uint8_t baud_rate;
    uint8_t sd_log;
    uint8_t sline;
    uint8_t noack;


	uint32_t tr_no;
	uint32_t no_axles;
    float    weight;
	uint8_t	 exit_status;
}srv_plaza_data_wim_t;
typedef struct __attribute__((packed)) srv_plaza_weigh_s {
	float inst_value;
	float hs_spread;
}srv_plaza_data_weigh_t;
typedef struct __attribute__((packed)) srv_plaza_diag_s {
	uint8_t snrs_sts1;
	uint8_t snrs_sts2;
	uint8_t sd_log;
}srv_plaza_data_diag_t;
typedef struct __attribute__((packed)) srv_plaza_err_s {
	srv_err_t dev;
	srv_err_t sys;
	srv_err_t m4;
	srv_err_t weigh;
	srv_err_t wim;
}srv_plaza_data_err_t;
typedef struct __attribute__((packed)) srv_plaza_ident_s {
	uint8_t fw_ver[32];
	uint8_t loc_id[32];
	uint8_t lane_id[32];
	uint8_t buf[32];
}srv_plaza_data_ident_t;
typedef struct __attribute__((packed)) srv_plaza_data_s {
	uint8_t pident;
	srv_plaza_data_srno_t srno;
	srv_plaza_data_wim_t wim;
	srv_plaza_data_weigh_t weigh;
	srv_plaza_data_diag_t diag;
	srv_plaza_data_err_t err;
	/* Identity */
	srv_plaza_data_ident_t ident;
}srv_plaza_data_t;
#endif


/*-------------------------------*------------------*/

/* payload struct */
typedef union __attribute__((packed)) srv_msg_req_payload_u {
/* Server message structures */
/*SRV_RT_REBOOT					0*/
	uint8_t rt_reboot;
/*SRV_RT_RESTART				1*/
	uint8_t rt_restart;
/* SRV_GET_FW_INFO					2 */
	srv_fw_info_t fw_info;
/* SRV_INSTALL_FW				3 */
	srv_install_fw_t install_fw;
/* SRV_SET_ASSET_ID						4 */
/* SRV_GET_ASSET_ID						5 */
	srv_asset_info_t asset_info;
/* SRV_SET_LOAD_CELL_PARAMETERS			6 */
/* SRV_GET_LOAD_CELL_PARAMETERS			7 */
	srv_loadcell_params_t loadcell_params;
/*	SRV_SET_NETWORK_PARAMS				8 */
	srv_network_params_t network_params;
/*	SRV_GET_NETWORK_PARAMS				9 */
	srv_network_params_get_t network_params_get;
/* SRV_SET_CONFIGURATION_DATA			10 */
/* SRV_GET_CONFIGURATION_DATA			11 */
	srv_wim_config_data_t wim_config_data;
/* SRV_SET_KNOBS						12 */
/* SRV_GET_KNOBS						13 */
	srv_knobs_t knobs;
#if 0
/* SRV_SET_SUPPRESS_NAN					14 */
/* SRV_GET_SUPRESS_NAN_STATUS			15 */
	uint8_t supress_nan;
#endif
#define SRV_SET_USER_OPTIONS				14
#define SRV_GET_USER_OPTIONS	 			15
	srv_user_options_t user_options;

/* SRV_SET_FILTER   					16 */
/* SRV_GET_FILTER	    				17 */
	srv_filter_t filter;
/* SRV_SET_DATA_RATE					18 */
	uint8_t data_rate;
/* SRV_SET_COM_CHANNEL					19 */
/* SRV_GET_COM_CHANNEL					20 */
	uint8_t com_channel;
/* SRV_SET_FACTORY_DEFAULT				21 */
	uint8_t factory_default;
/* SRV_SET_WIM_MODE						22 */
/* SRV_GET_WIM_MODE						23 */
	uint8_t wim_mode;
/* SRV_SET_DEFAULT_WIM_MODE				24 */
	uint8_t default_wim_mode;
/* SRV_SET_BP_CFG						25 */
/* SRV_GET_BP_CFG						26 */
	uint8_t bp_cfg;
/* SRV_SET_VS_MODE						27 */
/* SRV_GET_VS_MODE						28 */
	uint8_t vs_mode;
/* SRV_SET_SPEED_FACTOR					29 */
/* SRV_GET_SPEED_FACTOR					30 */
	float speed_factor;
/* SRV_SND_OTP							31 */
/* SRV_RCV_OTP							32 */
/* SRV_GEN_OTP							33 */
/* SRV_VAL_OTP							34 */
	srv_otp_t otp;
/* SRV_SET_CAL_TABLE					35 */
/* SRV_GET_CAL_TABLE					36 */
	srv_calibration_data_t calibration_data;
/* SRV_LIVE_DATA_REQUIRED				37 */
	uint8_t live_data_flag;
/* SRV_RECEIVE_LIVE_DATA				38 */
/* SRV_SET_ZERO_SETTING_RANGES			38 */
	srv_live_data_t live_data;

/* SRV_GET_ADC_CODE 					39 */
	uint32_t adc_code;
/* SRV_SET_EN_ZERO_AT_POWER_UP			40 */
/* SRV_GET_EN_ZERO_AT_POWER_UP			41 */
	uint8_t en_zero_at_power_up_flag;
/* SRV_EN_AUTO_ZERO						42 */
/* SRV_GET_AUTO_ZERO_STATUS				43 */
	uint8_t auto_zero_flag;
/* SRV_GET_AUTO_ZERO_VALUE				44 */
	float auto_zero_value;
/* SRV_SET_ZERO							45 */
	uint8_t zero_flag;
/* SRV_GET_ZERO_VALUE					46 */
	float zero_value;
	float cancel_zero_value;
/* SRV_GET_INI_ZERO_VALUE				46 */
	float ini_zero_value;
/* SRV_CANCEL_ZERO						47 */
	uint8_t cancel_zero_flag;
/* SRV_AXLE_RECORD_REQUIRED				48 */
	uint8_t axle_record_flag;
/*	SRV_GET_ZERO_SETTING_RANGES         49 */
	srv_zero_setting_ranges_t zero_setting_ranges;

/* ADC Data axle record */
	srv_axle_record_t axle_record;
/* SRV_RECEIVE_AXLE_DATA				50 */
	srv_axle_t axle;
/* SRV_GET_HW_STATUS					51 */
	srv_hw_status_t hw_status;
/* SRV_RECEIVE_CPU_LOAD					52 */
	float cpu_load;
/* SRV_SET_DATA_LOGGING_FUNCTION		53 */
/* SRV_GET_DATA_LOGGING_FUNCTION		54 */
	uint8_t data_logging_function_flag;
/* SRV_RECEIVE_SD_STATUS				55 */
	srv_sd_status_t sd_status;
/* SRV_SET_PLAZA_UPDATE				56 */
/* SRV_GET_PLAZA_UPDATE	 			57 */
	srv_plaza_update_t srv_plaza_update;
	srv_plaza_data_t srv_plaza_data;

/* SRV_RECEIVE_DATA_LOGGING_STATUS		58 */
	uint8_t data_logging_status_flag;
/* SRV_SET_TIME							59 */
	uint64_t time;
/* SRV_RECEIVE_CURRENT_TIME				60 */
/* SRV_GET_DATA_UPDATES					61 */
	srv_data_update_t data_update;
/* SRV_GET_SETTINGS_UPDATES				62 */
	srv_settings_update_t settings_update;
/* payload struct */
}srv_msg_req_payload_t;

typedef struct __attribute__((packed)) srv_msg_rp_header_s {
	uint8_t id;
	uint8_t status;
}srv_msg_rp_header_t;

typedef struct __attribute__((packed)) srv_msg_req_s {
	srv_msg_rp_header_t   header;
	srv_msg_req_payload_t payload;
}srv_msg_rp_t;




#endif /* SERVER_H__ */
