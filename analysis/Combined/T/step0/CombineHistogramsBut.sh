#!/bin/bash

hadd -f output/sumButP.root ../../step0/output/sumButA.root ../../step0/output/sumButD.root ../../step0/output/sumButE.root ../../step0/output/sumButF.root
hadd -f output/sumButM.root ../../step0/output/sumButB.root ../../step0/output/sumButC.root ../../step0/output/sumButG.root ../../step0/output/sumButH.root

root -l -b -q CombineButPMScaled.C

hadd -f output/sumBut.root output/sumButPScaled.root output/sumButM.root
#hadd -f output/sumBut.root output/sumButP.root output/sumButM.root