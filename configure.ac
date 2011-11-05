AC_INIT([Oort], [0.1.0], [rlane@club.cc.cmu.edu], [oort], [http://github.com/rlane/Oort])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([no-dist-gzip dist-bzip2])
#m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

CXXFLAGS="-std=c++0x -g -O1 -Wall"
LDFLAGS=""

AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

GLM_CFLAGS='-I $(top_srcdir)/third_party/glm'
AC_SUBST(GLM_CFLAGS)

PKG_PROG_PKG_CONFIG

SDL_REQUIRED=1.2.14
GLEW_REQUIRED=1.5.2
LUA_REQUIRED=5.1.4

PKG_CHECK_MODULES(SDL, sdl >= $SDL_REQUIRED)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

PKG_CHECK_MODULES(GLEW, glew >= $GLEW_REQUIRED)
AC_SUBST(GLEW_CFLAGS)
AC_SUBST(GLEW_LIBS)

PKG_CHECK_MODULES(LUA, lua >= $LUA_REQUIRED)
AC_SUBST(LUA_CFLAGS)
AC_SUBST(LUA_LIBS)

AC_CONFIG_FILES([
	Makefile
	common/Makefile
	sim/Makefile
	renderer/Makefile
	gl/Makefile
	ui/headless/Makefile
	ui/sdl/Makefile
	third_party/Makefile
])

AC_OUTPUT
