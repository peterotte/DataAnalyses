const Int_t NumberOfThetaBins = 181;

void PlotTorF(Int_t AModellSelect, Int_t AVarSelect, Int_t ATaggCh, Double_t AScaleFactor) {
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
			y[i] = AScaleFactor*MyTreeT[i];
		} else {
			y[i] = AScaleFactor*MyTreeF[i];
		}
		//printf("%f ",y[i]);
	}

	TGraph *gr;
	gr = new TGraph(NumberOfThetaBins,x,y);
	gr->SetLineColor(2+AModellSelect);
	gr->SetLineWidth(2);

	gr->Draw("SAME"); // and draw it without an axis
}

void PlotFullTorF(Int_t AModellSelect, Int_t AVarSelect, Int_t AGammaEnergy, Double_t AScaleFactor) {
	new TCanvas();
	TH1D *sdf = new TH1D("sd","sdf",18,0,NumberOfThetaBins);
	sdf->Draw();
	sdf->GetYaxis()->SetRangeUser(-1,1);
	PlotTorF(AModellSelect, AVarSelect, AGammaEnergy, AScaleFactor);
}




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

void ShowResult(TH2D* h2, Int_t ATaggCh, Double_t AScaleFactor){
	char tempStr[200]; //Für Strings zusammenklopfen

	TH1D *h1P;
	sprintf(tempStr,"h1 for TaggCh %i", ATaggCh);
	sprintf(tempStr,"h1forTaggCh%i", ATaggCh);
	printf("h2: %8x  %s from: %d\n",h2, tempStr, ATaggCh);
	h1P = h2->ProjectionY(tempStr,ATaggCh,ATaggCh+9);

	h1P->Scale(1./10.);
	sprintf(tempStr,"h1_%i", ATaggCh);
	h1P->SetName(tempStr);
	h1P->Scale(AScaleFactor);
	//h1P->SetMarkerStyle(4);
	h1P->GetYaxis()->SetRangeUser(-1.2,1.2);
	h1P->Draw();
	printf("Draw Tagg Ch %i data.\n",ATaggCh);

/*	FILE* RootFilesOutput ;
	RootFilesOutput = fopen("output/F_Results.dat", "a");
	fprintf(RootFilesOutput, "Draw %i MeV (bin %i) data.\n",ATaggCh, ATaggCh);
	for (Int_t i=1; i <= 18; i++) {
		fprintf(RootFilesOutput, "%i\t%f\t%f\n", (i-1)*10+5,h1P->GetBinContent(i),h1P->GetBinError(i));
	}
	fclose(RootFilesOutput);
*/
	
	PlotTorF(2, 0, ATaggCh, 1); //Theory, MAIDDMT, T or F, ...

	if (ATaggCh >2) {
	//	PlotSvenF(ABinN);
	//	PlotPeterF(ABinN);
	}

	printf("\n");
}

void PlotWithMAID() {
	TFile *MyFile = new TFile("../step1T/output/results.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("ButDivHDivPolsFlux");
	if (!h2) {
		printf("TH2D ButDivHDivPolsFlux not found.\n");
		exit(1);
	} else {
		printf("TH2D ButDivHDivPolsFlux found at 0x%8x.\n",h2);
	}
	const Int_t AnzBin = 9*3;
	Double_t Factors[AnzBin] = {
		1.0, 	1.0,	1.0,	1.0,	1.0,	1.0,	1.0,
		1.0, 	1.0,	1.0,	1.0,	1.0,	1.0,	1.0
	};
	TCanvas *c1 = new TCanvas();
	c1->Divide(9,3);
	for (Int_t i=0;i<AnzBin;i++) {
		c1->cd(i+1);
//		ShowResult(h2,i+1,Factors[i]);
		ShowResult(h2,i*10,1.);
	}
	
}
