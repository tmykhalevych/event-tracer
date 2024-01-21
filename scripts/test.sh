#!/bin/bash

current_dir="$(pwd)"
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
build_dir="$script_dir/../build"

cd $build_dir
ctest

cd $current_dir
