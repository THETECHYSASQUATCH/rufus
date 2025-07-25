AC_INIT([rufus], [4.10], [https://github.com/pbatard/rufus/issues], [rufus], [https://rufus.ie])
AM_INIT_AUTOMAKE([-Wno-portability foreign no-dist no-dependencies])
AC_CONFIG_SRCDIR([src/rufus.c])
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])

AC_PREREQ([2.50])
AC_PROG_CC
# autoconf 2.61 doesn't have AC_PROG_AR, but 2.63 has it
AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
AC_PROG_AR
AC_PROG_RANLIB
AC_PROG_SED
AC_PATH_PROG(RM, rm, rm)
AC_CHECK_TOOL(DLLTOOL, dlltool, :)
AC_CHECK_TOOL(STRIP, strip, :)
AC_CHECK_TOOL(WINDRES, windres, :)
AC_C_INLINE
AC_DEFINE([_GNU_SOURCE], [], [Use GNU extensions])

# Platform detection
case "${host_os}" in
    *mingw*|*cygwin*|*msys*)
        platform_windows=yes
        AM_CFLAGS="${AM_CFLAGS} -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -D_WIN32_IE=0x0A00"
        # "-Wl,--nxcompat" to enable DEP (Data Execution Prevention)
        # "-Wl,--dynamicbase" to enable ASLR (Address Space Layout Randomization)
        AM_LDFLAGS="${AM_LDFLAGS} -Wl,-no-undefined -Wl,--nxcompat -Wl,--no-insert-timestamp -Wl,--dynamicbase"
        ;;
    *darwin*)
        platform_macos=yes
        ;;
    *linux*)
        platform_linux=yes
        ;;
    *)
        platform_other=yes
        ;;
esac

AM_CONDITIONAL([PLATFORM_WINDOWS], [test "x$platform_windows" = "xyes"])
AM_CONDITIONAL([PLATFORM_MACOS], [test "x$platform_macos" = "xyes"])
AM_CONDITIONAL([PLATFORM_LINUX], [test "x$platform_linux" = "xyes"])

# Debug symbols
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[keep debug symbols for gdb (default=yes)])],
	[debug_enabled=$enableval],
	[debug_enabled='yes'])
if test "x$debug_enabled" != "xno" ; then
  CFLAGS="-g -O0"
else
  CFLAGS="-Os"
  LDFLAGS="-s"
fi

# Alpha/Beta/Test
AC_ARG_ENABLE([alpha],[AS_HELP_STRING([--enable-alpha], [build an ALPHA release (default=no)])], [alpha_enabled=$enableval], [alpha_enabled='no'])
if test "x$alpha_enabled" != "xno" ; then
  CFLAGS+=" -DALPHA"
  SUFFIX=_ALPHA
fi
AC_ARG_ENABLE([beta],[AS_HELP_STRING([--enable-beta], [build a BETA release (default=no)])], [beta_enabled=$enableval], [beta_enabled='no'])
if test "x$beta_enabled" != "xno" ; then
  CFLAGS+=" -DBETA"
  SUFFIX=_BETA
fi
AC_ARG_ENABLE([test],[AS_HELP_STRING([--enable-test=#], [build a TEST release (default=no)])], [test_enabled=$enableval], [test_enabled='no'])
if test "x$test_enabled" != "xno" ; then
  if test "x$test_enabled" == "xyes" ; then $enableval="" ; fi
  CFLAGS+=" -DTEST=$enableval"
  SUFFIX=_TEST$enableval
fi

AC_MSG_RESULT([enabling Large File Support (ISO support)])
AM_CFLAGS="$AM_CFLAGS -D_FILE_OFFSET_BITS=64 -D_OFF_T_ -D_off_t=off64_t -Doff_t=off64_t -Doff32_t=long"

# Check for -Wno-pointer-sign compiler support (GCC >= 4)
saved_CFLAGS="${CFLAGS}"
CFLAGS="$CFLAGS -Wno-pointer-sign"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
	[nopointersign_cflags="-Wno-pointer-sign"], [nopointersign_cflags=""])
CFLAGS="${saved_CFLAGS}"

# NB: The DECLSPEC_IMPORT redefinition below is a temporary(?) workaround for MinGW32 delay-loading
# See https://github.com/pbatard/rufus/pull/2513 as well as https://sourceware.org/bugzilla/show_bug.cgi?id=14339
AM_CFLAGS="$AM_CFLAGS -DUNICODE -D_UNICODE -UNDEBUG -DCOBJMACROS -D__USE_MINGW_ANSI_STDIO=0 -UDECLSPEC_IMPORT -DDECLSPEC_IMPORT=__attribute__\(\(visibility\(\\\"hidden\\\"\)\)\) -std=gnu11 -Wshadow -Wall -Wformat-security -Wundef -Wunused -Wstrict-prototypes -Wno-restrict -Wno-array-bounds -Werror-implicit-function-declaration -Wbidi-chars=none $nopointersign_cflags"

AC_SUBST([VISIBILITY_CFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([SUFFIX])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([.mingw/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/bled/Makefile])
AC_CONFIG_FILES([src/ext2fs/Makefile])
AC_CONFIG_FILES([src/libcdio/iso9660/Makefile])
AC_CONFIG_FILES([src/libcdio/udf/Makefile])
AC_CONFIG_FILES([src/libcdio/driver/Makefile])
AC_CONFIG_FILES([res/loc/Makefile])
AC_CONFIG_FILES([src/ms-sys/Makefile])
AC_CONFIG_FILES([src/syslinux/libfat/Makefile])
AC_CONFIG_FILES([src/syslinux/libinstaller/Makefile])
AC_CONFIG_FILES([src/syslinux/win/Makefile])
AC_CONFIG_FILES([src/wimlib/Makefile])
AC_OUTPUT
