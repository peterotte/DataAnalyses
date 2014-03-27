#!/bin/bash

./CombineHistogramsBut.sh

root -l -b -q RescaleH.C

root -l -b -q ScalerFPosBinDebug.C

