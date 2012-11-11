#!/bin/bash
g++ -O3 \
    -march=native \
    -msse4.1 \
    -msse4.2 \
    timer.cpp test.cpp \
    -o do_timings `pkg-config opencv --cflags --libs`
