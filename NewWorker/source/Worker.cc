/*
 * Fahrplan:
 *   1. T und F Analyse komplett durchziehen
 *   2. Wie müssen die Mittleren Werte von BeamPol, TargetPol und TaggEff richtig bestimmt werden?
 *   2. Defekte Bereich rausnehmen können
 *   3. TaggEff messen
 *   - CB Timing von Sep 2010 verbessern
 *   - Analyse auf cos \theta umstellen
 *
 *   - vielleicht doch nicht die komplette Datei am Anfang einlesen, sondern abschnittsweise
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
#include <TROOT.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TThread.h>
#include <TFile.h>

#include <Worker.h>
#include <ReadCalibrationFiles.cc>
#include <ReadRawDataFile.cc>
#include <ProcessDataFileMk1.cc>
#include <ProcessDataFileMk2.cc>

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
    strcpy(TempChar, OutputDirectory);
    strcat(TempChar, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    strcat(TempChar, ".root");
    printf("INFO: Output to: %s\n",TempChar);

    TFile *f = new TFile(TempChar, "RECREATE"); //Create and overwrite if file exists
    /*hCBHits_VS_EventID->Write();
    hMesonPhi_VS_EventID->Write();
    hMesonThetaLab_VS_EventID->Write();*/
    hMissingMassCombinedSignal->Write();
    hCountNumberOfHistos->Write();
    hTaggerScalerAccum->Write();
    hTaggerTime->Write();
    hLiveTimeAccum->Write();
    hMissingMassCombinedSignalLTCorrected->Write();
    hMissingMassCombinedSignalLTCorrectedP->Write();
    hMissingMassCombinedSignalLTCorrectedM->Write();
    hTaggerScalerAccumLTCorrected->Write();
    hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff->Write();
    hTaggerScalerAccumPhotonsLTCorrected->Write();
    hBeamPol->Write();
    hTargetPol->Write();
    hTaggEffAbs->Write();
    hTaggEffAbsAllMesons->Write();
    f->Close();
    printf("INFO: Completed writing histograms to ROOT file.\n");

   /* HINT: 08.08.2013: works only, if there is not ROOT App startet yet (-b mode), why, I don't know.
    *
    printf("INFO: Started saving histograms.\n");
    sprintf(TempChar, "%s/%s_CBHits.png", OutputDirectory, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    printf("INFO: Output to: %s\n",TempChar);
    TCanvas *c = new TCanvas("c");
    hCBHits_VS_EventID->Draw("COLZ");
    c->SaveAs(TempChar);

    hMesonPhi_VS_EventID->Draw("COLZ");
    sprintf(TempChar, "%s/%s_MesonPhi.png", OutputDirectory, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    c->SaveAs(TempChar);

    hMesonThetaLab_VS_EventID->Draw("COLZ");
    sprintf(TempChar, "%s/%s_MesonTheta.png", OutputDirectory, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    c->SaveAs(TempChar);

    hMissingMassCombinedSignalLTCorrected->Draw("COLZ");
    sprintf(TempChar, "%s/%s_MissMass.png", OutputDirectory, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    c->SaveAs(TempChar);


    c->Close();
    delete c;
    *
    */
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
    CloseExportTreeFile();

    printf("INO: MyThread End.\n");
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

    //-------------- Ok nun eingelesen, mache etwas mit den Daten
    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};
	TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);

    InitCalibHistograms();
    InitialiseExportTreeFile();

    Printf("--------------------------------------");


    if (ProgramBatchMode) {
        ProcessFile();
        CloseExportTreeFile();
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
