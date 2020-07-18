#!/bin/sh -l

set -x

# Install
make install

# Compile example
cd ../example
mkdir build
cd build
cmake ..
make
