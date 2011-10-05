AC_INIT([Oort], [0.1.0], [rlane@club.cc.cmu.edu], [oort], [http://github.com/rlane/Oort])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([no-dist-gzip dist-bzip2])
#m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE

AC_CANONICAL_HOST

AC_MSG_CHECKING([for NaCl])
case $host in
  *-*-nacl*)
    nacl="yes"
    ;;
  *)
    nacl="no"
    ;;
esac
AC_MSG_RESULT([$nacl])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB

AM_PROG_VALAC([0.12.0])

AC_ARG_ENABLE(luajit,
              AC_HELP_STRING([--enable-luajit=@<:@no/yes@:>@],
                             [use luajit @<:@default=yes@:>@]),,
              enable_luajit=yes)
if test "x${enable_luajit}" = "xyes"; then
	LUA_SUBDIR='third_party/luajit'
	LUA_LDADD='$(LUA_DIR)/src/libluajit.a'
	LUA_CFLAGS='-I $(LUA_DIR)/src'
else
	LUA_SUBDIR='third_party/lua'
	LUA_LDADD='$(LUA_DIR)/src/liblua.a'
	LUA_CFLAGS='-I $(LUA_DIR)/src'
fi
LUA_DIR='$(top_srcdir)/$(LUA_SUBDIR)'
LUA_BUILD_SUBDIR='$(LUA_SUBDIR)_build'
AC_SUBST(LUA_SUBDIR)
AC_SUBST(LUA_BUILD_SUBDIR)
AC_SUBST(LUA_DIR)
AC_SUBST(LUA_LDADD)
AC_SUBST(LUA_CFLAGS)

CJSON_DIR='$(top_srcdir)/third_party/cjson'
AC_SUBST(CJSON_DIR)
CJSON_LDADD='$(CJSON_DIR)/libcjson.a'
AC_SUBST(CJSON_LDADD)
CJSON_CFLAGS='-I $(CJSON_DIR)'
AC_SUBST(CJSON_CFLAGS)

# Set up prefix-relative directories
if test "x${prefix}" = "xNONE"; then
  PREFIX="${ac_default_prefix}"
else
  PREFIX="${prefix}"
fi
AC_DEFINE_UNQUOTED(PACKAGE_DATADIR, "${PREFIX}/share/${PACKAGE}",[Package data directory])
AC_DEFINE_UNQUOTED(PACKAGE_BINDIR, "${PREFIX}/bin",[Package executable directory])

CFLAGS="-std=gnu99 -ggdb -Wall -Wno-unused-but-set-variable -Wno-pointer-sign -Wno-missing-braces -Wno-unused-function"
LDFLAGS=""

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

PKG_PROG_PKG_CONFIG

GLIB_REQUIRED=2.12.0
GTK_REQUIRED=2.10.0
GTKGLEXT_REQUIRED=1.2
GLEW_REQUIRED=1.5.2

if test "x${nacl}" = "xyes"; then
	AC_PATH_PROG([RUBY],[ruby])
	PKG_CHECK_MODULES(OORT, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)
else
	PKG_CHECK_MODULES(OORT, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED)
	PKG_CHECK_MODULES(OORT_UI, gtk+-2.0 >= $GTK_REQUIRED gtkglext-1.0 >= $GTKGLEXT_REQUIRED glew >= $GLEW_REQUIRED)
fi
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

if test "x${nacl}" = "xyes"; then
	FRONTENDS="nacl"
else
	FRONTENDS="dedicated ui test"
fi
AC_SUBST(FRONTENDS)

AM_CONDITIONAL([MINGW32], [test x"${with_mingw32}" = "xyes"])
AM_CONDITIONAL([NACL], [test x"${nacl}" = "xyes"])

AC_CONFIG_FILES([
	Makefile
	sim/Makefile
	renderer/Makefile
	dedicated/Makefile
	ui/Makefile
	nacl/Makefile
	math3d/Makefile
	vector/Makefile
	vapi/Makefile
	test/Makefile
	third_party/cjson/Makefile
	third_party/luajit_build/Makefile
	third_party/lua_build/Makefile
])

AC_OUTPUT
