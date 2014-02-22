
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


int CalculateStability(int fFileNumber, TFile *file1, std::string hSource_Str, TH2D *hTarget) {

    TH2D *hSource = (TH2D*)file1->Get( hSource_Str.c_str() );
    if( !hSource ) {
        printf("WARNING: File %d: 2D %s histogram not found.\n", fFileNumber, hSource_Str.c_str());
        return 1;
    }

    delete gROOT->FindObject("Temp_CountsEvents");
    TH1D *hPIDE_CountEvents = hSource->ProjectionX("Temp_CountsEvents");
    int NPIDE_Events = 0;
    for (int i=1; i<=hPIDE_CountEvents->GetNbinsX(); i++) { if (hPIDE_CountEvents->GetBinContent(i)>0) NPIDE_Events=i; }
    delete gROOT->FindObject("Temp_ProjectionY");
    TH1D *hPIDE = hSource->ProjectionY("Temp_ProjectionY");
    hPIDE->Scale(1./(double)NPIDE_Events*hSource->GetNbinsY());
    for (int i=0; i<hSource->GetNbinsY(); i++) {
        hTarget->SetBinContent(fFileNumber+1, i+1, hPIDE->GetBinContent(i+1));
        hTarget->SetBinError(fFileNumber+1, i+1, hPIDE->GetBinError(i+1));
    }
}


