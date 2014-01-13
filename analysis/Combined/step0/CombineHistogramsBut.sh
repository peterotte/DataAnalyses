#!/bin/bash

#export LD_LIBRARY_PATH=/cern/root/lib

cd /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output

hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButP.root sumButA.root sumButC.root sumButD.root sumButE.root  
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButM.root sumButB.root sumButF.root sumButG.root

root /datapool/home/otte/NewAnalysis/analysis/Combined/step0/CombineButPMScaled.C

#hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumBut.root sumButP.root sumButM.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumBut.root sumButPScaled.root sumButM.root