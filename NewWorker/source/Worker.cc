/*
 * Fahrplan:
 *   1. T und F Analyse komplett durchziehen
 *   2. Wie müssen die Mittleren Werte von BeamPol, TargetPol und TaggEff richtig bestimmt werden?
 *   3. TaggEff messen
 *   - CB Timing von Sep 2010 verbessern
 *   - vielleicht den PID ganz herausnehmen?
 *
 *   4. Tests des Einflusses des PIDs (untere Schwelle)
 *   5. Kalibrationen vom PID (untere Energie schwelle!) verbessern
 *   6. Vielleicht doch den PID TDC mit reinnehmen?
 *   7. Wegen der Normierung auf die Zähler, vielleicht doch nur ein pi0 pro Event rekonstruieren?
 *   8. Zeit zwischen Tagger und Meson betrachten, nicht mehr nur die Taggerzeit
 *   9. Das Helicity Pattern genauer untersuchen und Ereignisse mit besonderen Bits aussortieren.
 *
 */



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
#include <TH3.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TThread.h>
#include <TFile.h>

#include "TA2KFParticle.h"
#include "TA2CBKinematicFitter.h"


#include <Worker.h>
#include <ReadCalibrationFiles.cc>
#include <ReadRawDataFile.cc>
#include <ProcessDataFileMk2.cc>
#include <ProcessDataFileMk1.cc>

//-----------------------------------------------------------------------------

void sigfunc(int sig) {
    if (sig!=SIGINT) {
        return;
    } else {
        printf("Called CTRL-C.\n");
        SigIntCalled--;
    }
}

//-----------------------------------------------------------------------------

