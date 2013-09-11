
void TaggEff(Int_t fback1, Int_t ffore, Int_t fback2) {

    if ( ( strlen(TaggEffOutputPath) == 0 ) || ( strlen(TaggEffInputPath) == 0 ) ) {
        Printf("ERROR: No Path information read in yet.");
        return;
    }

    // ***************************************************************
    // Read files
    // ***************************************************************

    char buffer[1024];

    //Open all 3 files for a complete taggeff measurement
    //1. file: fback1 (before real taggeff measurement)
    //2. file: ffore (real taggeff measurement)
    //3. file: fback2 (after real taggeff measurement)

    sprintf(buffer, "%s/%s.root", TaggEffInputPath, RunsMetaInformation.at(fback1).FileName.c_str());
    TFile* back1 = new TFile(buffer);
    if( !back1 ){ printf("ERROR: File %s not found\n", buffer); return; }

    sprintf(buffer, "%s/%s.root", TaggEffInputPath, RunsMetaInformation.at(ffore).FileName.c_str());
    TFile* fore = new TFile(buffer);
    if( !fore ){ printf("ERROR: File %s not found\n", buffer); return; }

    sprintf(buffer, "%s/%s.root", TaggEffInputPath, RunsMetaInformation.at(fback2).FileName.c_str());
    TFile* back2 = new TFile(buffer);
    if( !back2 ){ printf("ERROR: File %s not found\n", buffer); return; }

    sprintf(buffer, "%s/TaggEff_M_%i.root", TaggEffOutputPath, RunsMetaInformation.at(ffore).RunNumber);
    TFile* foutput = new TFile(buffer, "RECREATE");
    if( !foutput ){ printf("ERROR: File %s not found\n", buffer); return; }
    printf("TaggEff results go to: %s\n", buffer);

    //Create folder structure
    foutput->mkdir("Input");
    foutput->cd("Input");
    gDirectory->mkdir("Trigger");
    gDirectory->mkdir("Tagger");

    foutput->cd();
    foutput->mkdir("Output");
    foutput->cd("Output");
    gDirectory->mkdir("Scaler");
    gDirectory->mkdir("TDC");
    gDirectory->mkdir("TaggEff");


    printf("INFO: All 3 files loaded and the output file created.\n");

    // ***************************************************************


    // ***************************************************************
    // Connect to Histograms
    // ***************************************************************
    //TDC Hits
    TH2D *histHits2D = (TH2D*)fore->Get("hTaggerTime");
    if( !histHits2D ) printf("2D TDC histogram not found.\n");

    //Scaler
    TH1D *histSc = (TH1D*)fore->Get("TaggerScalerAccum");
    if( !histSc ) printf("Scalers histogram not found.\n");
    TH1D *histScB = (TH1D*)back1->Get("TaggerScalerAccum");
    if( !histScB ) printf("Background (before) Scalers histogram not found.\n");
    TH1D *histScB2 = (TH1D*)back2->Get("TaggerScalerAccum");
    if( !histScB2 ) printf("Background (after) Scalers histogram not found.\n");

    //Trigger Livetime Scaler
    TH1D *histTriggerSc = (TH1D*)fore->Get("LiveTimeAccum");
    if( !histTriggerSc ) printf("Trig Scalers histogram not found.\n");
    TH1D *histTriggerScB = (TH1D*)back1->Get("LiveTimeAccum");
    if( !histTriggerScB ) printf("Background (before) Trig Scalers histogram not found.\n");
    TH1D *histTriggerScB2 = (TH1D*)back2->Get("LiveTimeAccum");
    if( !histTriggerScB2 ) printf("Background (after) Trig Scalers histogram not found.\n");

    //printf("INFO: All histograms connected.\n");
    // ***************************************************************




    // ***************************************************************
    // Histogram counter for later hadd
    // ***************************************************************
    foutput->cd();
    TH1D *hCountNumberOfHistos = new TH1D("CountNumberOfHistos", "CountNumberOfHistos", 1,1,2);
    hCountNumberOfHistos->Fill(1);
    hCountNumberOfHistos->Write();
    // ***************************************************************

    // ***************************************************************
    //Save input histograms into new file
    // ***************************************************************

    //Trigger Scalers
    foutput->cd("Input");
    gDirectory->cd("Trigger");
    histTriggerScB->Write("TriggerScalerBg1");
    histTriggerSc->Write("TriggerScaler");
    histTriggerScB2->Write("TriggerScalerBg2");

    //Tagger Scalers
    //Save original Scaler histos into actual file
    foutput->cd("Input");
    gDirectory->cd("Tagger");
    histScB->Write("TaggerScalerBg1");
    histSc->Write("TaggerScaler");
    histScB2->Write("TaggerScalerBg2");

    // Tagger TDC
    // Project 2D TDC histo into 1D
    TH1D *histHits = histHits2D->ProjectionY("TaggerTDC", 0, -1, "e"); // e = errors are computed. see: http://root.cern.ch/root/html/TH2.html#TH2:ProjectionY
    histHits->Write();
    // ***************************************************************


    // ***************************************************************
    // Convert all measurements into rates
    // ***************************************************************

    if (TaggEffTaggerClockSpeed <= 0) {
        Printf("ERROR: TaggEffTaggerClockSpeed not set.");
    }
    //const Double_t TimeConversionFactor = 2071.6; //2.61E6 in 21 min
    Double_t RunTimeMeasurementB = histTriggerScB->GetBinContent(1) *1./ TaggEffTaggerClockSpeed;
    Double_t RunTimeMeasurement = histTriggerSc->GetBinContent(1) *1./ TaggEffTaggerClockSpeed;
    Double_t RunTimeMeasurementB2 = histTriggerScB2->GetBinContent(1) *1./ TaggEffTaggerClockSpeed;

    Double_t RunLiveRatioMeasurementB = histTriggerScB->GetBinContent(2) / histTriggerScB->GetBinContent(1);
    Double_t RunLiveRatioMeasurement = histTriggerSc->GetBinContent(2) / histTriggerSc->GetBinContent(1);
    Double_t RunLiveRatioMeasurementB2 = histTriggerScB2->GetBinContent(2) / histTriggerScB2->GetBinContent(1);

    printf("INFO: Time Duration B1 / M / B2 (min):  %6.1f %6.1f %6.1f min \n", RunTimeMeasurementB/60, RunTimeMeasurementB, RunTimeMeasurement/60, RunTimeMeasurement, RunTimeMeasurementB2/60, RunTimeMeasurementB2 );

    printf("INFO: Live Ratio B1 / M / B2:  %6.3f %6.3f %6.3f  \n", RunLiveRatioMeasurementB, RunLiveRatioMeasurement, RunLiveRatioMeasurementB2 );
    // ***************************************************************


    // ***************************************************************
    //Calculate Rate for all 4 histograms
    // ***************************************************************

    foutput->cd("Output");
    gDirectory->cd("Scaler");
    TH1D *histScBRate = (TH1D*)histScB->Clone("TaggerScalerBRate");
    histScBRate->Scale(1./(RunTimeMeasurementB*RunLiveRatioMeasurementB));
    histScBRate->Write();

    TH1D *histScRate = (TH1D*)histSc->Clone("TaggerScalerRate");
    histScRate->Scale(1./(RunTimeMeasurement*RunLiveRatioMeasurement));
    histScRate->Write();

    TH1D *histScB2Rate = (TH1D*)histScB2->Clone("TaggerScalerB2Rate");
    histScB2Rate->Scale(1./(RunTimeMeasurementB2*RunLiveRatioMeasurementB2));
    histScB2Rate->Write();

    //Tagger TCDs
    foutput->cd("Output");
    gDirectory->cd("TDC");
    TH1D *histHitsRate = (TH1D*)histHits->Clone("TaggerTDCRate");
    histHitsRate->Scale(1/(RunTimeMeasurement*RunLiveRatioMeasurement));
    histHitsRate->Write();
    // ***************************************************************


    // ***************************************************************
    // Calculate the mean rate of Background runs
    // ***************************************************************
    foutput->cd("Output");
    gDirectory->cd("Scaler");
    TH1D *histScAvgBRate = (TH1D*)histScBRate->Clone("TaggerScalerBAvgRate");
    histScAvgBRate->Add(histScB2Rate, 1.); //performs: this = this + c1*h1
    histScAvgBRate->Scale(1./2.);
    histScAvgBRate->Write();
    // ***************************************************************


    // ***************************************************************
    // Calculate background free rate of Middle run
    // ***************************************************************

    TH1D *histScBgFreeRate = (TH1D*)histScRate->Clone("TaggerScalerBgFreeRate");
    histScBgFreeRate->Add(histScAvgBRate, -1.); //performs: this = this + c1*h1
    histScBgFreeRate->Write();
    // ***************************************************************


    // ***************************************************************
    // Calculate the Tagg Eff
    // ***************************************************************
    foutput->cd("Output");
    gDirectory->cd("TaggEff");
    TH1D *histTaggEff = (TH1D*)histHitsRate->Clone("TaggEff");
    histTaggEff->Divide(histScRate);
    histTaggEff->Write();

    TH1D *histTaggEffBgFree = (TH1D*)histHitsRate->Clone("TaggEffBgFree");
    histTaggEffBgFree->Divide(histScBgFreeRate);
    histTaggEffBgFree->Write();

    TH1D *histTaggEffDiff = (TH1D*)histTaggEffBgFree->Clone("TaggEffRelDiff");
    histTaggEffDiff->Add(histTaggEff, -1.);
    histTaggEffDiff->Divide(histTaggEffBgFree);
    histTaggEffDiff->Write();

    // ***************************************************************

    TH1D *histMeanTaggEff = new TH1D("MeanTaggEff","MeanTaggEff",100,0,1);
    for (int i=1;i<=NTaggChs;i++) {
        if (histTaggEffBgFree->GetBinContent(i) > 0) {
            histMeanTaggEff->Fill(histTaggEffBgFree->GetBinContent(i));
        }
    }

    //Now try to determine the average tagg eff value without points to far away
    Char_t TempStr[1024];
    sprintf(TempStr,"PrevFitTMP");
    delete gROOT->FindObject(TempStr);

    TF1 *PrevFitTMP = new TF1(TempStr,"gaus",0,1);
    //PrevFitTMP->SetParameter(1, 0.09);

    TFitResultPtr MyFitResult = histMeanTaggEff->Fit(TempStr, "QRF0"); //0 = do not draw, q=quiet, R = respect range, f = special min finder

    Double_t MeanTaggEff = 0;
    Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        MeanTaggEff = PrevFitTMP->GetParameter(1);
        printf("INFO: Fit result: %f +- %f\n", PrevFitTMP->GetParameter(1), PrevFitTMP->GetParError(1));
    } else {
        printf("ERROR: Fit did not converge.\n");
    }

    histMeanTaggEff->Write();

    // ***************************************************************
    // Now kick out all outliners further away than 60% from average

    if (MyFitStatus == 0) {
        TH1D *histTaggEffBgFreeCorrected = (TH1D*)histTaggEffBgFree->Clone("TaggEffBgFreeC");

        for (int i=1;i<=NTaggChs;i++) {
            histTaggEffBgFreeCorrected->SetBinContent(i, 0);
            histTaggEffBgFreeCorrected->SetBinError(i, 0);
            if ( (histTaggEffBgFree->GetBinContent(i) < (MeanTaggEff*1.6)) && (histTaggEffBgFree->GetBinContent(i) > (MeanTaggEff*0.4)) ) {
                if ((histTaggEffBgFree->GetBinError(i) / histTaggEffBgFree->GetBinContent(i)) < 1) {
                    histTaggEffBgFreeCorrected->SetBinContent(i, histTaggEffBgFree->GetBinContent(i));
                    histTaggEffBgFreeCorrected->SetBinError(i, histTaggEffBgFree->GetBinError(i));
                }
            }
        }

        histTaggEffBgFreeCorrected->Write();
    }



    // ***************************************************************






    // ***************************************************************
    // Close files
    // ***************************************************************

    fore->Close();
    back1->Close();
    back2->Close();
    foutput->Close();

    // ***************************************************************

    Printf(" ");

}


//****************************************************************************


int DoAllTaggEffRuns(int fBeamTimeBlock) {
    Printf("------------------------------");
    Printf("INFO: All Tagg Eff Run start.");

    if (NTaggEffRunsMetaInformation==0) { Printf("ERROR: No TaggEffRuns loaded."); return 0;}
    ReadTaggEffConfigurationFile(TaggEffRunsMetaInformation.at(0).RunBackground1);

    for (int i=0; i<NTaggEffRunsMetaInformation; i++) {
        if (TaggEffRunsMetaInformation.at(i).BeamTimeBlock == fBeamTimeBlock) {
            printf("INFO: Call now TaggEff(%i, %i, %i);\n", TaggEffRunsMetaInformation.at(i).RunBackground1, TaggEffRunsMetaInformation.at(i).RunMeasurement, TaggEffRunsMetaInformation.at(i).RunBackground2);
            TaggEff(TaggEffRunsMetaInformation.at(i).RunBackground1, TaggEffRunsMetaInformation.at(i).RunMeasurement, TaggEffRunsMetaInformation.at(i).RunBackground2);
        }
    }

    Printf("INFO: All Tagg Eff Run end.");
    Printf("------------------------------");
    return(0);
}

