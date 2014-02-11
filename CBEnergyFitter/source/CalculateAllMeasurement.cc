
char RootFileNameToRead[1000];

int ReadAnalysisConfigurationFile(int fFileNumber) {
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t Buffer2[1024];

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    int ReturnValue = 0;

    sprintf(TempStr, "/datapool/home/otte/NewAnalysis/data/%s/Analysis.dat", RunsMetaInformation.at(fFileNumber).DataPath.c_str());
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        printf("Could not open analysis config file!\n");
        exit(1);
    } else {
        //Printf("INFO: Analysis file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';

        if (sscanf(Line, "%s %s ", Buffer, Buffer2 ) == 2) {
            if (!strcmp(Buffer, "OutputPath:")) {
                strcpy(RootFileNameToRead, Buffer2);
                //Printf("INFO: RootFileNameToRead: %s",RootFileNameToRead);
                ReturnValue = -1;
            }
        }


    }

    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        printf("Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }

    return ReturnValue;
}



void DoFile(int fFileNumber) {

    // ***************************************************************
    // Read files
    // ***************************************************************

    if (ProgramDebugMode) {
        Printf("--------------------------------------");
    } else {
        printf(".");
        fflush(stdout);
    }

    char buffer[1024];

    if (ReadAnalysisConfigurationFile(fFileNumber) == 0) {
        printf("WARNING: fFileNumber %d. Could not find value for root file name.\n", fFileNumber);
    }

//    sprintf(buffer, "%s/%s.root", TaggEffInputPath, RunsMetaInformation.at(fback1).FileName.c_str());
    sprintf(buffer, "%s/%s.root", RootFileNameToRead, RunsMetaInformation.at(fFileNumber).FileName.c_str());

    TFile* file1 = new TFile(buffer);
    if ( file1->IsOpen() ) {
        if (ProgramDebugMode) printf("INFO: File No %d loaded.\n", fFileNumber);
    } else {
        if (ProgramDebugMode) printf("ERROR: File %s not found.\n", buffer);
        return;
    }


    // ***************************************************************


    // ***************************************************************
    // Connect to Histograms
    // ***************************************************************
    //Meson Mass
    TH1D *histMesonMass1D = (TH1D*)file1->Get("MesonInvariantMass");
    if( !histMesonMass1D ) {
        printf("WARNING: File %d: 1D Meson Invariant Mass histogram not found.\n", fFileNumber);
        return;
    }
    //Dropped Events
    TH1D *histDroppedEvents = (TH1D*)file1->Get("hDroppedEvents");
    if( !histDroppedEvents ) {
        printf("WARNING: File %d: 1D Dropped Events histogram not found.\n", fFileNumber);
        return;
    }
    TH1D *hScaler = (TH1D*)file1->Get("TaggerScalerAccum");
    if( !hScaler ) {
        printf("WARNING: File %d: 1D TaggerScalerAccum histogram not found.\n", fFileNumber);
        return;
    }
    TH2D *hTime2D = (TH2D*)file1->Get("hTaggerTime");
    if( !hTime2D ) {
        printf("WARNING: File %d: 2D hTaggerTime histogram not found.\n", fFileNumber);
        return;
    }

    if (ProgramDebugMode) printf("INFO: All histograms connected.\n");
    // ***************************************************************

    double TempValue = 0;

    if (histDroppedEvents->GetBinContent(2) > 0) {
        TempValue = histDroppedEvents->GetBinContent(1)/histDroppedEvents->GetBinContent(2);
    }
    hDroppedEventsRatio->SetBinContent(fFileNumber+1, TempValue);
    hTotalEvents->SetBinContent(fFileNumber+1, histDroppedEvents->GetBinContent(2));



    //Begin: CB Meson Mass
    Char_t TempStr[1024];
    sprintf(TempStr,"PrevFitTMP");
    delete gROOT->FindObject(TempStr);

    TF1 *PrevFitTMP = new TF1(TempStr,"gaus",134-10,134+10);
    //PrevFitTMP->SetParameter(1, 0.09);

    char FitParameters[10] = "QRF0";
    if (ProgramDebugMode) { strcpy(FitParameters, "RF0"); }
    TFitResultPtr MyFitResult = histMesonMass1D->Fit(TempStr, FitParameters); //0 = do not draw, q=quiet, R = respect range, f = special min finder

    Double_t MeanTaggEff = 0;
    Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        MeanTaggEff = PrevFitTMP->GetParameter(1);
        if (ProgramDebugMode) printf("INFO: Fit result: %f +- %f\n", PrevFitTMP->GetParameter(1), PrevFitTMP->GetParError(1));

        hMesonMassCenterVSFiles->SetBinContent(fFileNumber+1, PrevFitTMP->GetParameter(1));
        hMesonMassCenterVSFiles->SetBinError(fFileNumber+1, PrevFitTMP->GetParError(1));

        hMesonMassWidthVSFiles->SetBinContent(fFileNumber+1, PrevFitTMP->GetParameter(2));
        hMesonMassWidthVSFiles->SetBinError(fFileNumber+1, PrevFitTMP->GetParError(2));

        RunsMetaInformation.at(fFileNumber).CBEnergyScale = RunsMetaInformation.at(fFileNumber).CBEnergyScale * (MassPion0 / PrevFitTMP->GetParameter(1));

        if ( (RunsMetaInformation.at(fFileNumber).CBEnergyScale > 1.05) || (RunsMetaInformation.at(fFileNumber).CBEnergyScale < 0.95) ) {
            printf("WARNING: File %d: Correction value out of 5 percent range (%f).\n", fFileNumber, RunsMetaInformation.at(fFileNumber).CBEnergyScale);
        }

    } else {
        printf("ERROR: File %d: Fit did not converge.\n", fFileNumber);
        RunsMetaInformation.at(fFileNumber).CBEnergyScale = 1.0;
    }
    // ***************************************************************


    // ***************************************************************
    // Scaler / TDC Tests
    // ***************************************************************

    TH1D *hTime1D = hTime2D->ProjectionY("Time1D",700,950);

    delete gROOT->FindObject("Divide");
    TH1D* hDiff = (TH1D*)hScaler->Clone("Divide");

    hDiff->Divide(hTime1D);
    for (int i=0; i< NTaggChs; i++) {
        hScalerVSTDCRatio->SetBinContent(fFileNumber+1, i+1, hDiff->GetBinContent(i+1));
        hScalerVSTDCRatio->SetBinError(fFileNumber+1, i+1, hDiff->GetBinError(i+1));
    }


    // ***************************************************************




    // ***************************************************************
    // Close files
    // ***************************************************************

    file1->Close();

    // ***************************************************************

    if (ProgramDebugMode) {
        Printf(" ");
    }
}


void DoAllFiles() {
    for (int i=0; i<NRunsMetaInformation; i++) {
//    for (int i=0; i<100; i++) {
        if (RunsMetaInformation.at(i).RunType == 0) {
            DoFile(i);
        }
    }
}



int PlotAllMeasurements() {

    TCanvas *c1 = new TCanvas("MyCanvas");
    c1->Divide(2,2);
    gStyle->SetPalette(1);

    c1->cd(1);
    ((TH1D*)gROOT->FindObject("MesonMassCenterVSFiles"))->Draw("");
    c1->cd(2);
    ((TH1D*)gROOT->FindObject("MesonMassWidthVSFiles"))->Draw("");
    c1->cd(3);
    hDroppedEventsRatio->Draw();
    c1->cd(4);
    hTotalEvents->Draw();

    TCanvas *c2 = new TCanvas("c2");
    hScalerVSTDCRatio->Draw("COLZ");

    return 0;
}


