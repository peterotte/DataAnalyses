const Int_t NumberOfThetaBins = 181;

void PlotTheoryModell(Int_t AModellSelect, Int_t AVariable) {
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
	} elseif (AModellSelect == 1) {
		MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/SAID/Results.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
	} else {
		MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/MAIDDMT/Results.root");
		MyMAIDTree->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
		MyMAIDTree->SetBranchAddress("MyTreeF",&MyTreeF);
		MyMAIDTree->SetBranchAddress("MyTreeT",&MyTreeT);
		MyMAIDTree->SetBranchAddress("MyTreeW",&MyTreeW);
	}
	

	char str[1000];
	if (AVariable) {
		sprintf(str, "Observable T, Modell %d", AModellSelect);
	} else {
		sprintf(str, "Observable F, Modell %d", AModellSelect);
	}
	printf("%s\n", str);
	TH2D *h2 = new TH2D("Modell", str, 257, 0, 257, NumberOfThetaBins, -0.5, 180.5);

	Int_t nentries = (Int_t)MyMAIDTree->GetEntries();
	for (Int_t k=0; k<nentries; k++) {
		MyMAIDTree->GetEntry(k);

		for (Int_t i=0;i<NumberOfThetaBins;i++) {
			if (AVariable) {
				h2->Fill(k+0.5, i, MyTreeT[i]);
			} else {
				h2->Fill(k+0.5, i, MyTreeF[i]);
			}
		}
	}

	h2->Draw("COLZ");
}

void PlotTheory() {
	gStyle->SetPalette(1);
	gStyle->SetOptStat(0);
	TCanvas *c1 = new TCanvas();
	const Int_t NModels = 3;
	c1->Divide(NModels,2);
	c1->cd(1);
	PlotTheoryModell(0,0);
	c1->cd(NModels+1);
	PlotTheoryModell(0,1);

	c1->cd(2);
	PlotTheoryModell(1,0);
	c1->cd(NModels+2);
	PlotTheoryModell(1,1);

	c1->cd(3);
	PlotTheoryModell(2,0);
	c1->cd(NModels+3);
	PlotTheoryModell(2,1);


}
