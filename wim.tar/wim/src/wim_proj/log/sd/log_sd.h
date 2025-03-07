#ifndef LOG_SD_H__
#define LOG_SD_H__

#include "alsdk_ringbuf.h"
#include <stddef.h>
#define LOG_SD_BUFF_SIZE 2040

typedef struct log_sd_s {
	int null_fd;               /* Connection file descriptor */
	uint8_t use;
	uint8_t present;
	uint8_t mounted;
	char const* root;          /* sd root mount */
	int fd;                    /* Connection file descriptor */
	int fd_year;
	int fd_month;
	int fd_day;
	pthread_mutex_t mutex;
    alsdk_ringbuf_t wrb;       /* write ring buf*/
    pthread_mutex_t wrb_mutex; /* write rinfbuf mutex */
    char	buff[LOG_SD_BUFF_SIZE];
	size_t  buff_index;

}log_sd_t;

int log_sd_mount(void);
int log_sd_unmount(log_sd_t* s);

int log_sd_init(log_sd_t* s, char const* root, char* wbuf, size_t wbuf_size);
int log_sd_open(log_sd_t* s);
int log_sd_close(log_sd_t* s);
int log_sd_destroy(log_sd_t* s);
int	log_sd_handle_eod(log_sd_t* s);

int log_sd_send(log_sd_t* s, char* data, size_t len);
int log_sd_flush(log_sd_t* s);

int log_sd_send_direct(log_sd_t* s, char* data , size_t len);
int log_sd_send_buff(log_sd_t* s, char* data , size_t len);
int log_sd_get_line_from_buff(log_sd_t* s, char* data, size_t len);
int log_sd_flush_line_from_buff(log_sd_t* s);


#endif /* LOG_SD_H__ */
