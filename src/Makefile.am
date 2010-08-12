#
#   tcprstat -- Extract stats about TCP response times
#   Copyright (C) 2010  Ignacio Nin
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

AM_CFLAGS = -Wall -Werror -g

bin_PROGRAMS = tcprstat tcprstat-static
tcprstat_SOURCES = tcprstat.h tcprstat.c functions.h functions.c capture.h \
	capture.c process-packet.h process-packet.c local-addresses.h \
	local-addresses.c stats.h stats.c output.h output.c stats-hash.h \
	stats-hash.c
tcprstat_static_SOURCES = $(tcprstat_SOURCES)

if BUILD_PCAP
tcprstat_CFLAGS = $(AM_CFLAGS) -pthread -I$(top_srcdir)/@LIBPCAP_SUBDIR@
tcprstat_LDFLAGS = $(AM_LDFLAGS) -L$(top_srcdir)/@LIBPCAP_SUBDIR@ -lpthread
tcprstat_LDADD = -lpcap

tcprstat_static_CFLAGS = $(AM_CFLAGS) -pthread -I$(top_srcdir)/@LIBPCAP_SUBDIR@
tcprstat_static_LDFLAGS = $(AM_LDFLAGS) -static -L$(top_srcdir)/@LIBPCAP_SUBDIR@
tcprstat_static_LDADD = -lpthread -lpcap

else
tcprstat_CFLAGS = $(AM_CFLAGS) -pthread
tcprstat_LDFLAGS = $(AM_LDFLAGS) -lpthread -lpcap

tcprstat_static_CFLAGS = $(AM_CFLAGS) -pthread
tcprstat_static_LDFLAGS = $(AM_LDFLAGS) -static
tcprstat_static_LDADD = -lpthread -lpcap

endif
