#!/bin/bash


./CombineHistogramsButFeb.sh
./CombineHistogramsButSep.sh
./CombineHistogramsH.sh

# Requires ButFeb and ButSep root files
./CombineHistogramsBut.sh
