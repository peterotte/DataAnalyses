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
#include <TMinuit.h>
#include <TMath.h>

#include <Worker.h>

#include "ReadConfigFiles.cc"
#include "CalculateTaggEff.cc"
#include "CalculateAllMeasurement.cc"

// *****************************************************************



int main(int argc, char **argv) {
    //Handle command line options
    Char_t TempStr[1024];
    Int_t fSelectedBeamtime = 0;
    Int_t WriteAbsTaggEffNew = 0;
    Int_t ReplaceRunMetaInfoFile = 0;
    for (int i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(TempStr, argv[i]);
            fSelectedBeamtime = strtol(TempStr, (char **)NULL, 10);
            Printf("INFO: SelectedBeamtime: %i\n", fSelectedBeamtime);
        } else {
            if (!strcmp("-b", argv[i])) ProgramBatchMode = -1;
            if (!strcmp("-d", argv[i])) ProgramDebugMode = -1;
            if (!strcmp("-new", argv[i])) WriteAbsTaggEffNew = -1;
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
    theApp->SetPrompt("NewTaggEffAna %d: ");

    ReadRunsInformationConfigurationFile(fSelectedBeamtime, ProgramDebugMode);

    DoAllTaggEffRuns(fSelectedBeamtime);
    CombineAllMeasurements();
    FindOptionalScale();
    ChangeAbsTaggEffValuestoScale(fSelectedBeamtime);
    WriteRunsInformationConfigurationFile(WriteAbsTaggEffNew);
    if (ReplaceRunMetaInfoFile) {
        ReplaceAndBackupRunsMetaInformation();
        SaveTaggEffNormRootFile();
    }


    if (!ProgramBatchMode) {
        PlotAllMeasurements();
        PlotTaggEffTrend(fSelectedBeamtime);

        //    TBrowser *TB = new TBrowser();

        theApp->Run();
    }
}


