#!/bin/bash

./CombineHistogramsButSingle.sh

./CombineHistogramsH_FT.sh
./CombineHistogramsH_ET.sh

root -l -b -q H_ET_Subtraction.C
