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
AC_INIT([rtime], [0.2], [ignacio.nin@percona.com])
AM_INIT_AUTOMAKE([ ])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([pcap], [pcap_open_live], [ : ],
    AC_MSG_ERROR([libpcap not found -- please install of specify]))

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
