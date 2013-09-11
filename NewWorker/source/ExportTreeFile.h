#ifndef EXPORTTREEFILE_H
#define EXPORTTREEFILE_H

#include <TFile.h>
#include <TTree.h>

TFile *MyRootTreeFile;
TTree *MyTree;

Double_t PMesonMass;

Int_t MyTreeNTaggerHits = 0;

Double_t MyTreeTaggerE[120];
Int_t MyTreeTaggerCh[120];
Double_t MyTreeTaggerMesonLabTimeDiff[120];
Double_t MyTreeMesonCMTh[120];

Double_t MyTreeMesonLabTh;
Double_t MyTreeMesonLabPh;
Double_t MyTreeMesonLabE;
Double_t MyTreeMesonLabM;

Double_t MyTreeLabMissingM[120];

Int_t MyTreeBeamPolBit; //Bit 0 = BeamPolBit
Int_t MyTreeTriggerPat;



#endif // EXPORTTREEFILE_H
