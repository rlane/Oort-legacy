AC_INIT([Oort], [0.1.0], [rlane@club.cc.cmu.edu], [oort], [http://github.com/rlane/Oort])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([no-dist-gzip dist-bzip2])
#m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB

AM_PROG_VALAC([0.12.0])

# Set up prefix-relative directories
if test "x${prefix}" = "xNONE"; then
  PREFIX="${ac_default_prefix}"
else
  PREFIX="${prefix}"
fi
AC_DEFINE_UNQUOTED(PACKAGE_DATADIR, "${PREFIX}/share/${PACKAGE}",[Package data directory])
AC_DEFINE_UNQUOTED(PACKAGE_BINDIR, "${PREFIX}/bin",[Package executable directory])

CFLAGS="-ggdb -m32 -Wall -Wno-unused-but-set-variable -Wno-pointer-sign -Wno-missing-braces -Wno-unused-function"
LDFLAGS="-m32"

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

PKG_PROG_PKG_CONFIG

GLIB_REQUIRED=2.12.0
GTK_REQUIRED=2.10.0
GTKGLEXT_REQUIRED=1.2
GLEW_REQUIRED=1.5.2

PKG_CHECK_MODULES(OORT, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED)
#PKG_CHECK_MODULES(OORT_UI, gtk+-2.0 >= $GTK_REQUIRED gtkglext-1.0 >= $GTKGLEXT_REQUIRED glew >= $GLEW_REQUIRED)
AC_SUBST(OORT_CFLAGS)
AC_SUBST(OORT_LIBS)
AC_SUBST(OORT_UI_CFLAGS)
AC_SUBST(OORT_UI_LIBS)

AC_ARG_ENABLE(standalone, Create standalone executables)
if test "x${enable_standalone}" = "xyes"; then
	OORT_EXTRA_LDFLAGS="-all-static"
else
	OORT_EXTRA_LDFLAGS=
fi
AC_SUBST(OORT_EXTRA_LDFLAGS)

AC_ARG_WITH(valgrind, Use valgrind)
if test "x${with_valgrind}" = "xyes"; then
	PKG_CHECK_MODULES(VALGRIND, valgrind)
	VALGRIND_CFLAGS="$VALGRIND_CFLAGS -DWITH_VALGRIND"
fi
AC_SUBST(VALGRIND_CFLAGS)
AC_SUBST(VALGRIND_LIBS)

AM_CONDITIONAL([MINGW32], [test x"${with_mingw32}" = "xyes"])

AC_CONFIG_FILES([
	Makefile
	sim/Makefile
	dedicated/Makefile
	math3d/Makefile
	vector/Makefile
	vapi/Makefile
	test/Makefile
	cjson/Makefile
	lua_build/Makefile
])

AC_OUTPUT
