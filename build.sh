#! bin/bash

cmake -GNinja -S . -B build/
cmake --build build/
