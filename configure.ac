#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

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

AC_PREREQ(2.61)
AC_INIT([tcprstat], [0.3.1], [ignacio.nin@percona.com])
AM_INIT_AUTOMAKE([ ])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_YACC
AC_PROG_LEX

# Checks for libraries.

# Pcap static check
save_LDFLAGS="$LDFLAGS"
LDFLAGS=-static
AC_CHECK_LIB([pcap], [pcap_open_live], , [buildpcap=yes])
LDFLAGS="$save_LDFLAGS"

# Checks for header files.
AC_CHECK_HEADERS(
    [arpa/inet.h netinet/in.h stdint.h stdlib.h string.h sys/time.h unistd.h])
AC_CHECK_HEADER([pcap/sll.h], , [buildpcap=yes])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T
AC_CHECK_SIZEOF([unsigned long])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday inet_ntoa memset strdup strtol])

# Build our version of libpcap
if test x$buildpcap == xyes
then
    # We need flex and yacc
    if ! $YACC --version &>/dev/null
    then
        AC_MSG_ERROR(
            [Cannot build libpcap without bison or yacc. Please install either.])
    elif test "x`$LEX --version`" == "x"
    then
        AC_MSG_ERROR(
            [Cannot build libpcap without flex. Please install.])
    fi
    
    # Build our shipped version of libpcap.
    (cd libpcap && tar xzf libpcap*.tar.gz && cd libpcap*/ &&
        patch -p1 < ../resolver-patch) || {
        AC_MSG_ERROR([error unpacking libpcap tar])
    }
    
    LIBPCAP_SUBDIR=`echo libpcap/libpcap-*/`
    AC_SUBST(LIBPCAP_SUBDIR)
    
    AC_CONFIG_SUBDIRS([libpcap/libpcap-*/])
    AC_CONFIG_FILES([libpcap/Makefile])
    
fi

AM_CONDITIONAL(BUILD_PCAP, test x$buildpcap == xyes)

AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
