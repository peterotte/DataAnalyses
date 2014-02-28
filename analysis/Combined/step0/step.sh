#!/bin/bash

### But-Runs
./CombineHistogramsButSingle.sh

### H-Runs
#Full Target Runs
hadd -f output/sumH_FT.root /datapool/2013_05_pi0/outputFT/*.root
#Empty Target Runs
hadd -f output/sumH_ET.root /datapool/2013_05_pi0/outputET/*.root
#Do the empty Target substraction
root -l -b -q H_ET_Subtraction.C
