#ifndef __worker_h_INCLUDED__
#define __worker_h_INCLUDED__

Char_t InputPathDataDir[256] = "/home/otte/NewAnalysis/data";


int ProgramBatchMode = 0; //if 0, it runs interactively, else, it runs in batch mode
int ProgramDebugMode = 0; //if 0, it runs not in debug mode

Char_t TaggEffInputPath[1024] = ""; //Actual path to root files, call ReadTaggEffConfigurationFile before
Char_t TaggEffOutputPath[1024] = "";
Double_t TaggEffTaggerClockSpeed = -1; //How many ticks per second, set in TaggEff.dat

Double_t OptionalRatioScale = 1; //Scale to match Ratio to taggeff measurements, to be determined via FindOptionalScale()
const Int_t NTaggChs = 352;
const double MassPion0 = 134.9766;




TH1D *hMesonMassCenterVSFiles, *hMesonMassWidthVSFiles;

TH1D *hDroppedEventsRatio;
TH1D *hTotalEvents;


#endif
