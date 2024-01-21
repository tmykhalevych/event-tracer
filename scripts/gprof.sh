#!/bin/bash

current_dir="$(pwd)"
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
build_dir="$script_dir/../build"
out_dir="$build_dir/gprof"
atrifacts_dir="$out_dir/atrifacts"

target="$current_dir/$1"
test_duration_sec=60

mkdir -p "$out_dir"
mkdir -p "$atrifacts_dir"

cd $atrifacts_dir

echo "Start testing $target for $test_duration_sec sec"

"$target" &
target_pid=$!
sleep $test_duration_sec
kill -SIGINT $target_pid

echo "Stop testing $target"

gprof "$target" gmon.out > gprof.txt
gprof2dot -f prof -o gprof.dot gprof.txt

report="$out_dir/gprof.png"
dot -Tpng -o "$report" "$atrifacts_dir/gprof.dot"

echo "Report ready: $report"

cd $current_dir
