#!/bin/bash


##Select the right output diretories
cd /datapool/2010_09
./ChangeToWOPID.sh
cd /datapool/2011_02
./ChangeToWOPID.sh
cd /datapool/2013_05_pi0
./ChangeToWOPID.sh

cd /datapool/2010_09
./ChangeToWithPID.sh
cd /datapool/2011_02
./ChangeToWithPID.sh
cd /datapool/2013_05_pi0
./ChangeToWithPID.sh


##Delete all generated output
cd /datapool/home/otte/NewAnalysis/analysis/AnalyseRawData
./delete.sh

##Analyse Tagging Eff Files first
cd /datapool/home/otte/NewAnalysis/AR/bin
./Worker 1440 1549
## with Sven data file selection: ./Worker 1468 1577

##Calculate TaggEff Files
cd /datapool/home/otte/NewAnalysis/NewTaggEff/build
bin/NewWorker -b -r 0
bin/NewWorker -b -r 1
bin/NewWorker -b -r 2

##Analyse all Data files with correct TaggEff Files now
cd /datapool/home/otte/NewAnalysis/AR/bin
./Worker 0 1439
## with Sven data file selection ./Worker 0 1467
## with Sven data file selection and only Butanol ./Worker 0 1420


##Ggf. noch den CbEnergyScale für jede Datei berechnen
## Its results should be checked especially for Empty Target runs (= Scaling from before)
# cd /datapool/home/otte/NewAnalysis/CBEnergyFitter/build
# ./bin/CBEnergyFitter -b -r
## Falls dieser Schritt gemacht wird, so müssen nochmal alle Dateien analysiert werden
#cd /datapool/home/otte/NewAnalysis/analysis/AnalyseRawData
#./delete.sh
#cd /datapool/home/otte/NewAnalysis/AR/bin
#./Worker 0 1439



