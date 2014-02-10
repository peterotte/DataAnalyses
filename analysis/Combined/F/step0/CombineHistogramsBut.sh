#!/bin/bash

#Sep and Feb But beamtimes
hadd -f output/sumButP.root ../../step0/output/sumButD.root ../../step0/output/sumButE.root ../../step0/output/sumButF.root
hadd -f output/sumButM.root ../../step0/output/sumButC.root ../../step0/output/sumButG.root ../../step0/output/sumButH.root

#DEBUG: Sep only But beamtimes
#hadd -f output/sumButP.root ../../step0/output/sumButD.root 
#hadd -f output/sumButM.root ../../step0/output/sumButC.root

#DEBUG: Feb only But beamtime
#hadd -f output/sumButP.root ../../step0/output/sumButE.root ../../step0/output/sumButF.root
#hadd -f output/sumButM.root ../../step0/output/sumButG.root ../../step0/output/sumButH.root


root -l -b -q CombineButPMScaled.C

hadd -f output/sumBut.root output/sumButPScaled.root output/sumButM.root