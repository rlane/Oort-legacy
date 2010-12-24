#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([risc], [0.1], [risc-game-dev@sourceforge.net])
AC_CONFIG_SRCDIR([test/test_check_collision.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
#AC_CHECK_LIB([GL], [main])
#AC_CHECK_LIB([GLU], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([clock_gettime munmap sqrt strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
