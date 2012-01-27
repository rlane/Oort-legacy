AC_INIT([Oort], [0.1.0], [rlane@club.cc.cmu.edu], [oort], [http://github.com/rlane/Oort])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([no-dist-gzip dist-bzip2 subdir-objects color-tests silent-rules])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
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
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG(C++)

CXXFLAGS="-std=c++0x -g -O1 -Wall"
LDFLAGS="-rdynamic -ldl"

AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

GLM_CFLAGS='-I $(top_srcdir)/third_party/glm'
AC_SUBST(GLM_CFLAGS)

JSON_CFLAGS='-I $(top_srcdir)/third_party/json-spirit/json_spirit'
AC_SUBST(JSON_CFLAGS)

BOX2D_CFLAGS=''
BOX2D_LIBS='-lBox2D'
AC_SUBST(BOX2D_CFLAGS)
AC_SUBST(BOX2D_LIBS)

PKG_PROG_PKG_CONFIG

SDL_REQUIRED=1.2.14
GLEW_REQUIRED=1.5.2
LUA_REQUIRED=5.1.4

PKG_CHECK_MODULES(SDL, sdl >= $SDL_REQUIRED)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

BOOST_LIBS=-lboost_thread
AC_SUBST(BOOST_LIBS)

if test "x${nacl}" = "xno"; then
	PKG_CHECK_MODULES(GLEW, glew >= $GLEW_REQUIRED)
	AC_SUBST(GLEW_CFLAGS)
	AC_SUBST(GLEW_LIBS)

	FRONTENDS="oort_headless oort_sdl"
else
	NACL_CFLAGS=""
	NACL_LIBS="-lppapi -lppapi_cpp -lppapi_gles2 -lnosys"
	AC_SUBST(NACL_CFLAGS)
	AC_SUBST(NACL_LIBS)
	AC_PATH_PROG([RUBY],[ruby])

	FRONTENDS="oort_nacl"
fi
AC_SUBST(FRONTENDS)

PKG_CHECK_MODULES(LUA, lua >= $LUA_REQUIRED)
AC_SUBST(LUA_CFLAGS)
AC_SUBST(LUA_LIBS)

AC_CHECK_HEADER("Box2D/Box2D.h")	

AC_MSG_CHECKING([b2World constructor arguments])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <Box2D/Box2D.h>
void check() {
	b2World(b2Vec2(0,0), false);
}
]])], [b2WorldConstructor=old])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <Box2D/Box2D.h>
void check() {
	b2World(b2Vec2(0,0));
}
]])], [b2WorldConstructor=new])
AC_MSG_RESULT([$b2WorldConstructor])
if [[ "$b2WorldConstructor" = "old" ]]; then
	AC_DEFINE(B2WORLD_OLD_CONSTRUCTOR, [], [Old version of b2World constructor])
fi

AC_CONFIG_FILES([
	Makefile
])

AC_OUTPUT
