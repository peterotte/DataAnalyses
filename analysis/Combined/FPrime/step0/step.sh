#!/bin/bash

./CombineHistogramsBut.sh

#Debug: To Match the Ratio of Photons from But and H beamtimes
#root -l -b -q RescaleH.C

#Debug
#root -l -b -q ScalerFPosBinDebug.C

#Debug: Calculate sumBut.root and sumH.root with PhotonFluxCorrected to 1 Ref TaggCh
root -l -b -q SetButToUse1TaggScBut.C
root -l -b -q SetButToUse1TaggScH.C