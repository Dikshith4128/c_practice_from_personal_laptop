#ifndef PROJECT_H__
#define PROJECT_H__

#include <config.h>
#include <stdint.h>

/*------ wim hardware flags  and data -----*/
#define WIM_ADC_ADS1255 1
#define WIM_ADC_AD7190  2

#define WIM_BP_NORMAL	  0
#define WIM_BP_STAGGERED  1

#if (BP == WIM_BP_NORMAL)
#define WIM_BP  WIM_BP_NORMAL
#warning WIM_BP_NORMAL
#elif (BP == WIM_BP_STAGGERED)
#define WIM_BP  WIM_BP_STAGGERED
#warning WIM_BP_STAGGERED
#else
#error UNKNOW BP $BP
#endif

/* LCD */
#define WIM_CLCD_NONE		0
#define WIM_CLCD_HD44780	1

#define WIM_CLCD_RES_NONE	0
#define WIM_CLCD_RES_2_16	1
#define WIM_CLCD_RES_4_20   2


/* Matrix Keypad */
#define WIM_MKEYPAD_NONE 0
#define WIM_MKEYPAD_4_4  1
#define WIM_MKEYPAD_5_4  2


/*------ wim flags and data -----*/

#define WIM_TIM 1
#define WIM_RIM 2

#if (WIM == WIM_TIM)
#define WIM_TYPE  WIM_TIM
#warning WIM_TYPE_TIM
#elif (WIM == WIM_RIM)
#define WIM_TYPE  WIM_RIM
#warning WIM_TYPE_RIM
#else
#error UNKNOW WIM $WIM
#endif



/* Customer specific*/
#define WIM_CUST_ARETE		0
#define WIM_CUST_ASPEKT		1
#define WIM_CUST_ESSAE		2
#define WIM_CUST_LANDT		3
#define WIM_CUST_MERIT		4
#define WIM_CUST_RAJDEEP	5
#define WIM_CUST_VAAAN		6
#define WIM_CUST_TULAMAN	7
#define WIM_CUST_NAGARJUN	8
#define WIM_CUST_FCS		9
#define WIM_CUST_JWS		10
#define WIM_CUST_EUROTECH 	11
#define WIM_CUST_MCUBE		12
#define WIM_CUST_MAX		13


#define P_HW_INFO_RS232_BIT	0
#define P_HW_INFO_ETH_BIT	1
#define P_HW_INFO_USB_BIT	2
#define P_HW_INFO_RS485_BIT	3
#define P_HW_INFO_1255_BIT	4
#define P_HW_INFO_7190_BIT	5
#define P_HW_INFO_LCD_BIT	6
#define P_HW_INFO_MKEY_BIT	7

#define P_HW_INFO_RS232_MASK	(1 << P_HW_INFO_RS232_BIT)
#define P_HW_INFO_ETH_MASK		(1 << P_HW_INFO_ETH_BIT)
#define P_HW_INFO_USB_MASK		(1 << P_HW_INFO_USB_BIT)
#define P_HW_INFO_RS485_MASK	(1 << P_HW_INFO_RS485_BIT)
#define P_HW_INFO_1255_MASK		(1 << P_HW_INFO_1255_BIT)
#define P_HW_INFO_7190_MASK		(1 << P_HW_INFO_7190_BIT)
#define P_HW_INFO_LCD_MASK		(1 << P_HW_INFO_LCD_BIT)
#define P_HW_INFO_MKEY_MASK		(1 << P_HW_INFO_MKEY_BIT)

#define WIM_AXLE_ARETE 0
#define WIM_AXLE_MERIT 1

#define WIM_SEP_ARETE 0

#define WIM_ARB_ARETE 0
#define WIM_ARB_LANDT 1

#define WIM_MERIT_AXLE_STORE_SIZE 1000

#define WIM_LANDT_AXLE_BUFFER_SIZE 8192

typedef struct project_wim_info_s {
	uint8_t axle;
	uint8_t sep;
	uint8_t arb;
}project_wim_info_t;
typedef struct project_cust_info_s {
	uint32_t cust_id;
	uint16_t ui_port;
	uint16_t net_io_port;
}project_cust_info_t;

typedef struct project_hw_info_adc_s {
	uint8_t  type;
	uint32_t sample_rate;
	float pga;
	float vref;
	uint8_t f1_size;
	uint8_t f2_size;
	uint16_t npkts_phs;
	uint32_t axle_ndata;
}project_hw_info_adc_t;

typedef struct project_hw_info_io_s {
	uint8_t rs232;
	uint8_t eth;
	uint8_t usb;
	uint8_t rs485;
}project_hw_info_io_t;

typedef struct project_hw_info_lcd_s {
	uint8_t type;
	uint8_t res;
	uint8_t nrows;
	uint8_t ncols;
}project_hw_info_lcd_t;

typedef struct project_hw_info_mkeypad_s {
	uint8_t type;
	uint8_t nrows;
	uint8_t ncols;
}project_hw_info_mkeypad_t;

typedef struct project_hw_info_sdcard_s {
	uint8_t populated;
	uint8_t use;
}project_hw_info_sdcard_t;

typedef struct project_hw_info_s {
	uint32_t ser_no;
	uint32_t ver_no;

	project_hw_info_adc_t	  adc;
	project_hw_info_io_t	  io;
	project_hw_info_lcd_t	  lcd;
	project_hw_info_mkeypad_t mkeypad;
	project_hw_info_sdcard_t  sdcard;
}project_hw_info_t;

typedef struct project_info_s {
	project_cust_info_t cust;
	project_hw_info_t hw;
	project_wim_info_t wim;
}project_info_t;

#define PROJECT_IMAGE_INFO_FILE        "/etc/os-release"
#define PROJECT_IMAGE_INFO_FILE_NAME   "os-release"


typedef struct project_image_info_s {
	uint8_t  version[16];
}project_image_info_t;
typedef struct project_m4_info_s {
	uint8_t  version[16];
}project_m4_info_t;


extern volatile project_image_info_t project_image_info;
extern volatile project_m4_info_t    project_m4_info;

extern volatile project_info_t project_info;
int project_info_init(void);


#endif /* PROJECT_H__ */
