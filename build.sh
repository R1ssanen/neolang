#! usr/sbin/bash

cmake -GNinja -S . -B build/
cd build/
ninja
cd ../

