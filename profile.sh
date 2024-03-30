#!/bin/bash

PROFILE_DIR=profiles

AVAILABLE_SUTS=$(.build/1brc --list | grep "1E5")
SUTS=${1:-$AVAILABLE_SUTS}

for sut in $SUTS; do
    valgrind --tool=callgrind \
        --dump-instr=yes \
        --cache-sim=yes \
        --collect-jumps=yes \
        --collect-bus=yes \
        --collect-systime=yes \
        --branch-sim=yes \
        --callgrind-out-file="$PROFILE_DIR/$sut.callgrind.out" \
        --log-file="$PROFILE_DIR/$sut.log" \
        .build/1brc-O3 -q "$sut"
    echo "Profiled $sut"
done
