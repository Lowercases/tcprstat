#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#
#   rtime -- Extract stats about TCP response times
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

AC_PREREQ(2.61)
AC_INIT([rtime], [0.1], [ignacio.nin@percona.com])
AM_INIT_AUTOMAKE([ ])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Pcap static check
save_LDFLAGS="$LDFLAGS"
LDFLAGS=-static
AC_CHECK_LIB([pcap], [pcap_open_live], , [nostaticpcap=yes])
LDFLAGS="$save_LDFLAGS"

if test x$nostaticpcap == xyes
then
    # Build our shipped version of libpcap.
    (cd libpcap && tar xzf libpcap*.tar.gz) || {
        AC_MSG_ERROR([error unpacking libpcap tar])
    }
    
    AC_CONFIG_SUBDIRS([libpcap/libpcap-*/])

fi

AM_CONDITIONAL(BUILD_PCAP, test x$nostaticpcap == xyes)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
if test x$nostaticpcap == xyes
then
    AC_CONFIG_FILES([libpcap/Makefile])
fi

AC_OUTPUT