void DoFile(int fFileNumber, int fDoCBEnergyScaling, int fDoStabilityTests, int fDoPIDADCCalibration) {

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

    sprintf(buffer, "%s/%d.root", RootFileNameToRead, fFileNumber);

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
    TH1D *hMesonInvariantMass1D = (TH1D*)file1->Get("MesonInvariantMass");
    if( !hMesonInvariantMass1D ) {
        printf("WARNING: File %d: 1D MesonInvariantMass histogram not found.\n", fFileNumber);
        return;
    }
    TH1D *hLiveTime = (TH1D*)file1->Get("LiveTimeAccum");
    if( !hLiveTime ) {
        printf("WARNING: File %d: 1D LiveTimeAccum histogram not found.\n", fFileNumber);
        return;
    }
    TH2D *hPIDChADC = (TH2D*)file1->Get("hPIDChADC");
    if( !hPIDChADC ) {
        printf("WARNING: File %d: 2D hPIDChADC histogram not found.\n", fFileNumber);
        return;
    }

    if (ProgramDebugMode) printf("INFO: All histograms connected.\n");
    // ***************************************************************

    double TempValue = 0;
    double TotalNumberOfEvents = histDroppedEvents->GetBinContent(2);
    double TotalDroppedEvents = histDroppedEvents->GetBinContent(1);
    double TotalUsedEvents = TotalNumberOfEvents-TotalDroppedEvents;

    if (TotalNumberOfEvents > 0) {
        TempValue = TotalDroppedEvents/TotalNumberOfEvents;
    }
    hDroppedEventsRatio->SetBinContent(fFileNumber+1, TempValue);
    hTotalEvents->SetBinContent(fFileNumber+1, TotalNumberOfEvents);


    if (fDoCBEnergyScaling) {
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
    }
    // ***************************************************************


    // ***************************************************************
    // Scaler / TDC Tests
    // ***************************************************************

    if (fDoStabilityTests) {
        delete gROOT->FindObject("Divide");
        TH1D* hDiff = (TH1D*)hScaler->Clone("Divide");

        double LiveTimeTagger = hLiveTime->GetBinContent(3) / hLiveTime->GetBinContent(1);
        double LiveTimeExp = hLiveTime->GetBinContent(2) / hLiveTime->GetBinContent(1);

        //Method 1: Tagg Sc / Tagg TDCs
    /*    TH1D *hTime1D = hTime2D->ProjectionY("Time1D",700,950);
        //hScaler->Scale(1./LiveTimeTagger);
       // hTime1D->Scale(1./LiveTimeExp);

        hDiff->Divide(hTime1D);
    */
        //Method 2: Tagg Sc / NMesons
        double NMesons = hMesonInvariantMass1D->Integral(134-20+1,124+20+1);
        hDiff->Scale(1./NMesons);
        hDiff->Scale(1.*LiveTimeExp/LiveTimeTagger);

        for (int i=0; i< NTaggChs; i++) {
            hScalerVSTDCRatio->SetBinContent(fFileNumber+1, i+1, hDiff->GetBinContent(i+1));
            hScalerVSTDCRatio->SetBinError(fFileNumber+1, i+1, hDiff->GetBinError(i+1));
        }
        // ***************************************************************

        //Stability Tests
        CalculateStability(fFileNumber, file1, "TaggerScalerAccum_VS_EventID", hTaggScAccum_Stability);
        CalculateStability(fFileNumber, file1, "CBADCHits_VS_EventID", hCBADCHits_Stability);
        CalculateStability(fFileNumber, file1, "CBTDCHits_VS_EventID", hCBTDCHits_Stability);
        CalculateStability(fFileNumber, file1, "CBHits_VS_EventID", hCBHits_Stability);
        CalculateStability(fFileNumber, file1, "TaggerChTDC_VS_EventID", hTaggerChTDC_Stability);
        CalculateStability(fFileNumber, file1, "CBPromptTime_VS_EventID", hCBPromptTime_Stability);
        CalculateStability(fFileNumber, file1, "TaggerTimePrompt_VS_EventID", hTaggerTimePrompt_Stability);
        CalculateStability(fFileNumber, file1, "CBChEnergy_VS_EventID", hCBChEnergy_Stability);
        CalculateStability(fFileNumber, file1, "PIDChEnergy_VS_EventID", hPIDE_Stability);
        CalculateStability(fFileNumber, file1, "CBHitsPrompt_VS_EventID", hCBHitsPrompt_Stability);
        CalculateStability(fFileNumber, file1, "hCBEnergySum_VS_EventID", hCBEnergySum_Stability);
        CalculateStability(fFileNumber, file1, "MesonPhi_VS_EventID", hMesonPhi_Stability);
        CalculateStability(fFileNumber, file1, "MesonThetaLab_VS_EventID", hMesonTheta_Stability);

        // ******************************************************

    }

    //****************************************************************

    if (fDoPIDADCCalibration) {
        gROOT->cd();

        for (int PIDCh = 0; PIDCh<NPIDChs; PIDCh++) {
 //       for (int PIDCh = 14; PIDCh<15; PIDCh++) {
            TH1D *hSinglePIDCH = hPIDChADC->ProjectionX("SinglePIDCh",PIDCh+1,PIDCh+1);

            //Search rougly for start of data
            int i=1;
            while ( (hSinglePIDCH->GetBinContent(i) <= 100.) && (i<200) ) { i++;  } //smaller than 100 because sometimes the first few channels are still filled.

            //next, search for min
            double ActValue = hSinglePIDCH->GetBinContent(i);
            int ActMinPos = i;
            while ( ( hSinglePIDCH->GetBinContent(ActMinPos) > hSinglePIDCH->GetBinContent(ActMinPos+1) ) && (ActMinPos<300) ) { ActMinPos++;  }

            double FirstReallyFilledChannel = i*10.;
            if (ProgramDebugMode) { printf("FirstReallyFilledChannel: %f\n",FirstReallyFilledChannel);}
            double FirstMinimaChannel = ActMinPos*10.;
            if (ProgramDebugMode) { printf("FirstMinimaChannel: %f\n",FirstMinimaChannel);}

            //Find begin of fit
            double BeginOfFitX = FirstMinimaChannel-40.;
            if (BeginOfFitX < FirstReallyFilledChannel) BeginOfFitX = FirstReallyFilledChannel;
            if (ProgramDebugMode) { printf("BeginOfFitX: %f\n",BeginOfFitX);}

            Char_t TempStr2[1024];
            sprintf(TempStr2,"PrevFitTMP2");
            delete gROOT->FindObject(TempStr2);

            TF1 *PrevFitTMP2 = new TF1(TempStr2,"[1]/(x-[0])+gaus(2)+landau(5)", BeginOfFitX, 2000);
            PrevFitTMP2->SetParameter(0, FirstReallyFilledChannel); //Shift
            PrevFitTMP2->SetParameter(1, 40000.); //N0
            PrevFitTMP2->SetParameter(2, 1000.); //N1
            PrevFitTMP2->SetParameter(3, 245.); //Mean1 //245
            PrevFitTMP2->SetParameter(4, 35.); //Sigma 1
            PrevFitTMP2->SetParameter(5, 3700.); //N2
            PrevFitTMP2->SetParameter(6, 350.); //Mean2
            PrevFitTMP2->SetParameter(7, 82.); //Sigma2
            PrevFitTMP2->SetParLimits(0, 0, 150);
            PrevFitTMP2->SetParLimits(1, 10000, 2000000);
            PrevFitTMP2->SetParLimits(2, 0, 12000);
            PrevFitTMP2->SetParLimits(3, 150, 500); //100
            PrevFitTMP2->SetParLimits(4, 1, 200);
            PrevFitTMP2->SetParLimits(5, 0, 30000);
            PrevFitTMP2->SetParLimits(6, 150, 1000); //100
            PrevFitTMP2->SetParLimits(7, 1, 300);

            char FitParameters2[10] = "QRF0";
            if (ProgramDebugMode) { strcpy(FitParameters2, "RF0"); }
       //     strcpy(FitParameters2, "RF");
            TFitResultPtr MyFitResult2 = hSinglePIDCH->Fit(TempStr2, FitParameters2); //0 = do not draw, q=quiet, R = respect range, f = special min finder

            Double_t MeanGaus = 0;
            Double_t SigmaGaus = 0;
            Int_t MyFitStatus2 = MyFitResult2; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
            if (MyFitStatus2 == 0) {
                if (PrevFitTMP2->GetParameter(0) > FirstReallyFilledChannel) FirstReallyFilledChannel=PrevFitTMP2->GetParameter(0)+1;
                MeanGaus = PrevFitTMP2->GetParameter(3);
                if (PrevFitTMP2->GetParameter(6)<MeanGaus) MeanGaus = PrevFitTMP2->GetParameter(6);
                SigmaGaus = PrevFitTMP2->GetParameter(4);
                if (PrevFitTMP2->GetParameter(7)<SigmaGaus) SigmaGaus = PrevFitTMP2->GetParameter(7);
                if (ProgramDebugMode) printf("INFO: Fit result: %f +- %f\n", MeanGaus, SigmaGaus);

                hPIDECalibration->SetBinContent(fFileNumber+1,1+PIDCh, PrevFitTMP2->GetMinimumX(FirstReallyFilledChannel+1, MeanGaus-1));

                if (ProgramDebugMode) printf("INFO: Minimum found (%f %f): %f", FirstReallyFilledChannel+1, MeanGaus-1, PrevFitTMP2->GetMinimumX(FirstReallyFilledChannel+1, MeanGaus-1));
            } else {
                printf("ERROR: File %d: PID Calibration Fit PIDCh %d did not converge.\n", fFileNumber, PIDCh);
            }

         //   TCanvas *c4 = new TCanvas("sdf");
         //   hSinglePIDCH->Draw();
        }

    }

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

    for (int i=FirstFileNumberToAnalyse; i<=LastFileNumberToAnalyse; i++) {
        if (RunsMetaInformation.at(i).RunType == 0) {
            DoFile(i, 1, 1, 1);
        }
    }
}



