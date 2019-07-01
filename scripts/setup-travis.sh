#!/bin/bash

set -x

cd $TRAVIS_BUILD_DIR
export CMAKE_BUILD_TYPE="Debug"

if [[ "$CMAKE_GENERATOR" == "" ]]; then
    export CMAKE_GENERATOR="Unix Makefiles"
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    if [[ "$TRAVIS_COMPILER" == "gcc" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi
fi

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    export PATH="/c/Program Files/CMake/bin":$PATH

    if [[ "$CMAKE_GENERATOR" == "Visual Studio"* ]]; then
        choco install -y vcbuildtools
        export PATH=$PATH:"/c/Program Files (x86)/Microsoft Visual Studio/2015/BuildTools/MSBuild/14.0/Bin"
        export BUILD_ARGS="-verbosity:minimal -m:2"
    fi

    if [[ "$CMAKE_GENERATOR" == "MinGW Makefiles" ]]; then
        export CMAKE_BUILD_TYPE="Release"
        export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_SH=CMAKE_SH-NOTFOUND"
        export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=release"

        # Remove sh.exe from git in the PATH for MinGW generator
        # 1) add extra ':' to allow the REMOVE below to work
        TMP_PATH=:$PATH:
        # 2) remove sh.exe from the PATH
        REMOVE='/c/Program Files/Git/usr/bin'
        TMP_PATH=${TMP_PATH/:$REMOVE:/:}
        # 3) remove extra ':'
        TMP_PATH=${TMP_PATH%:}
        TMP_PATH=${TMP_PATH#:}
        export PATH=$TMP_PATH
    fi
fi

if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi

export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"


set +x
