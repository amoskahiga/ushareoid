#!/bin/sh

mkdir build
cd build || exit 1
cmake ../ -DCMAKE_INSTALL_PREFIX=~/.kde4
make || exit 1
make install || exit 1
kbuildsycoca4
