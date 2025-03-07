#ifndef APP_DEBUG_ARCH_H__
#define APP_DEBUG_ARCH_H__

int app_debug_sync_init(void);
int app_debug_sync_printf(const char *format, ...);

#if (APP_DEBUG >0)

#define APP_PLATFORM_DIAG(message,...) \
do{ \
  app_debug_sync_printf( message, ##__VA_ARGS__); \
}while(0)
#else
#define APP_PLATFORM_DIAG(message,...)
#endif


#endif /* APP_DEBUG_ARCH_H__ */

