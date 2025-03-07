#define _GNU_SOURCE
#include <config.h>
#include <stdarg.h>
#include <stdint.h>

#include <alsdk.h>
#include "app_debug_arch.h"

static pthread_mutex_t app_debug_printf_sync_mutex;

int app_debug_sync_init(void)
{
	alsdk_pthread_mutex_init(&app_debug_printf_sync_mutex, NULL);

	return 0;
}

int app_debug_sync_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    alsdk_pthread_mutex_lock(&app_debug_printf_sync_mutex);
    vprintf(format, args);
    alsdk_pthread_mutex_unlock(&app_debug_printf_sync_mutex);

    va_end(args);
}
