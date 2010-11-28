# Maintainer: Rich Lane <rlane@club.cc.cmu.edu>

pkgname=risc-git
pkgver=20101128
pkgrel=1
pkgdesc="A space fleet programming game"
arch=('i686' 'x86_64')
url="https://github.com/rlane/risc"
license=("GPL")
depends=('glib2' 'lua' 'gtk2' 'gtkglext' 'glew' 'sdl_gfx')
makedepends=('pkgconfig')

_gitroot="git://github.com/rlane/risc"
_gitname="risc"

build() {
  cd "$srcdir"

  msg "Connecting to GIT server...."

  if [[ -d $_gitname ]] ; then
    cd $_gitname && git pull origin
    msg "The local files are updated."
  else
    git clone $_gitroot $_gitname
  fi

  msg "GIT checkout done or server timeout"
  msg "Starting make..."

  rm -rf "$srcdir/$_gitname-build"
  git clone "$srcdir/$_gitname" "$srcdir/$_gitname-build"
  cd "$srcdir/$_gitname-build"

  make
}

package() {
  cd "$srcdir/$_gitname-build"
  make DESTDIR="$pkgdir" install
}
