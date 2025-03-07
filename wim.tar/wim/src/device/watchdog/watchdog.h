#ifndef WATCHDOG_H__
#define DEVICE_H__

#include <stdint.h>

int device_watchdog_init(void);
int device_watchdog_hwfeed(void);

#endif /* WATCHDOG_H__ */
