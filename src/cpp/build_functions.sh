#! /bin/bash
g++ lucid_functions.cpp -msse4.2 -march=native -O3 `pkg-config opencv --cflags --libs`

