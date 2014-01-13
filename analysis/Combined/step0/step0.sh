#!/bin/bash


./CombineHistogramsButSingle.sh

./CombineHistogramsH_FT.sh
./CombineHistogramsH_ET.sh

root /datapool/home/otte/NewAnalysis/analysis/Combined/step0/H_ET_Subtraction.C


# Requires ButFeb and ButSep root files
./CombineHistogramsBut.sh
