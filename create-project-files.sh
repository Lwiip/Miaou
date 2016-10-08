#!/usr/bin/env bash
#this file creates supports files for your project (makefile you can use with make and eclipse project file you can use with eclipse CDT http://www.eclipse.org/cdt/)
#those supports files were created and tested with cmake 2.8 and eclipse cdt luna
rm -rf build
mkdir build
cd build
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../
make

cd ..

function client {
    ./build/jalon0$1/RE216_JALON0$1_CLIENT 127.0.0.1 3310
}

function serveur {
    ./build/jalon0$1/RE216_JALON0$1_SERVER 3310
}
