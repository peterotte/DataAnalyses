#include <ExportTreeFile.h>
#include <TLorentzVector.h>

/*
int InitialiseExportTreeFile() {

    Char_t name[256];
    strcpy(name, "/exports/RAID0_a2datastore1/a2cb/2013_04_UnpolPi0/output2/");
    strcat(name, RawDataFileNameOnly);
    Char_t* n1 = strrchr(name, '.');
    *n1 = '\0';  //deletes the ".rd0" or ".dat" part
    strcat(name, ".root");

    printf("MyTree dedicated file: %s\n", name);


    MyRootTreeFile = new TFile(name,"RECREATE");
    printf("Created MyRootTreeFile\n");
    MyTree = new TTree("MyTree","Event Tree");
    printf("Created MyTree\n");
    printf("MyTree saved to %s\n", gROOT->GetFile()->GetName());


    //P Start
    MyTree->Branch("PMesonMass",&PMesonMass,"PMesonMass/D");

    MyTree->Branch("NTaggerHits",&MyTreeNTaggerHits,"MyTreeNTaggerHits/I");
    MyTree->Branch("TaggerE",MyTreeTaggerE,"MyTreeTaggerE[MyTreeNTaggerHits]/D");
    MyTree->Branch("TaggerCh",MyTreeTaggerCh,"MyTreeTaggerCh[MyTreeNTaggerHits]/I");
    MyTree->Branch("TaggerMesonLabTimeDiff",MyTreeTaggerMesonLabTimeDiff,"MyTreeTaggerMesonLabTimeDiff[MyTreeNTaggerHits]/D");
    MyTree->Branch("MesonCMTh",MyTreeMesonCMTh,"MyTreeMesonCMTh[MyTreeNTaggerHits]/D");

    MyTree->Branch("MesonLabTh",&MyTreeMesonLabTh,"MyTreeMesonLabTh/D");
    MyTree->Branch("MesonLabPh",&MyTreeMesonLabPh,"MyTreeMesonLabPh/D");
    MyTree->Branch("MesonLabE",&MyTreeMesonLabE,"MyTreeMesonLabE/D");

    MyTree->Branch("LabMissingM",MyTreeLabMissingM,"MyTreeLabMissingM[MyTreeNTaggerHits]/D");
    MyTree->Branch("BeamPolBit",&MyTreeBeamPolBit,"MyTreeBeamPolBit/I");
    MyTree->Branch("TriggerPat",&MyTreeTriggerPat,"MyTreeTriggerPat/I");

    printf("Added Branches to MyTree.\n");
}

int CloseExportTreeFile() {

    MyRootTreeFile->Write();
    MyRootTreeFile->Close();

}

int TreeInitEvent() {
    MyTreeNTaggerHits = 0;
}

int TreeFillMeson(TLorentzVector &fMeson) {
    PMesonMass = fMeson.M(); //For Tree File

    MyTreeMesonLabTh = fMeson.Theta();
    MyTreeMesonLabPh = fMeson.Phi();
    MyTreeMesonLabE = fMeson.E();

    MyTreeBeamPolBit = 0; //for polarisated beamtime: fADC[6];
    MyTreeTriggerPat = 0; //for polarisated beamtime: ((int)(fADC[1] & 0xFF)<<8) + (fADC[0] & 0xFF);
}

int TreeFillPhoton(double fDeltaTime, double fEnergy, int fTaggerCh, double fMissingMass, double fMesonThetaCM) {
    //P4Fehlend = fP4target[0] + Tagged[i].GetP4() - PMesonKorrigiert.GetP4();

    MyTreeTaggerE[MyTreeNTaggerHits] = fEnergy;
    MyTreeTaggerCh[MyTreeNTaggerHits] = fTaggerCh;
    MyTreeTaggerMesonLabTimeDiff[MyTreeNTaggerHits] = fDeltaTime;
    MyTreeLabMissingM[MyTreeNTaggerHits] = fMissingMass;
    MyTreeMesonCMTh[MyTreeNTaggerHits] = fMesonThetaCM;

    MyTreeNTaggerHits++;

    if (MyTreeNTaggerHits > 100) { printf("ERROR: Too many NTaggerHits: %i\n",MyTreeNTaggerHits); exit(-1); }
}

int TreeEventComplete() {
    if (MyTreeNTaggerHits > 0) {
        MyTree->Fill();
    }
}

*/

//-------------------------------------------
int InitialiseExportTreeFile() {}
int CloseExportTreeFile() {}
int TreeInitEvent() {}
int TreeFillMeson(TLorentzVector &fMeson) {}
int TreeFillPhoton(double fDeltaTime, double fEnergy, int fTaggerCh, double fMissingMass, double fMesonThetaCM) {}
int TreeEventComplete() {}
