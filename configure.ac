#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright (C) 2016 Jean Michel Sellier <jeanmichel.sellier@gmail.com>
# Copyright (C) 2022 Karl Semich <0xloem@gmail.com>
#
# This file is part of Gneural Knockoff.
# 
# Gneural Knockoff is free software: you can redistribute it and/or modify it
# under the terms of the GNU Affero General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
# 
# Gneural Knockoff is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
# for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with Gneural Knockoff. If not, see <https://www.gnu.org/licenses/>.


AC_PREREQ([2.69])
AC_INIT([gneural], [0.0.1.2], [jeanmichel.sellier@gmail.com])
AC_CONFIG_SRCDIR([src/gneural_network.c],[src/nnet.c])
#AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [exp], [echo "GNU libm found."], [echo "GNU libm is not found. Aborting"])

# Checks for header files.
AC_CHECK_HEADERS([memory.h stdint.h stdlib.h string.h inttypes.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
#AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT([src/Makefile])
