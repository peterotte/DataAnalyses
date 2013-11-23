void ShowResult(TH2D* MySource, Int_t ABinN, Double_t AScaleFactor){ //ABinN = 1 = 130+-5MeV
	static TH2D *h2;
	char tempStr[200]; //Für Strings zusammenklopfen
	if (h2 == 0)	{
		if (MySource != 0) { 
			h2 = MySource;
		}{
			h2 = (TH2D*)gROOT->FindObject("h2Result_But_div_H_Scaler_corrected_BeamPol_TargetPol_TaggEff");
		}
		gROOT->ProcessLine(".L MAIDDrawing.C");
	}
	TH1D *h1P;
	sprintf(tempStr,"h1 %iMeV (10MeV Bin)", ABinN*10+120);
	h1P = h2->ProjectionY(tempStr,ABinN,ABinN);
	sprintf(tempStr,"h1_%i", ABinN);
	h1P->SetName(tempStr);
	h1P->Scale(1./200.);
	h1P->Scale(AScaleFactor);
	//h1P->SetMarkerStyle(4);
	h1P->GetYaxis()->SetRangeUser(-1.2,1.2);
	h1P->Draw();
	printf("Draw %i MeV (bin %i) data.\n",ABinN*10+120, ABinN);
	FILE* RootFilesOutput ;
	RootFilesOutput = fopen("output/F_Results.dat", "a");
	fprintf(RootFilesOutput, "Draw %i MeV (bin %i) data.\n",ABinN*10+120, ABinN);
	for (Int_t i=1; i <= 18; i++) {
		fprintf(RootFilesOutput, "%i\t%f\t%f\n", (i-1)*10+5,h1P->GetBinContent(i),h1P->GetBinError(i));
	}
	fclose(RootFilesOutput);
	
	PlotTorF(0, 1, ABinN*10+120,1); //Theory, T or F, ...
	PlotTorF(1, 1, ABinN*10+120,1); //Theory, T or F, ...
	PlotTorF(2, 1, ABinN*10+120,1); //Theory, T or F, ...
	printf("\n");
}

void PlotWithMAID() {
	TFile *MyFile = new TFile("../step2/output/FinalResultButDivH.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("h2Result_But_div_H_Scaler_corrected_BeamPol_TargetPol_TaggEff");
	TCanvas *c1 = new TCanvas();
	const Int_t AnzBin = 28;
	Double_t Factors[AnzBin] = {
		0.05, 	0.1,	1.0,	0.8,	2.2,	2.5,	2.5,
		2.8,	4.6,	2.8,	4.3,	2.0,	1.5,	2.6, 	
		1.8, 	2.0, 	1.6,	2.0,	2.6,	1.7,	2.0, 	
		2.0,	1.9,	2.0,	1.4,	1.3,	1.3,	1.2
	};
	c1->Divide(7,4);
	for (Int_t i=0;i<AnzBin;i++) { //0: 125..135, 1:135..145, ...
		c1->cd(i+1);
		ShowResult(h2,i+1,Factors[i]);
//		ShowResult(h2,i+1,1);
	}
	
}
