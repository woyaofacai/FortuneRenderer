#!/bin/bash

if [ ! -d "./build" ]; then
    mkdir "./build"
fi


if [ -f "./build/CMakeCache.txt" ]; then
    rm -f "./build/CMakeCache.txt"
fi

cd ./build
#cmake .. -DCMAKE_BUILD_TYPE=Release
cmake ..


cd ..


