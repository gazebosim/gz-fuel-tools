#!/bin/sh -l

set -x

# Install
make install

# So ign-tools finds fuel-tools
export GZ_CONFIG_PATH=/usr/local/share/ignition
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH

# Compile example
cd ../example
mkdir build
cd build
cmake ..
make
cd ../../build
