void PlotSvenF(Int_t ABinN) {
	char tempStr[200]; //Für Strings zusammenklopfen

	TH1D* h1;
	TH1D *h1 = new TH1D("h1_1","h1SignalSven",18,0,180);

	strcat(tempStr, "/datapool/home/otte/results/DataOthers/Sven/SvenData/FValues_Theta10_Energy10.txt");
	printf("Read File: %s\n", tempStr);
	ifstream in1; // Erzeugen des inputsream-Objektes
	in1.open(tempStr);
	int EnergieBinNr, Energie, AnzahlKombinierterBins, ThetaBin, Theta;
	double Wert, DWert;
	while (-1) {
		in1 >> EnergieBinNr >> Energie >> AnzahlKombinierterBins >> ThetaBin >> Theta >> Wert >> DWert;
		if (!in1.good()) break; //Abort, after end of file

//		printf("%i %i %i %i %i %f %f\n", EnergieBinNr, Energie, AnzahlKombinierterBins, ThetaBin, Theta, Wert, DWert);

		if (EnergieBinNr == ABinN) {
			   h1->SetBinContent(ThetaBin+1, Wert);
			   h1->SetBinError(ThetaBin+1, DWert);
		}
	}
	h1->SetLineColor(2);
	h1->SetMarkerColor(2);
//	h1->SetMarkerStyle(20);
	h1->Draw("SAME");

	in1.close();
}

void ShowResult(TH2D* MySource, Int_t ABinN, Double_t AScaleFactor){ //ABinN = 1 = 130+-5MeV
	static TH2D *h2;
	char tempStr[200]; //Für Strings zusammenklopfen
	if (h2 == 0)	{
		if (MySource != 0) { 
			h2 = MySource;
		}{
			h2 = (TH2D*)gROOT->FindObject("h2Result_But_div_H_div_Pol_div_Consts");
		}
		gROOT->ProcessLine(".L MAIDDrawing.C");
	}
	TH1D *h1P;
	sprintf(tempStr,"h1 %iMeV (10MeV Bin)", ABinN*10+120);
	h1P = h2->ProjectionY(tempStr,ABinN,ABinN);
	sprintf(tempStr,"h1_%i", ABinN);
	h1P->SetName(tempStr);
	h1P->Scale(-1*AScaleFactor);
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
	
//	PlotTorF(0, 1, ABinN*10+120,1); //Theory, T or F, ...
//	PlotTorF(1, 1, ABinN*10+120,1); //Theory, T or F, ...
	PlotTorF(2, 1, ABinN*10+120,1); //Theory, T or F, ...

	if (ABinN >2) {
	//	PlotSvenF(ABinN);
	}

	printf("\n");
}

void PlotWithMAID() {
	TFile *MyFile = new TFile("../step1/output/results.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("ButDivHDivPolsFlux");
	TCanvas *c1 = new TCanvas();
	const Int_t AnzBin = 7*3;
	Double_t Factors[AnzBin] = {
		1.0, 	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,
		1.0, 	1.0,	1.0,	1.0,	1.0,	1.0,	1.0
	};
	c1->Divide(7,3);
	for (Int_t i=0;i<AnzBin;i++) { //0: 125..135, 1:135..145, ...
		c1->cd(i+1);
//		ShowResult(h2,i+1,Factors[i]);
		ShowResult(h2,i+1,4E11);
	}
	
}
