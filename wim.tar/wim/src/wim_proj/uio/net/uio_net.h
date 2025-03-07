#ifndef UIO_NET_H__
#define UIO_NET_H__

#include "alsdk_ringbuf.h"
#include "rt_timer.h"
#include "lt_timer.h"
typedef enum uio_net_state_e {
	UIO_NET_INIT = 0,
	UIO_NET_BIND,
	UIO_NET_LISTEN,
	UIO_NET_CONNECTED,
	UIO_NET_DISCONNECTED
}uio_net_state_t;
/**
 * @struct net device structure.
 .* Encapsulates a net connection.
 */
typedef struct uio_net_s {

	pthread_mutex_t mutex;
	char const* ifc; /* device */
	uint16_t port;
	int fd;			/* listen cocket descriptor */

	uio_net_state_t state;	/* Signifies connection state */
	int open_fd;			/* open socket descriptor */
	uint8_t ack_send;
	uint8_t ack_pend;

	pthread_mutex_t rrb_mutex; /* read ringbuf mutex */
	alsdk_ringbuf_t rrb; /* read ringbuf */

	pthread_mutex_t pbuf_mutex; /* write rinfbuf mutex */
	char* pbuf;
	size_t pbuf_size;
	size_t pbuf_index;
	uint8_t pbuf_detect;

	pthread_mutex_t wrb_mutex; /* write rinfbuf mutex */
	alsdk_ringbuf_t wrb; /* write ring buf*/
	uint32_t count;
    int epoll_fd;
	rt_timer_obj_t *ack_timer;
}uio_net_t;

int uio_net_init(uio_net_t *s, int epoll_fd, lt_timer_obj_t *ack_timer, char const* ifc, uint16_t port,
		char* rbuf, size_t rbuf_size, char* wbuf, size_t wbuf_size, char* pbuf, size_t pbuf_size);
int uio_net_open(uio_net_t *s);
int uio_net_listen(uio_net_t *s);
int uio_net_handle_accept(uio_net_t* s);
int uio_net_handle_accept_serial(uio_net_t* s);
int uio_net_close(uio_net_t *s);
int uio_net_destroy(uio_net_t *s);




int uio_net_do_write(uio_net_t* s, char* buf);

int uio_net_send(uio_net_t * s, char* data, size_t len);
void uio_net_putchar(uio_net_t * s, char data);

int uio_net_get_line(uio_net_t* s, char* data, size_t len);
int uio_net_flush(uio_net_t* s);


int uio_net_send_buff(uio_net_t* s, char* data , size_t len);

int uio_net_reset_send_buff(uio_net_t* s);

#endif /* UIO_NET_H__ */
