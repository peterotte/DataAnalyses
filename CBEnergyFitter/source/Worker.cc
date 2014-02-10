#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h> //For clock_t clock and CLOCKS_PER_SEC
#include <signal.h>

#include <TRint.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TF1.h>
#include <TMath.h>

#include <Worker.h>

#include "ReadConfigFiles.cc"
#include "CalculateAllMeasurement.cc"

// *****************************************************************



int main(int argc, char **argv) {
    //Handle command line options
    Char_t TempStr[1024];
    Int_t fSelectedBeamtime = 0;
    Int_t WriteCBEnergyScaleNew = 0;
    Int_t ReplaceRunMetaInfoFile = 0;
    for (int i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(TempStr, argv[i]);
            fSelectedBeamtime = strtol(TempStr, (char **)NULL, 10);
            Printf("INFO: SelectedBeamtime: %i\n", fSelectedBeamtime);
        } else {
            if (!strcmp("-b", argv[i])) ProgramBatchMode = -1;
            if (!strcmp("-d", argv[i])) ProgramDebugMode = -1;
            if (!strcmp("-new", argv[i])) WriteCBEnergyScaleNew = -1;
            if (!strcmp("-r", argv[i])) ReplaceRunMetaInfoFile = -1;
        }
    }
    if (ProgramBatchMode) { Printf("Program runs in batch mode."); } else { Printf("Program runs in interactive mode."); }


    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};

    TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);
    theApp->SetPrompt("CBScaleAna %d: ");

    ReadRunsInformationConfigurationFile(fSelectedBeamtime, ProgramDebugMode);

    // ** Prepare Histograms **
    gROOT->cd();
    hMesonMassCenterVSFiles = new TH1D("MesonMassCenterVSFiles", "MesonMassCenterVSFiles", 1600,0,1600);
    hMesonMassWidthVSFiles = new TH1D("MesonMassWidthVSFiles", "MesonMassWidthVSFiles", 1600,0,1600);
    hDroppedEventsRatio = new TH1D("DroppedEventsRatio", "DroppedEventsRatio", 1600, 0, 1600);
    hTotalEvents = new TH1D("hTotalEvents", "hTotalEvents", 1600, 0, 1600);
    // ** Prepare Histograms End **

    DoAllFiles();

    WriteRunsInformationConfigurationFile(WriteCBEnergyScaleNew);
    if (ReplaceRunMetaInfoFile) {
        ReplaceAndBackupRunsMetaInformation();
    }


    if (!ProgramBatchMode) {
        PlotAllMeasurements();

        //    TBrowser *TB = new TBrowser();

        theApp->Run();
    }
}


