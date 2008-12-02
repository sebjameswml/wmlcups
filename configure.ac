dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT([wmlcups], [0.1], [sjames@wmltd.co.uk])
AC_CONFIG_SRCDIR(wmlcups/cups-inetd.c)
AC_REVISION([$Revision: 1.3 $])
AC_PREFIX_DEFAULT(/usr)

AM_INIT_AUTOMAKE([1.9 foreign])

AM_CONFIG_HEADER(wmlcups/config.h)
AM_MAINTAINER_MODE

AC_LANG_C

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_CHECK_PROG(DOXYGEN, doxygen, doxygen, /bin/echo)

dnl Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_HEADER_STDC
AC_FUNC_STAT
AC_CHECK_FUNCS([memset strerror strstr strtol])

dnl Checks for header files.
AC_CHECK_HEADERS([sys/statvfs.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
dnl Checks for endianness
AC_C_BIGENDIAN

dnl Checks for word sizes
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(unsigned char)
if test "$ac_cv_sizeof_char" -eq 1; then
  AC_DEFINE(INT8_TYPE, [char], [Signed 8 bit])
else
  AC_MSG_ERROR([compiler has no 1 byte char])
fi
if test "$ac_cv_sizeof_unsigned_char" -eq 1; then
  AC_DEFINE(UINT8_TYPE, [unsigned char], [Unsigned 8 bit])
else
  AC_MSG_ERROR([compiler has no 1 byte unsigned char])
fi

AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)
if test "$ac_cv_sizeof_short" -eq 2; then
  AC_DEFINE(INT16_TYPE, [short], [Signed 16 bit])
else
  AC_MSG_ERROR([compiler has no 2 byte short])
fi
if test "$ac_cv_sizeof_unsigned_short" -eq 2; then
  AC_DEFINE(UINT16_TYPE, [unsigned short], [Unsigned 16 bit])
else
  AC_MSG_ERROR([compiler has no 2 byte unsigned short])
fi

AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(unsigned long)
if test "$ac_cv_sizeof_int" -eq 4; then
  AC_DEFINE(INT32_TYPE, [int], [Signed 32 bit])
elif test "$ac_cv_sizeof_long" -eq 4; then
  AC_DEFINE(INT32_TYPE, [long], [Signed 32 bit])
else
  AC_MSG_ERROR([compiler has no 4 byte integer])
fi
if test "$ac_cv_sizeof_unsigned_int" -eq 4; then
  AC_DEFINE(UINT32_TYPE, [unsigned int], [Unsigned 32 bit])
elif test "$ac_cv_sizeof_unsigned_long" -eq 4; then
  AC_DEFINE(UINT32_TYPE, [unsigned long], [Unsigned 32 bit])
else
  AC_MSG_ERROR([compiler has no 4 byte unsigned integer])
fi

if test "$ac_cv_sizeof_long" -eq 8; then
  AC_DEFINE(INT64_TYPE, [long], [Signed 64 bit])
else
  AC_CHECK_SIZEOF(long long)
  if test "$ac_cv_sizeof_long_long" -eq 8; then
    AC_DEFINE(HAVE_LONG_LONG, 1, [Have the long long type])
    AC_DEFINE(INT64_TYPE, [long long], [Signed 64 bit])
  else
    AC_MSG_ERROR([compiler has no 8 byte integer])
  fi
fi
if test "$ac_cv_sizeof_unsigned_long" -eq 8; then
  AC_DEFINE(UINT64_TYPE, [unsigned long], [Unsigned 32 bit])
else
  AC_CHECK_SIZEOF(unsigned long long)
  if test "$ac_cv_sizeof_unsigned_long_long" -eq 8; then
    AC_DEFINE(HAVE_UNSIGNED_LONG_LONG, 1, [Have the unsigned long long type])
    AC_DEFINE(UINT64_TYPE, [unsigned long long], [Unsigned 32 bit])
  else
    AC_MSG_ERROR([compiler has no 8 byte unsigned integer])
  fi
fi

AC_CHECK_SIZEOF(float)
if test "$ac_cv_sizeof_float" -eq 4; then
  AC_DEFINE(FLOAT4_TYPE, [float], [32 bit float])
else
  AC_MSG_ERROR([compiler has no 4 byte float])
fi

AC_CHECK_SIZEOF(double)
if test "$ac_cv_sizeof_double" -eq 8; then
  AC_DEFINE(DOUBLE8_TYPE, [double], [64 bit float])
else
  AC_MSG_ERROR([compiler has no 8 byte double])
fi

dnl Determine host system type
dnl AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(HOST, "$host", [The host system wmlcups was configured for])

dnl enable/disable compiler warnings (using gcc), if specified
AC_ARG_ENABLE(warnings,
  [  --disable-warnings      disable gcc warnings [default=no]],
  [case "${enableval}" in
    yes) wmlcups_warnings=yes ;; 
    no)  wmlcups_warnings=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-warnings) ;;
  esac], wmlcups_warnings=yes)
AC_MSG_CHECKING(whether to use gcc verbose warnings)
AC_MSG_RESULT($wmlcups_warnings)
if (test "$wmlcups_warnings" = yes && test "$GCC" = yes); then
  CXXFLAGS="-Wall $CXXFLAGS"
fi

dnl enable/disable debug logging, if specified
AC_ARG_ENABLE(debug-logging,
  [  --enable-debug-logging  enable debug logging [default=no]],
  [case "${enableval}" in
    yes) wmlcups_debug=yes ;;
    no)  wmlcups_debug=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug-logging) ;;
  esac], wmlcups_debug=no)
AC_MSG_CHECKING(whether to enable debug logging)
AC_MSG_RESULT($wmlcups_debug)
if test "$wmlcups_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Define if debug logging is enabled])
fi

dnl enable/disable disk space check
AC_ARG_ENABLE(disk-space-check,
  [  --enable-disk-space-check  enable disk space checking [default=yes]],
  [case "${enableval}" in
    yes) wmlcups_disk_space=yes ;;
    no)  wmlcups_disk_space=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-disk-space-check) ;;
  esac], wmlcups_disk_space=yes)
AC_MSG_CHECKING(whether to enable disk space checking)
AC_MSG_RESULT($wmlcups_disk_space)
if test "$wmlcups_disk_space" = yes; then
  AC_DEFINE(CHECK_DISK_SPACE_FEATURE, 1, [Define if disk space checking is enabled])
fi

dnl enable/disable queue enabled check
AC_ARG_ENABLE(queue-enabled-check,
  [  --enable-queue-enabled-check  enable queue enabled checking [default=yes]],
  [case "${enableval}" in
    yes) wmlcups_queue_enabled=yes ;;
    no)  wmlcups_queue_enabled=no ;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-queue-enabled-check) ;;
  esac], wmlcups_queue_enabled=yes)
AC_MSG_CHECKING(whether to enable queue enabled checking)
AC_MSG_RESULT($wmlcups_queue_enabled)
if test "$wmlcups_queue_enabled" = yes; then
  AC_DEFINE(CHECK_QUEUE_IS_ENABLED_FEATURE, 1, [Define if queue enabled checking is enabled])
fi

dnl Always output certain files
AC_CONFIG_FILES([Makefile wmlcups/Makefile doc/Makefile doc/Doxyfile])

AC_OUTPUT
