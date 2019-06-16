#!/bin/bash

if [ ! -d "./build" ]; then 
    mkdir build 
fi

pushd build
clang ../source/data_desk_main.c -DBUILD_LINUX=1 -DBUILD_WIN32=0 -o ./data_desk -ldl
popd
