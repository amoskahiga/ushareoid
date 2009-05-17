#!/bin/sh

mkdir build
cd build || exit 1
cmake ../ -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix`
make || exit 1
sudo make install || exit 1
kbuildsycoca4
