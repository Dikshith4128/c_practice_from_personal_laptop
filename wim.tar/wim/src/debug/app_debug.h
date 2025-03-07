#ifndef APP_DEBUG_H__
#define APP_DEBUG_H__

#include "app_debug_config.h"
#include "app_debug_opts.h"
#include "app_debug_arch.h"

/** lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define APP_DBG_LEVEL_ALL     0x00
#define APP_DBG_LEVEL_OFF     APP_DBG_LEVEL_ALL /* compatibility define only */
#define APP_DBG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define APP_DBG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define APP_DBG_LEVEL_SEVERE  0x03
#define APP_DBG_MASK_LEVEL    0x03

/** flag for APP_DEBUGF to enable that debug message */
#define APP_DBG_ON            0x80U
/** flag for APP_DEBUGF to disable that debug message */
#define APP_DBG_OFF           0x00U

/** flag for APP_DEBUGF indicating a tracing message (to follow program flow) */
#define APP_DBG_TRACE         0x40U
/** flag for APP_DEBUGF indicating a state debug message (to follow module states) */
#define APP_DBG_STATE         0x20U
/** flag for APP_DEBUGF indicating newly added code, not thoroughly tested yet */
#define APP_DBG_FRESH         0x10U
/** flag for APP_DEBUGF to halt after printing this debug message */
#define APP_DBG_HALT         0x08U


#if (APP_DEBUG > 0)
/** print debug message only if debug message type is enabled...
 *  AND is of correct type AND is at least APP_DBG_LEVEL
 */
#define APP_DEBUGF(debug, message,...) do{ \
                               if( \
									((debug) & APP_DBG_ON) && \
									((debug) & APP_DBG_TYPES_ON) && \
									((int16_t)((debug) & APP_DBG_MASK_LEVEL) >= APP_DBG_MIN_LEVEL)) { \
										APP_PLATFORM_DIAG( message,##__VA_ARGS__); \
										if((debug) & APP_DBG_HALT) { \
											while(1); \
										} \
									} \
                             }while(0)

#else  /* APP_DEBUG */
#define APP_DEBUGF(debug, message,...)
#endif /* APP_DEBUG */

#endif /* APP_DEBUG_H__ */

