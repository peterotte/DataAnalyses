const Int_t NumberOfThetaBins = 181;

void PlotTorF(Int_t AModellSelect, Int_t AVarSelect, Int_t AGammaEnergy, Double_t AScaleFactor) {
	if ((AGammaEnergy < 145) || (AGammaEnergy > 400)) {
		printf("wrong Energy Range.");
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
		MyRootTreeFile2 = new TFile("/home/peter/analysis/Theory/MAID/MAIDResults.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MyMAIDTree->GetEntry(AGammaEnergy-145);  //145 = 0, 146 = 1, etc.
	} elseif (AModellSelect == 1) {
		MyRootTreeFile2 = new TFile("/home/peter/analysis/Theory/SAID/SAIDResults.root");
		MySAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MySAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MySAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MySAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MySAIDTree->GetEntry(AGammaEnergy-145);  //145 = 0, 146 = 1, etc.
	} else {
		MyRootTreeFile2 = new TFile("/home/peter/analysis/Theory/MAIDDMT/MAIDDMTResults.root");
		MyMAIDDMTTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDDMTTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDDMTTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDDMTTree->SetBranchAddress("MyTreeW",&MyTreeW);
		MyMAIDDMTTree->GetEntry(AGammaEnergy-145);  //145 = 0, 146 = 1, etc.
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
