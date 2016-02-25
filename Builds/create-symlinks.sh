#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: `basename $0` LOCAL_LIB_DIR LOCAL_INCLUDE_DIR"
    exit 2
fi

LOCAL_LIB_DIR="$1"
LOCAL_INCLUDE_DIR="$2"

if [[ ! -d $LOCAL_LIB_DIR ]]; then
    mkdir -p "$LOCAL_LIB_DIR"
fi

if [[ ! -d $LOCAL_INCLUDE_DIR ]]; then
    mkdir -p "$LOCAL_INCLUDE_DIR"
fi

targetpath="/usr/lib/x86_64-linux-gnu/libOpenCL.so.1"
linkpath="$LOCAL_LIB_DIR/libOpenCL.so"

if [[ ! -e $linkpath && -e $targetpath ]]; then
    ln -s "$targetpath" "$linkpath"
fi

targetpath=""

if [ -d /usr/include/*/GL ]; then
    targetpath=$(ls -d1 /usr/include/*/GL | head -n 1)
elif [ -d /usr/include/GL ]; then
    targetpath="/usr/include/GL"
fi

linkpath="$LOCAL_INCLUDE_DIR/GL"

if [[ ! -e $linkpath && -e $targetpath ]]; then
    ln -s "$targetpath" "$linkpath"
fi

targetpath=""

if [ -d /usr/include/*/CL ]; then
    targetpath=$(ls -d1 /usr/include/*/CL | head -n 1)
elif [ -d /usr/include/CL ]; then
    targetpath="/usr/include/CL"
fi

linkpath="$LOCAL_INCLUDE_DIR/CL"

if [[ ! -e $linkpath && -e $targetpath ]]; then
    ln -s "$targetpath" "$linkpath"
fi
