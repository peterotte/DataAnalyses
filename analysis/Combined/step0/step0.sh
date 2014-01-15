#!/bin/bash


./CombineHistogramsButSingle.sh

./CombineHistogramsH_FT.sh
./CombineHistogramsH_ET.sh

root H_ET_Subtraction.C

./CombineHistogramsBut.sh

root RescaleH.C