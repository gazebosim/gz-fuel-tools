#!/bin/sh -l

set -x

# Install
make install

# So ign-tools finds fuel-tools
export IGN_CONFIG_PATH=/usr/local/share/ignition

# Compile example
cd ../example
mkdir build
cd build
cmake ..
make
cd ../../build
