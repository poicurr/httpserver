#!/bin/bash

PROJECT_ROOT_DIR=$(pwd)
BUILD_DIR=$PROJECT_ROOT_DIR/build/

function build {
  if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
  fi
  cd $BUILD_DIR
  cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
}

build

