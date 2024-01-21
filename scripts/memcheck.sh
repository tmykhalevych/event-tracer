#!/bin/bash

current_dir="$(pwd)"
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
build_dir="$script_dir/../build"
out_dir="$build_dir/memcheck"

target="$current_dir/$1"
test_duration_sec=60

mkdir -p "$out_dir"
cd $out_dir

echo "Start testing $target for $test_duration_sec sec"

report="$out_dir/memckeck.txt"

valgrind --leak-check=full "$target" > "$report" 2>&1 &
target_pid=$!
sleep $test_duration_sec
kill -SIGINT $target_pid

echo "Stop testing $target"
echo "Report ready: $report"

cd $current_dir
