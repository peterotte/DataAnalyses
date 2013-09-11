


int CombineAllMeasurements() {
    Printf("------------------------------");
    Printf("INFO: Plot start.");

    if (NTaggEffRunsMetaInformation == 0) {
        Printf("ERROR: NTaggEffRunsMetaInformation = 0");
        return 0;
    }

    delete gROOT->FindObject("All_M");
    TH2D *hAllM = new TH2D("All_M", "All_M", NTaggEffRunsMetaInformation, 0, NTaggEffRunsMetaInformation, NTaggChs, 0, NTaggChs);

    delete gROOT->FindObject("MPresent");
    TH1D *hMPresent = new TH1D("MPresent", "MPresent", NTaggChs, 0, NTaggChs); //Used to store number of used TaggEff measurements

    //Mean TaggEff values of all Measurements
    delete gROOT->FindObject("MeanTaggEff");
    TH1D *hMeanTaggEff = new TH1D("MeanTaggEff", "MeanTaggEff", NTaggEffRunsMetaInformation, 0, NTaggEffRunsMetaInformation);

    Printf("INFO: Read TaggEff Config for RunID: %i ",TaggEffRunsMetaInformation.at(0).RunMeasurement);
    ReadTaggEffConfigurationFile(TaggEffRunsMetaInformation.at(0).RunMeasurement);
    Printf("---------------------------------");
    //Fill 2D histo
    for (Int_t k = 0; k <NTaggEffRunsMetaInformation; k++) {
        Char_t buffer[1024];
        sprintf(buffer, "%s/TaggEff_M_%i.root", TaggEffOutputPath, RunsMetaInformation.at(TaggEffRunsMetaInformation.at(k).RunMeasurement).RunNumber);
        printf("File %5i: ", TaggEffRunsMetaInformation.at(k).RunMeasurement);
        printf("Read %s\n",buffer);
        TFile *fActFile = new TFile(buffer);
        if( !fActFile ){ printf("File not found.\n"); return(0); }
        fActFile->cd("Output");
        gDirectory->cd("TaggEff");
        TH1D *hTaggEff = (TH1D*)gDirectory->Get("TaggEffBgFreeC"); //Use the histogram, where outliners are already deleted
        if( !hTaggEff ) printf("TaggEff Histo not found.\n");

        for (int i=1;i<=NTaggChs;i++) {
            hAllM->SetBinContent(k+1,i,hTaggEff->GetBinContent(i));
            hAllM->SetBinError(k+1,i,hTaggEff->GetBinError(i));
            if (hTaggEff->GetBinContent(i) > 0) {
                hMPresent->Fill(i-1);
            }
            hMPresent->SetBinError(i,0);
        }

        fActFile->Close();


        //Do fit for mean value of single taggeff measurement
        TH1D *TempHist = hAllM->ProjectionY("TempHist", 1+k, 1+k, "e"); // e = errors are computed. see: http://root.cern.ch/root/html/TH2.html#TH2:ProjectionY

        Char_t TempStr[1024];
        sprintf(TempStr,"PrevFitTMP%i",k);
        delete gROOT->FindObject(TempStr);
        TF1 *PrevFitTMP = new TF1(TempStr,"pol0",1,352);
        PrevFitTMP->SetParameter(0, 0.09);
        TFitResultPtr MyFitResult = TempHist->Fit(TempStr, "QR0F"); //0 = do not draw, q=quiet, R = respect range, f = special min finder

        Double_t MeanTaggEff = 0;
        Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
        if (MyFitStatus == 0) {
            MeanTaggEff = PrevFitTMP->GetParameter(0);
            TaggEffRunsMetaInformation.at(k).MeanTaggEff = MeanTaggEff;
            printf("Fit result (#%i): %f +- %f\n", k, PrevFitTMP->GetParameter(0), PrevFitTMP->GetParError(0));
        } else {
            printf("ERROR: Fit did not converge (%i).\n", k);
        }

        delete TempHist;

        hMeanTaggEff->SetBinContent(k+1, MeanTaggEff);

    }



    delete gROOT->FindObject("TaggEffMeanTemp"); //Used for the sum, just for temporal purposes
    TH1D *hTaggEffMeanT = hAllM->ProjectionY("TaggEffMeanTemp", 0, -1, "e"); // e = errors are computed. see: http://root.cern.ch/root/html/TH2.html#TH2:ProjectionY

    delete gROOT->FindObject("TaggEffMean");
    TH1D *hTaggEffMean = (TH1D*) hTaggEffMeanT->Clone("TaggEffMean");
    hTaggEffMean->Divide(hMPresent);

    // ***********************************************************
    // Calculate the scale to norm TaggEffMean
    char buffer[1024] = "PrevFitTMPScaleMean";
    delete gROOT->FindObject(buffer);
    TF1 *PrevFitTMP = new TF1(buffer,"pol0",1,352);
    //PrevFitTMP->SetParameter(0, 0.1);
    TFitResultPtr MyFitResult = hTaggEffMean->Fit(buffer, "QR0F"); //0 = do not draw, q=quiet, R = respect range, f = special min finder

    Double_t MeanTaggEffAvg = 1;
    Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        MeanTaggEffAvg = PrevFitTMP->GetParameter(0);
        printf("INFO: MeanTaggEffAvg Fit result: %f +- %f\n", PrevFitTMP->GetParameter(0), PrevFitTMP->GetParError(0));
    } else {
        printf("ERROR: MeanTaggEffAvg Fit did not converge.\n");
    }


    delete gROOT->FindObject("TaggEffMeanNorm");
    TH1D *hTaggEffMeanN = (TH1D*) hTaggEffMean->Clone("TaggEffMeanNorm");

    hTaggEffMeanN->Scale(1/MeanTaggEffAvg);

    Printf("------------------------------");
    return 0;
}

