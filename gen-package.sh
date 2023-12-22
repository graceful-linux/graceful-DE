#!/bin/bash
set -e

versionMajor=0
versionMinor=0
versionPatch=0
versionTweak=0
version="${versionMajor}.${versionMinor}.${versionPatch}.${versionTweak}"

curDir=$(dirname $(realpath -- $0))
workDir=${curDir}/out/

[[ ! -d "${workDir}" ]] && mkdir -p "${workDir}"

sed -i -E "s/^set\(PROJECT_VERSION_MAJOR\ [0-9]+\)$/set\(PROJECT_VERSION_MAJOR\ ${versionMajor}\)/" \
    ${curDir}/CMakeLists.txt 
sed -i -E "s/^set\(PROJECT_VERSION_MINOR\ [0-9]+\)$/set\(PROJECT_VERSION_MINOR\ ${versionMinor}\)/" \
    ${curDir}/CMakeLists.txt
sed -i -E "s/^set\(PROJECT_VERSION_PATCH\ [0-9]+\)$/set\(PROJECT_VERSION_PATCH\ ${versionPatch}\)/" \
    ${curDir}/CMakeLists.txt
sed -i -E "s/^set\(PROJECT_VERSION_TWEAK\ [0-9]+\)$/set\(PROJECT_VERSION_TWEAK\ ${versionTweak}\)/" \
    ${curDir}/CMakeLists.txt


#tar cf ${workDir}/graceful-DE-${}

cat << EOF > ${workDir}/PKGBUILD
# Maintainer: dingjing <dingjing@live.cn>

pkgname=graceful-DE
pkgver=1.0.0
pkgrel=1
pkgdesc='An ultra-lightweight tiled desktop environment.'
url='https://github.com/graceful-linux/graceful-DE'
arch=('x86_64')
license=('MIT')
groups=('graceful-linux')
depends=('libxft' 'libx11' 'yajl' 'libxinerama' 'fontconfig') 
makedepends=('cmake' 'gcc')
source=("https://github.com/graceful-linux/graceful-greeter/archive/refs/tags/$pkgver.tar.gz")
sha512sums=('5eca42a2ae551ffba3e9c2005ec977e31a55fdc215da51ec1947cc83e45ef16642c841452bdc844612a7d1eacaecd1aa367feff22f9cc64c62c3bb4194efd1fd')

prepare() {
#mv $pkgver "graceful-greeter-$pkgver"
  cd graceful-greeter-$pkgver && mkdir build && cd build 
  cmake ..
}

build() {
  cd graceful-greeter-$pkgver/build
  make
}

package() {
  cd $pkgname-$pkgver/build
  make DESTDIR="$pkgdir" install
}
EOF
