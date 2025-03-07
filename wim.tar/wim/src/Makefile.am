#wim
AM_CPPFLAGS = $(EXTRA_CPPFLAGS) \
			  -I$(top_srcdir)/include \
			  -I$(top_srcdir)/libalsdk \
			  -I$(top_srcdir)/libalsdk/src/core \
			  -I$(top_srcdir)/libalsdk/src/wrappers \
			  -I$(top_srcdir)/libalsdk/src/utils/environ \
			  -I$(top_srcdir)/libalsdk/src/utils/random \
			  -I$(top_srcdir)/libalsdk/src/utils/bsema \
			  -I$(top_srcdir)/libalsdk/src/utils/ds \
			  -I$(top_srcdir)/libalsdk/src/utils/ringbuf \
			  -I$(top_srcdir)/libalsdk/src/utils/security \
			  -I$(top_srcdir)/libalsdk/src/utils/filters
AM_CFLAGS   = $(EXTRA_CFLAGS)
AM_CXXFLAGS = $(EXTRA_CXXFLAGS)
AM_LDFLAGS  = $(EXTRA_LDFLAGS)
AM_LIBS     = $(EXTRA_LIBS)


#PROGRAM
bin_PROGRAMS = wim

#SOURCES
wim_SOURCES = main.h main.c
wim_SOURCES += debug/app_debug_config.h debug/app_debug_opts.h debug/app_debug_arch.h debug/app_debug.h debug/app_debug_arch.c

wim_SOURCES += board/board.h board/board.c
wim_SOURCES += system/system.h system/system.c
wim_SOURCES += device/device.h device/device.c
wim_SOURCES += device/cfg/dev_cfg.h device/cfg/dev_cfg.c
wim_SOURCES += device/watchdog/watchdog.h device/watchdog/watchdog.c
wim_SOURCES += license/license.h license/license.c

wim_SOURCES += wim_proj/project.h wim_proj/project.c
wim_SOURCES += wim_proj/libweigh/libweigh.h wim_proj/libweigh/libweigh.c
wim_SOURCES += wim_proj/libwim/libwim.h wim_proj/libwim/libwim.c

wim_SOURCES += wim_proj/utils/timers/rt/rt_timer.h wim_proj/utils/timers/rt/rt_timer.c
wim_SOURCES += wim_proj/utils/timers/lt/lt_timer.h wim_proj/utils/timers/lt/lt_timer.c

wim_SOURCES += wim_proj/cfg/cfg.h wim_proj/cfg/cfg.c
wim_SOURCES += wim_proj/cfg/cfg_default.h wim_proj/cfg/cfg_default.c
wim_SOURCES += wim_proj/cfg/cfg_backup.h wim_proj/cfg/cfg_backup.c
wim_SOURCES += wim_proj/cfg/cfg_active.h wim_proj/cfg/cfg_active.c
wim_SOURCES += wim_proj/cfg/emmc/cfg_emmc.h wim_proj/cfg/emmc/cfg_emmc.c
wim_SOURCES += wim_proj/cfg/nand/cfg_nand.h wim_proj/cfg/nand/cfg_nand.c

wim_SOURCES += wim_proj/log/log.h wim_proj/log/log.c
wim_SOURCES += wim_proj/log/console/log_console.h wim_proj/log/console/log_console.c
wim_SOURCES += wim_proj/log/sd/log_sd.h wim_proj/log/sd/log_sd.c

wim_SOURCES += wim_proj/io/io.h wim_proj/io/io.c
wim_SOURCES += wim_proj/io/debounce/io_debounce.h wim_proj/io/debounce/io_debounce.c

wim_SOURCES += wim_proj/uio/serial/uio_serial.h wim_proj/uio/serial/uio_serial.c
wim_SOURCES += wim_proj/uio/net/uio_net.h wim_proj/uio/net/uio_net.c
wim_SOURCES += wim_proj/uio/uio.h wim_proj/uio/uio.c

wim_SOURCES += wim_proj/guio/char_lcd/guio_char_lcd.h wim_proj/guio/char_lcd/guio_char_lcd.c
wim_SOURCES += wim_proj/guio/guio.h wim_proj/guio/guio.c

wim_SOURCES += wim_proj/guio/input/mkeypad/mkeypad.h wim_proj/guio/input/mkeypad/mkeypad.c
wim_SOURCES += wim_proj/guio/input/input.h wim_proj/guio/input/input.c

wim_SOURCES += wim_proj/weigh/adc/adc.h wim_proj/weigh/adc/adc.c
wim_SOURCES += wim_proj/weigh/axle/axle.h wim_proj/weigh/axle/axle.c
wim_SOURCES += wim_proj/weigh/wim/wim.h wim_proj/weigh/wim/wim.c
wim_SOURCES += wim_proj/weigh/ale/ale.h wim_proj/weigh/ale/ale.c

wim_SOURCES += wim_proj/server/server.h wim_proj/server/server.c
wim_SOURCES += wim_proj/weigh/weigh.h wim_proj/weigh/weigh.c