int PlotAllMeasurements() {

    TCanvas *c1 = new TCanvas("MyCanvas");
    c1->Divide(2,3);
    gStyle->SetPalette(1);

    c1->cd(1);
    ((TH1D*)gROOT->FindObject("All_M"))->Draw("COLZ");
    c1->cd(2);
    ((TH1D*)gROOT->FindObject("MPresent"))->Draw();
    c1->cd(3);
    ((TH1D*)gROOT->FindObject("TaggEffMeanTemp"))->Draw();
    c1->cd(4);
    ((TH1D*)gROOT->FindObject("TaggEffMean"))->Draw();
    c1->cd(5);
    ((TH1D*)gROOT->FindObject("TaggEffMeanNorm"))->Draw();
    c1->cd(6);
    ((TH1D*)gROOT->FindObject("MeanTaggEff"))->Draw();

    return 0;
}


int PlotTaggEffTrend(int fBeamTimeBlock) {
    //********************************************************
    //Plot the second canvas with ratio and taggeff measurements at the same time

    TH1D *h1 = new TH1D("TaggEffRatio0","TaggEffRatio0",1000,0,1000);
    TH1D *h1TaggEff = new TH1D("TaggEffMeasurements","TaggEffMeasurements",1000,0,1000);


    Int_t CounterSep10 = 0;

    Int_t MetaInfoRunNumber;
    Float_t MetaInfoLaddP2Ratio;
    Float_t MetaInfoAbsTaggEff;

    for (Int_t i=0; i<NRunsMetaInformation; i++) {
        if (RunsMetaInformation.at(i).BeamTimeBlock == fBeamTimeBlock ) {
            MetaInfoRunNumber = RunsMetaInformation.at(i).RunNumber;
            MetaInfoLaddP2Ratio = RunsMetaInformation.at(i).LaddP2Ratio;
            MetaInfoAbsTaggEff = RunsMetaInformation.at(i).AbsTaggEff;


            if ( (RunsMetaInformation.at(i).RunType == 0) && (RunsMetaInformation.at(i).AcquCrash == 0) ) { //(RunType = 0=Prod.Run, 1=TaggEff, 2=Other/badFile) //AcquCrash = 0 = no, 1 = yes
                CounterSep10++;
                if (MetaInfoLaddP2Ratio != -1) {
                    h1->SetBinContent(CounterSep10, MetaInfoAbsTaggEff);
                    h1->SetBinError(CounterSep10,0);
                }
            }

            for (Int_t k=0; k<NTaggEffRunsMetaInformation; k++) {
                if ( i == TaggEffRunsMetaInformation.at(k).CorrespodingRun ) {
                    h1TaggEff->SetBinContent(CounterSep10, TaggEffRunsMetaInformation.at(k).MeanTaggEff);
                    h1TaggEff->SetBinError(CounterSep10, 0.05*TaggEffRunsMetaInformation.at(k).MeanTaggEff);
                }
            }

        }

    }

    new TCanvas;
    h1->SetLineColor(1);
    //h1->Scale(1/OptionalRatioScale);
    h1->Draw();
    h1TaggEff->SetLineColor(2);
    h1TaggEff->SetMarkerColor(2);
    h1TaggEff->SetMarkerStyle(20);
    h1TaggEff->Draw("SAME");

    Printf("INFO: Plot end.");
    Printf("------------------------------");

    return(0);
}


