#!/usr/bin/zsh

mkdir build
cd build
cmake ..
make -j${nproc}
