#!/bin/bash
g++ -O3 \
    -march=native \
    -msse4.1 \
    -msse4.2 \
    src/lucid/tools/*.cpp \
    src/lucid/detectors/*.cpp \
    src/lucid/descriptors/*.cpp \
    -o do_experiments `pkg-config opencv --cflags --libs` -I./include
