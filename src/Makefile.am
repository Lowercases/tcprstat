AM_CFLAGS = -Wall -Werror -g

bin_PROGRAMS = rtime
rtime_SOURCES = rtime.h rtime.c functions.h functions.c capture.h capture.c \
	process-packet.h process-packet.c local-addresses.h local-addresses.c \
	stats.h stats.c
rtime_LDFLAGS = $(AM_LDFLAGS) -lpcap -lpthread
rtime_CFLAGS = $(AM_CFLAGS) -pthread