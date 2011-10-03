#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir
PROJECT=Oort
TEST_TYPE=-f
FILE=runtime.lua

DIE=0

have_libtool=false
if libtoolize --version < /dev/null > /dev/null 2>&1 ; then
	libtool_version=`libtoolize --version |
			 head -1 |
			 sed -e 's/^\(.*\)([^)]*)\(.*\)$/\1\2/g' \
			     -e 's/^[^0-9]*\([0-9.][0-9.]*\).*/\1/'`
	case $libtool_version in
	    2.2*|2.4*)
		have_libtool=true
		;;
	esac
fi
if $have_libtool ; then : ; else
	echo
	echo "You must have libtool >= 2.2 installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/libtool/"
	DIE=1
fi

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/autoconf/"
	DIE=1
}

if automake-1.11 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.11
    ACLOCAL=aclocal-1.11
else if automake-1.10 --version < /dev/null > /dev/null 2>&1 ; then
    AUTOMAKE=automake-1.10
    ACLOCAL=aclocal-1.10
else
	echo
	echo "You must have automake 1.10.x or 1.11.x installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/automake/"
	DIE=1
fi
fi

if test "$DIE" -eq 1; then
	exit 1
fi

test $TEST_TYPE $FILE || {
	echo "You must run this script in the top-level $PROJECT directory"
	exit 1
}

rm -rf autom4te.cache

# README and INSTALL are required by automake, but may be deleted by clean
# up rules. to get automake to work, simply touch these here, they will be
# regenerated from their corresponding *.in files by ./configure anyway.
touch README INSTALL

$ACLOCAL $ACLOCAL_FLAGS || exit $?

libtoolize --force || exit $?

autoheader || exit $?

$AUTOMAKE --add-missing || exit $?
autoconf || exit $?