int PlotAllMeasurements() {

    gStyle->SetPalette(1);

    TCanvas *cPID = new TCanvas("cPID");
    hPIDECalibration->Draw("COLZ");

    TCanvas *c1 = new TCanvas("cOverview");
    c1->Divide(2,2);

    c1->cd(1);
    if (!gROOT->FindObject("MesonMassCenterVSFiles")) { printf("ERROR: Histo MesonMassCenterVSFiles not found.\n"); return 1; }
    ((TH1D*)gROOT->FindObject("MesonMassCenterVSFiles"))->Draw("");
    c1->cd(2);
    if (!gROOT->FindObject("MesonMassWidthVSFiles")) { printf("ERROR: Histo MesonMassWidthVSFiles not found.\n"); return 1; }
    ((TH1D*)gROOT->FindObject("MesonMassWidthVSFiles"))->Draw("");
    c1->cd(3);
    hDroppedEventsRatio->Draw();
    c1->cd(4);
    hTotalEvents->Draw();

    TCanvas *c3 = new TCanvas("cStability");
    c3->Divide(5,3);
    c3->cd(1); hTaggScAccum_Stability->Draw("COLZ");
    c3->cd(2); hCBADCHits_Stability->Draw("COLZ");
    c3->cd(3); hCBTDCHits_Stability->Draw("COLZ");
    c3->cd(4); hCBHits_Stability->Draw("COLZ");
    c3->cd(5); hTaggerChTDC_Stability->Draw("COLZ");
    c3->cd(6); hCBPromptTime_Stability->Draw("COLZ");
    c3->cd(7); hTaggerTimePrompt_Stability->Draw("COLZ");
    c3->cd(8); hCBChEnergy_Stability->Draw("COLZ");
    c3->cd(9); hPIDE_Stability->Draw("COLZ");
    c3->cd(10); hCBHitsPrompt_Stability->Draw("COLZ");
    c3->cd(11); hCBEnergySum_Stability->Draw("COLZ");
    c3->cd(12); hMesonPhi_Stability->Draw("COLZ");
    c3->cd(13); hMesonTheta_Stability->Draw("COLZ");
    c3->cd(14); hScalerVSTDCRatio->Draw("COLZ");

    return 0;
}


