#!/bin/bash

cd /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output

hadd -f sumButP.root sumButA.root sumButC.root sumButD.root sumButE.root  
hadd -f sumButM.root sumButB.root sumButF.root sumButG.root

cd /datapool/home/otte/NewAnalysis/analysis/Combined/step0

root CombineButPMScaled.C

cd /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output

#hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumBut.root sumButP.root sumButM.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumBut.root sumButPScaled.root sumButM.root