const Int_t NumberOfThetaBins = 181;

//**********************************************************************

//-----------------------------------------------------------------------------

#define MASS_PROTON    938.2720
#define MASS_PIZERO    134.9766
#define MASS2_PROTON   (MASS_PROTON*MASS_PROTON)
#define MASS2_PIZERO   (MASS_PIZERO*MASS_PIZERO)

Int_t ActualTaggCh = 0;
Double_t ActualTaggE = 0;

Double_t q0(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t epsilon = (s + MASS2_PIZERO - MASS2_PROTON)/(2.0*root_s);
  if(epsilon < MASS_PIZERO) return q0(MASS_PIZERO +
MASS2_PIZERO/(2.0*MASS_PROTON));
  Double_t q = TMath::Sqrt(epsilon*epsilon - MASS2_PIZERO);

  return q;
}

Double_t omega_cm(Double_t omega_lab)
{
  Double_t s = MASS2_PROTON + 2.0*omega_lab*MASS_PROTON;
  Double_t root_s = TMath::Sqrt(s);
  Double_t omega = (s - MASS2_PROTON)/(2.0*root_s);

  return omega;
}

Double_t rho_0(Double_t omega_lab) {
  Double_t q = q0(omega_lab);
  Double_t k = omega_cm(omega_lab);

  return (q/k);
}

Double_t rho() {
	Double_t omega_lab = ActualTaggE;
	Double_t q = q0(omega_lab);
	Double_t k = omega_cm(omega_lab);

	//printf("omega_lab: %f\tq: %f\tk:%f\n", omega_lab, q, k);

	return (q/k);
}

Double_t rho_1(Double_t* x, Double_t* Param) {
  Double_t q = q0(x[0]);
  Double_t k = omega_cm(x[0]);

  return (q/k);
}

//**********************************************************************

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


void PlotTorF(Int_t AModellSelect, Int_t AVarSelect, Int_t ATaggCh) {
	if ((ATaggCh < 0) || (ATaggCh > 256)) {
		printf("wrong ATaggCh range.");
		return;
	}

	Double_t x[NumberOfThetaBins];
	Double_t y[NumberOfThetaBins];
	
	Double_t MyTreeF[NumberOfThetaBins];
	Double_t MyTreeT[NumberOfThetaBins];
	Double_t MyTreeGammaLabE;
	Double_t MyTreeW;
	
	TFile *MyRootTreeFile2;
	if (AModellSelect == 0) {
		MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/MAID/Results.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MyMAIDTree->GetEntry(ATaggCh);
	} elseif (AModellSelect == 1) {
		MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/SAID/Results.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MyMAIDTree->GetEntry(ATaggCh);
	} else {
		MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/MAIDDMT/Results.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MyMAIDTree->GetEntry(ATaggCh);
	}
	

	printf("MAID: Gamma Energy: %f MeV\n",MyTreeGammaLabE);	
	printf("MAID: CM Energy (W): %f MeV\n",MyTreeW);	
	for (Int_t i=0;i<NumberOfThetaBins;i++) {
		x[i] = i;
		if (AVarSelect == 0) {
			y[i] = MyTreeT[i];
		} else {
			y[i] = MyTreeF[i];
		}
		//printf("%f ",y[i]);
	}

	TGraph *gr;
	gr = new TGraph(NumberOfThetaBins,x,y);
	gr->SetLineColor(2+AModellSelect);
	gr->SetLineWidth(2);

	gr->Draw("SAME"); // and draw it without an axis
}

Double_t GetGammaLabE(Int_t ATaggCh) {
	if ((ATaggCh < 0) || (ATaggCh > 256)) {
		printf("wrong ATaggCh range.");
		return 0;
	}
	
	Double_t MyTreeGammaLabE;
	Double_t MyTreeGammaLabE_Saved;
	
	TFile *MyRootTreeFile2;
	MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/MAID/Results.root");
	MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
	MyMAIDTree->GetEntry(ATaggCh);
	MyTreeGammaLabE_Saved = MyTreeGammaLabE;
	MyRootTreeFile2->Close();
	
	return MyTreeGammaLabE_Saved;
}

Double_t FitFunction(Double_t* x, Double_t* Param) {
	Double_t Wert = x[0]*TMath::DegToRad();

	Double_t cos = TMath::Cos(Wert);
	Double_t sin = TMath::Sin(Wert);
	return rho()*sin*(Param[0] +
		Param[1]*cos +
		Param[2]*(3.0*cos*cos-1.0)/2.0 +
		Param[3]*(5.0*cos*cos*cos-3.0*cos)/2.0 
	);
}

