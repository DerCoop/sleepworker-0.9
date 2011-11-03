# Process this file with autoconf to produce a configure script.

AC_INIT([sleepworker], [0.9.06], [dercoop@users.sf.net])
AC_PREREQ([2.65])
AM_INIT_AUTOMAKE([sleepworker], [0.9.06])
AC_DEFINE(VERSION, [0.9.06]) 
# or AC_DEFINE(DEBUG)
AC_CONFIG_SRCDIR([src/sleepworker.c])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

AC_LANG_C
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
#Checks for stdlib.h, stdarg.h , string.h and float.h, defines STDC_HEADERS on success.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([select strdup strerror strstr])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 examples/Makefile
                 man/Makefile
                 scripts/Makefile
                 src/Makefile])
AC_OUTPUT