//****************************************************************************


void minuitF(Int_t &nDim, Double_t *gout, Double_t &result, Double_t par[], Int_t ierflg){

    Double_t scale=par[0];
    Double_t chi2=0;

    for (int i=0; i<NTaggEffRunsMetaInformation; i++) {
        if (TaggEffRunsMetaInformation.at(i).CorrespodingRun >= 0) {
            //printf("Corresponding run: %d\thas TaggEff: %f \n", TaggEffRunsMetaInformation.at(i).CorrespodingRun, RunsMetaInformation.at(TaggEffRunsMetaInformation.at(i).CorrespodingRun).AbsTaggEff);

            Double_t Wert1 = RunsMetaInformation.at(TaggEffRunsMetaInformation.at(i).CorrespodingRun).AbsTaggEff;
            Double_t Wert2 = TaggEffRunsMetaInformation.at(i).MeanTaggEff;
    //        chi2 += pow(werte1[i]- scale*werte2[i], 2.) / ( pow(werte1[i]*0.1, 2.) + pow(scale*werte2[i]*0.1, 2.) );
            chi2 += pow(Wert1- scale*Wert2, 2.);
        }
    }
    //printf("INFO: Scale: %f\n", scale);

    result = chi2;
}




void FindOptionalScale() {
    printf("\n\nINFO: Start finding optimal Scale for 1/LaddP2 and TaggEff measurements.\n");

    TMinuit *mini=new TMinuit(1);
    mini->SetFCN(minuitF);
    mini->SetPrintLevel(-1); //Verbose level, -1 will give very little output at all
    //mini->SetObjectFit(*this);

    Double_t pars[1] = {1.};
    Int_t ierflg = 0;

    Double_t resParas[1];
    Double_t errors[1];
    Double_t fcn = 0;


    // void mnparm(Int_t k, TString cnamj, Double_t uk, Double_t wk, Double_t a, Double_t b, Int_t& ierflg)
    // Implements one parameter definition*-*-*-
    // *-*              ===================================
    // *-*        Called from MNPARS and user-callable
    // *-*    Implements one parameter definition, that is:
    // *-*          K     (external) parameter number
    // *-*          CNAMK parameter name
    // *-*          UK    starting value
    // *-*          WK    starting step size or uncertainty
    // *-*          A, B  lower and upper physical parameter limits
    // *-*    and sets up (updates) the parameter lists.
    // *-*    Output: IERFLG=0 if no problems
    // *-*                  >0 if MNPARM unable to implement definition
    // *

    mini->mnparm(0, "par0", pars[0], .01, 0, 0, ierflg); // parameter Number and Name, start Value and Step size, Lower and Upper limit

    mini->mnexcm("MIGRAD", 0, 0, ierflg);
    for(int i=0;i<1;i++) gMinuit->GetParameter(i,resParas[i],errors[i]);
        fcn=gMinuit->fAmin; //hier kannst du dir das chi2 abholen

    OptionalRatioScale = resParas[0];
    printf("INFO: Optimal Scale: %f +- %f\n", resParas[0], errors[0]);

    mini->Delete();
    printf("INFO: Finding Optimal Scale completed.\n\n");
}


//*******************************************************************

void ChangeAbsTaggEffValuestoScale(int fSelectedBeamtime) {
    for (int i=0; i<NRunsMetaInformation; i++) {
        if ( (RunsMetaInformation.at(i).RunType != 1) && (fSelectedBeamtime == RunsMetaInformation.at(i).BeamTimeBlock) ) {
            RunsMetaInformation.at(i).AbsTaggEff = RunsMetaInformation.at(i).AbsTaggEff / OptionalRatioScale ;
        }
    }
}

