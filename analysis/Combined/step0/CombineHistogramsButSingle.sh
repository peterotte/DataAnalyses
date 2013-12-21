#!/bin/bash

#export LD_LIBRARY_PATH=/cern/root/lib

hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButA.root /datapool/2010_09/outputA/*.dat.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButB.root /datapool/2010_09/outputB/*.dat.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButC.root /datapool/2010_09/outputC/*.dat.root

hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButD.root /datapool/2011_02/outputD/*.dat.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButE.root /datapool/2011_02/outputE/*.dat.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButF.root /datapool/2011_02/outputF/*.dat.root
hadd -f /datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButG.root /datapool/2011_02/outputG/*.dat.root