int SaveSpectraAfterToDisc() {
    Char_t TempChar[1024];
    printf("INFO: Started saving ROOT file.\n");
    sprintf(TempChar, "%s/%d.root", OutputDirectory, IndexRunMetaInfomation);
    printf("INFO: Output to: %s\n",TempChar);

    TFile *f = new TFile(TempChar, "RECREATE"); //Create and overwrite if file exists
    hTaggerScalerAccum_VS_EventID->Write();
    hCBADCHits_VS_EventID->Write();
    hCBTDCHits_VS_EventID->Write();
    hCBHits_VS_EventID->Write();
    hTaggerChTDC_VS_EventID->Write();
    hCBTimePrompt_VS_EventID->Write();
    hTaggerTimePrompt_VS_EventID->Write();
    hCBChEnergy_VS_EventID->Write();
    hPIDChEnergy_VS_EventID->Write();
    hCBHitsPrompt_VS_EventID->Write();
    hCBEnergySum_VS_EventID->Write();
    hMesonPhi_VS_EventID->Write();
    hMesonThetaLab_VS_EventID->Write();

    hPIDChADC->Write();

    hMesonInvariantMass->Write();
    hCBEnergySum->Write();
    hCBEnergySumTP->Write();
    hCBEnergySumTM->Write();
    hCBEnergySumFP->Write();
    hCBEnergySumFM->Write();
    hMissingMassCombinedSignal->Write();
    hCountNumberOfHistos->Write();
    hDroppedEvents->Write();
    hTaggerScalerAccum->Write();
    hTaggerTime->Write();
    hLiveTimeAccum->Write();
    hMissingMassCombinedSignalLTCorrected->Write();
    hMissingMassCombinedSignalLTCorrectedFP->Write();
    hMissingMassCombinedSignalLTCorrectedFM->Write();
    hMissingMassCombinedSignalLTCorrectedTP->Write();
    hMissingMassCombinedSignalLTCorrectedTM->Write();
    hTaggerScalerAccumLTCorrectedAllEvents->Write();
    hTaggerScalerAccumLTCorrected->Write();
    hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff->Write();
    hTaggerScalerAccumPhotonsLTCorrected->Write();
    hBeamPol->Write();
    hTargetPolF->Write();
    hTargetPolT->Write();
    hTaggEffAbsT->Write();
    hTaggEffAbsF->Write();
    hTaggEffAbsAll->Write();
    f->Close();
    printf("INFO: Completed writing histograms to ROOT file.\n");

   /* HINT: 08.08.2013: works only, if there is not ROOT App startet yet (-b mode), why, I don't know.
    */
    if (ProgramBatchMode) {
        printf("INFO: Started saving histograms.\n");
        sprintf(TempChar, "%s/%d_CBHits.png", OutputDirectory, IndexRunMetaInfomation); //RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str()
        printf("INFO: Output to: %s\n",TempChar);
        TCanvas *c = new TCanvas("c");
        hCBHits_VS_EventID->Draw("COLZ");
        c->SaveAs(TempChar);

        hMesonPhi_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_MesonPhi.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hMesonThetaLab_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_MesonTheta.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hMissingMassCombinedSignalLTCorrected->Draw("COLZ");
        sprintf(TempChar, "%s/%d_MissMass.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        if (RawDataFormat == 1) { hTaggerScalerAccum_VS_EventID->SetAxisRange(0,200,"X"); }
        hTaggerScalerAccum_VS_EventID->Draw("COLZ");
        if (RawDataFormat == 1) { hTaggerScalerAccum_VS_EventID->SetAxisRange(0,200,"X"); }
        sprintf(TempChar, "%s/%d_TaggScaler.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hTaggerChTDC_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_TaggTDC.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hCBTimePrompt_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_CBTimePrompt.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hTaggerTimePrompt_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_TaggTimePrompt.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hCBChEnergy_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_CbEnergy.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hPIDChEnergy_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_PIDEnergy.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hCBHitsPrompt_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_CBHitsPrompt.png", OutputDirectory, IndexRunMetaInfomation);
        c->SaveAs(TempChar);

        hCBEnergySum_VS_EventID->Draw("COLZ");
        sprintf(TempChar, "%s/%d_CBEnergySum.png", OutputDirectory, IndexRunMetaInfomation );
        c->SaveAs(TempChar);


        c->Close();
        delete c;
    }
}




//-----------------------------------------------------------------------------
int ProcessFile() {
    //Register Signal handler function
    signal(SIGINT, sigfunc);

    if (RawDataFormat == 1) {
        ProcessDataFileMk1();
    } else if (RawDataFormat == 2) {
        ProcessDataFileMk2();
    } else {
        Printf("ERROR: Wrong RawDataFormat in Analysis Config File provided: %d.", RawDataFormat);
        exit(1);
    }

    CloseRawFile();

    Do_FinalPhysicsAnalysis();

    if (ProgramSaveSpectraMode) {
        SaveSpectraAfterToDisc();
    }

    return 0;
}

TThread *MyThread;
void *ThreadHandle(void *ptr) {
    printf("INFO: MyThread Start.\n");

    ProcessFile();

    printf("INFO: MyThread End.\n");
    return 0;
}


int main(int argc, char **argv)
{
    InitializeLookupTables();
    //c standard: 0 = false, sonst true
    Char_t FileIDStr[1024];
    FileIDStr[0] = '\0';

    //Handle command line options
    for (int i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(FileIDStr, argv[i]);
        } else {
            if (!strcmp("-b", argv[i])) ProgramBatchMode = -1;
            if (!strcmp("-c", argv[i])) ProgramOnlyFileCheckMode = -1;
            if (!strcmp("-s", argv[i])) ProgramSaveSpectraMode = -1;
            if (!strcmp("-n", argv[i])) { ProgramNoTBrowserMode = -1; ProgramBatchMode = -1;}
        }
    }
    if (ProgramBatchMode) { Printf("Program runs in batch mode."); } else { Printf("Program runs in interactive mode."); }
    if (ProgramOnlyFileCheckMode) { Printf("Program runs in only file check mode."); } else { Printf("Program runs NOT in only file check mode."); }
    if (ProgramSaveSpectraMode) { Printf("Program saves spectra after analysis."); } else { Printf("Program does NOT save spectra after analysis."); }
    if (ProgramNoTBrowserMode) { Printf("Program does not open a Prompt."); } else { Printf("Program opens a Prompt."); }
    Printf("--------------------------------------");


    if (strlen(FileIDStr) == 0) {
        Printf("ERROR: No FileIDStr provided.");
        return 1;
    }
    ReadRunsInformationConfigurationFile(); //Reads data/RunsMetaInformation.dat
    if (sscanf(FileIDStr, "%d", &IndexRunMetaInfomation) == 1) {
        if ( (IndexRunMetaInfomation >= 0) && (IndexRunMetaInfomation < RunsMetaInformation.size()) ) {
            Printf("INFO: IndexRunMetaInfomation %d.",IndexRunMetaInfomation);
        } else {
            Printf("ERROR: No RunInformation found for this data file.");
            exit(1);
        }
    } else {
        Printf("ERROR: Could not convert provided FileIDStr into integer.");
        exit(1);
    }

    // Check for bad file status
    if (RunsMetaInformation.at(IndexRunMetaInfomation).RunType == 2) {
        Printf("ERROR: The selected file %s is marked as bad. Exiting.", RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
        exit(1);
    }

    strcat(InputPathDataDir, RunsMetaInformation.at(IndexRunMetaInfomation).DataPath.c_str());
    strcat(InputPathDataDir, "/");
    Printf("INFO: InputPathDataDir: %s",InputPathDataDir);

    ReadConfigurationFiles();

    PrepareReadRawData();
    //Now we have read the first amount of data from disc. Do something with the data!

    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};
	TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);

    InitCalibHistograms();

    Printf("--------------------------------------");


    if (ProgramBatchMode) {
        ProcessFile();
    } else {
        MyThread = new TThread("MyThread", ThreadHandle, (void*) 0);
        MyThread->Run();
    }

    if (!ProgramNoTBrowserMode) {
        if (SigIntCalled >= -1) { //Start the GUI only if CTRL-C has not been sent more often than once.
            theApp->SetPrompt("NewAnalyser %d: ");

            //TBrowser *TB = new TBrowser();

            theApp->Run();
        }
    }

    if (!ProgramBatchMode) {
        MyThread->Join();

        delete MyThread;
    }

    return 0;
}