#CPPFLAGS
wim_CPPFLAGS  =  $(AM_CPPFLAGS)
wim_CPPFLAGS +=  $(CODE_COVERAGE_CPPFLAGS)
wim_CPPFLAGS +=  $(PTHREAD_CPPFLAGS)
wim_CPPFLAGS += -I$(top_srcdir)/src
wim_CPPFLAGS += -I$(top_srcdir)/src/debug
wim_CPPFLAGS += -I$(top_srcdir)/src/board
wim_CPPFLAGS += -I$(top_srcdir)/src/system
wim_CPPFLAGS += -I$(top_srcdir)/src/device
wim_CPPFLAGS += -I$(top_srcdir)/src/device/cfg
wim_CPPFLAGS += -I$(top_srcdir)/src/device/watchdog
wim_CPPFLAGS += -I$(top_srcdir)/src/license
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/libweigh
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/libwim
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/cfg
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/cfg/emmc
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/cfg/nand
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/utils/timers/rt
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/utils/timers/lt
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/log
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/log/console
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/log/sd
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/io
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/io/debounce
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/uio
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/uio/serial
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/uio/net
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/guio/input
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/guio/input/mkeypad
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/guio
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/guio/char_lcd
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/weigh/
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/weigh/adc
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/weigh/axle
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/weigh/wim
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/weigh/ale
wim_CPPFLAGS += -I$(top_srcdir)/src/wim_proj/server

#CFLAGS
wim_CFLAGS  = $(AM_CFLAGS)
wim_CFLAGS += $(CODE_COVERAGE_CFLAGS)
wim_CFLAGS += $(PTHREAD_CFLAGS)

#CXXFLAGS
wim_CXXFLAGS  = $(AM_CXXFLAGS)
wim_CXXFLAGS += $(CODE_COVERAGE_CXXFLAGS)
wim_CXXFLAGS += $(PTHREAD_CXXFLAGS)

#LDFLAGS
wim_LDFLAGS  =  $(AM_LDFLAGS)
wim_LDFLAGS +=  $(CODE_COVERAGE_LDFLAGS)
wim_LDFLAGS +=  $(PTHREAD_LDFLAGS)

#LIBS
wim_LIBS =  $(AM_LIBS)
wim_LIBS += $(CODE_COVERAGE_LIBS)
wim_LIBS += $(PTHREAD_LIBS)

#LDADD
wim_LDADD  =  $(PTHREAD_LIBS) -lgsl -lgslcblas -lgpiod -lm -lzmq -lrt
#wim_LDADD += -dlopen $(top_builddir)/libalsdk/libalsdk.la
wim_LDADD +=  $(top_builddir)/libalsdk/libalsdk.la

# valgring
TESTS = wim
check_PROGRAMS =  wim

#CODE_COVERAGE_DIRECTORY = $(top_builddir)/src

### Add ax_* rules ###
#CODE_COVERAGE
#SPLINT
AM_SPLINTFLAGS = +posixlib +unixlib +gnuextensions --formatcode -D__signed__= \
				 -I $(LIB_ALSDK_BASE)/src/core \
				 -I $(LIB_ALSDK_BASE)/src/wrappers \
				 -I $(LIB_ALSDK_BASE)/src/utils/environ \
				 -I $(LIB_ALSDK_BASE)/src/utils/random \
				 -I $(LIB_ALSDK_BASE)/src/utils/bsema \
				 -I $(LIB_ALSDK_BASE)/src/utils/ds \
				 -I $(LIB_ALSDK_BASE)/src/utils/ringbuf \
				 -I $(LIB_ALSDK_BASE)/src/utils/security \
				 -I $(LIB_ALSDK_BASE)/src/utils/filters \
				 -I $(top_srcdir)/src \
				 -I$(top_srcdir)/src/debug \
				 -I$(top_srcdir)/src/board \
				 -I$(top_srcdir)/src/system \
				 -I$(top_srcdir)/src/device \
				 -I$(top_srcdir)/src/device/cfg \
				 -I$(top_srcdir)/src/device/watchdog \
				 -I$(top_srcdir)/src/wim_proj/libweigh \
				 -I$(top_srcdir)/src/wim_proj/libwim \
				 -I$(top_srcdir)/src/wim_proj/cfg \
				 -I$(top_srcdir)/src/wim_proj/cfg/emmc \
				 -I$(top_srcdir)/src/wim_proj/cfg/nand \
				 -I$(top_srcdir)/src/wim_proj/utils/timers/rt \
				 -I$(top_srcdir)/src/wim_proj/utils/timers/lt \
				 -I$(top_srcdir)/src/wim_proj/log \
				 -I$(top_srcdir)/src/wim_proj/log/console \
				 -I$(top_srcdir)/src/wim_proj/log/sd \
				 -I$(top_srcdir)/src/wim_proj/io \
				 -I$(top_srcdir)/src/wim_proj/io/debounce \
				 -I$(top_srcdir)/src/wim_proj/uio \
				 -I$(top_srcdir)/src/wim_proj/uio/serial \
				 -I$(top_srcdir)/src/wim_proj/uio/net \
				 -I$(top_srcdir)/src/wim_proj/guio \
				 -I$(top_srcdir)/src/wim_proj/guio/char_lcd \
				 -I$(top_srcdir)/src/wim_proj/weigh \
				 -I$(top_srcdir)/src/wim_proj/weigh/adc \
				 -I$(top_srcdir)/src/wim_proj/weigh/axle \
				 -I$(top_srcdir)/src/wim_proj/weigh/wim \
				 -I$(top_srcdir)/src/wim_proj/server

include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean

#VALGRIND
@VALGRIND_CHECK_RULES@
