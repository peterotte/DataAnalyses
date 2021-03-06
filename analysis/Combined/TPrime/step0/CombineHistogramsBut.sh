#!/bin/bash

#Sep and Feb But beamtimes
hadd -f output/sumButP.root ../../step0/output/sumButD.root ../../step0/output/sumButE.root ../../step0/output/sumButF.root
hadd -f output/sumButM.root ../../step0/output/sumButC.root ../../step0/output/sumButG.root ../../step0/output/sumButH.root

#DEBUG: Sep only But beamtimes
#hadd -f output/sumButP.root ../../step0/output/sumButA.root ../../step0/output/sumButD.root
#hadd -f output/sumButM.root ../../step0/output/sumButB.root ../../step0/output/sumButC.root

#DEBUG: Feb only But beamtimes
#hadd -f output/sumButP.root ../../step0/output/sumButE.root ../../step0/output/sumButF.root
#hadd -f output/sumButM.root ../../step0/output/sumButG.root ../../step0/output/sumButH.root

#DEBUG: Jan 2011 beamtimes with carbon
#hadd -f output/sumButP.root ../../step0/output/sumButI.root
#hadd -f output/sumButM.root ../../step0/output/sumButJ.root

root -l -b -q CombineButPMScaled.C

hadd -f output/sumBut.root output/sumButPScaled.root output/sumButM.root
#hadd -f output/sumBut.root output/sumButP.root output/sumButM.root