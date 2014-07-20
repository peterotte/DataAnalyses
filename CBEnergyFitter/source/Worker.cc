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
    Int_t WriteCBEnergyScaleNew = 0;
    Int_t ReplaceRunMetaInfoFile = 0;
    int ActualSetupNumber = 0;
    for (int i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(TempStr, argv[i]);
            int TempValue = strtol(TempStr, (char **)NULL, 10);
            if (ActualSetupNumber==0) {
                ActualSetupNumber++;
                FirstFileNumberToAnalyse = TempValue;
                Printf("FirstFileNumberToAnalyse: %d\n", FirstFileNumberToAnalyse);
            } else if (ActualSetupNumber==1) {
                ActualSetupNumber++;
                LastFileNumberToAnalyse = TempValue;
                Printf("LastFileNumberToAnalyse: %d\n", LastFileNumberToAnalyse);
            }

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

    ReadRunsInformationConfigurationFile(ProgramDebugMode);

    // ** Prepare Histograms **
    gROOT->cd();
    hMesonMassCenterVSFiles = new TH1D("MesonMassCenterVSFiles", "MesonMassCenterVSFiles", 1600,0,1600);
    hMesonMassWidthVSFiles = new TH1D("MesonMassWidthVSFiles", "MesonMassWidthVSFiles", 1600,0,1600);
    hDroppedEventsRatio = new TH1D("DroppedEventsRatio", "DroppedEventsRatio", 1600, 0, 1600);
    hTotalEvents = new TH1D("hTotalEvents", "hTotalEvents", 1600, 0, 1600);
    hScalerVSTDCRatio_Meson = new TH2D("hScalerVSTDCRatio_Meson", "hScalerVSTDCRatio_Meson", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hScalerVSTDCRatio_Tagg = new TH2D("hScalerVSTDCRatio_Tagg", "hScalerVSTDCRatio_Tagg", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hScalerVSScalerRatio_Tagg = new TH2D("hScalerVSScalerRatio_Tagg", "hScalerVSScalerRatio_Tagg", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hPromptTDCVSScalerRatio_Tagg = new TH2D("hPromptTDCVSScalerRatio_Tagg", "hPromptTDCVSScalerRatio_Tagg", 1600, 0, 1600, NTaggChs, 0, NTaggChs);

    hPIDECalibration = new TH2D("hPIDECalibration", "hPIDECalibration", 1600, 0, 1600, NPIDChs, 0, NPIDChs);

    hTaggScAccum_Stability = new TH2D("TaggScAccum_Stability", "TaggScAccum_Stability", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hCBADCHits_Stability =  new TH2D("CBADCHits_Stability", "CBADCHits_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hCBTDCHits_Stability =  new TH2D("CBTDCHits_Stability", "CBTDCHits_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hCBHits_Stability = new TH2D("CBHits_Stability", "CBHits_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hTaggerChTDC_Stability = new TH2D("TaggerChTDC_Stability", "TaggerChTDC_Stability", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hCBPromptTime_Stability = new TH2D("CBPromptTime_Stability", "CBPromptTime_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hTaggerTimePrompt_Stability = new TH2D("TaggerTimePrompt_Stability", "TaggerTimePrompt_Stability", 1600, 0, 1600, NTaggChs, 0, NTaggChs);
    hCBChEnergy_Stability = new TH2D("CBChEnergy_Stability", "CBChEnergy_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hPIDE_Stability = new TH2D("PIDE_Stability", "PIDE_Stability", 1600, 0, 1600, NPIDChs, 0, NPIDChs);
    hCBHitsPrompt_Stability = new TH2D("CBHitsPrompt_Stability", "CBHitsPrompt_Stability", 1600, 0, 1600, NCBChs, 0, NCBChs);
    hCBEnergySum_Stability = new TH2D("CBEnergySum_Stability", "CBEnergySum_Stability", 1600, 0, 1600, 250, 0, 500);
    hMesonPhi_Stability = new TH2D("MesonPhi_Stability", "MesonPhi_Stability", 1600, 0, 1600, 45, -180, 180);
    hMesonTheta_Stability = new TH2D("MesonTheta_Stability", "MesonTheta_Stability", 1600, 0, 1600, 45, 0, 180);

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


