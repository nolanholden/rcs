#!/usr/bin/env bash

set -e

sudo apt-get install autoconf automake libtool curl make g++ unzip
cd protobuf/protobuf
git submodule update --init --recursive
./autogen.sh
./configure --prefix=/usr
make
make check
sudo make install
sudo ldconfig # refresh shared library cache.