void ShowResult(TH2D* h2, Int_t ATaggCh, Int_t ANBinsToCombine){
	char tempStr[1000], tempStr2[1000]; //Für Strings zusammenklopfen
	ActualTaggCh = ATaggCh;
	ActualTaggE = GetGammaLabE(ATaggCh);

	TH1D *h1P;
	sprintf(tempStr,"h1 for TaggCh %i", ATaggCh);
	sprintf(tempStr,"h1forTaggCh%i", ATaggCh);
	printf("h2: %8x  %s from: %d\n",h2, tempStr, ATaggCh);
	h1P = h2->ProjectionY(tempStr,ATaggCh,ATaggCh+ANBinsToCombine-1);

	h1P->Scale(1./(1.*ANBinsToCombine));
	sprintf(tempStr,"h_F_%i", ATaggCh);
	sprintf(tempStr2,"F (Ch_{Tagg}: %i, E_{lab}: %.1f MeV)", ATaggCh, ActualTaggE);
	if (ANBinsToCombine>1) {
		sprintf(tempStr,"h_F_%i_%i", ATaggCh, (ATaggCh+ANBinsToCombine-1));
		sprintf(tempStr2,"F (Ch_{Tagg}: %i-%i, E_{lab}: %.1f-%.1f MeV)", ATaggCh, ATaggCh+ANBinsToCombine-1, ActualTaggE, GetGammaLabE(ATaggCh+ANBinsToCombine-1));
	}
	h1P->SetNameTitle(tempStr, tempStr2);
	//h1P->SetMarkerStyle(4);
	h1P->GetYaxis()->SetRangeUser(-1.2,1.2);
	h1P->Draw();
	printf("Draw Tagg Ch %i data.\n",ATaggCh);
	printf("\n");
	
	sprintf(tempStr,"DataFit%i",ATaggCh);
	delete gROOT->FindObject(tempStr);	
	TF1 *PrevFitTMP = new TF1(tempStr,FitFunction,0,180,4);
	PrevFitTMP->SetParameter(0, 1);
	PrevFitTMP->SetParameter(1, 0);
	PrevFitTMP->SetParameter(2, 0);
	PrevFitTMP->SetParameter(3, 0);
	PrevFitTMP->SetParNames("StrengthSin","StrengthCos", "StrengthCos2", "StrengthCos3");
	TFitResultPtr MyFitResult = h1P->Fit(tempStr, "RFU"); //0 = do not draw, q=quiet, R = respect range, f = special min finder

	Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
	if (MyFitStatus == 0) {
	//	MeanTaggEff = PrevFitTMP->GetParameter(0);
	//	printf("Fit result (#%i): %f +- %f\n", k, PrevFitTMP->GetParameter(0), PrevFitTMP->GetParError(0));
	} else {
		printf("ERROR: Fit did not converge.\n");
	}

	PlotTorF(0, 1, ATaggCh); //Theory, MAID, T or F, ...
	PlotTorF(1, 1, ATaggCh); //Theory, SAID, T or F, ...
	PlotTorF(2, 1, ATaggCh); //Theory, MAIDDMT, T or F, ...
}

void PlotWithMAID() {
	TFile *MyFile = new TFile("../step1F/output/results.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("ButDivHDivPolsFlux");
	if (!h2) {
		printf("TH2D ButDivHDivPolsFlux not found.\n");
		exit(1);
	} else {
		printf("TH2D ButDivHDivPolsFlux found at 0x%08x.\n",h2);
	}
	const Int_t AnzBin = 9*3;
	const Int_t CombineNBins = 10;
	TCanvas *c1 = new TCanvas();
	c1->Divide(9,3);
	for (Int_t i=0;i<AnzBin;i++) {
		c1->cd(i+1);
		ShowResult(h2,1+i*CombineNBins, CombineNBins );
	}
	
}

void Plot(int fTaggCh, int fCombineNBins) {
	char tempStr[1000]; //Für Strings zusammenklopfen
	TFile *MyFile = new TFile("../step1F/output/results.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("ButDivHDivPolsFlux");
	if (!h2) {
		printf("TH2D ButDivHDivPolsFlux not found.\n");
		exit(1);
	} else {
		printf("TH2D ButDivHDivPolsFlux found at 0x%8x.\n",h2);
	}
	gStyle->SetOptFit(); //To get the Fit Parameters into the FitPanel.
	sprintf(tempStr,"c_F_%i", fTaggCh);
	if (fCombineNBins>1) sprintf(tempStr,"c_F_%i_%i", fTaggCh, fTaggCh+fCombineNBins-1);
	delete gROOT->FindObject(tempStr);
	TCanvas *c1 = new TCanvas(tempStr);
	ShowResult(h2,1+fTaggCh, fCombineNBins);	
}
