#!/bin/sh -l

set -x

# Install
make install

# Compile examples
cd ../example
mkdir build
cd build
cmake ..
make
