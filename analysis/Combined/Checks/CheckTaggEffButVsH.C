#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesBut[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/F/step0/output/sumBut.root"; 
const char Str_RootFilesH[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumH.root"; 

TFile *RootFileBut, *RootFileH ;

//********************************************************************

void CheckTaggEffButVsH() {
	Char_t Name[256];
	Char_t *VarName;

	//Connect to all existing histograms
	RootFileBut = new TFile(Str_RootFilesBut);
	TH2D *hBut2D = (TH2D*)gROOT->FindObject("hTaggerTime");
	RootFileH = new TFile(Str_RootFilesH);
	TH2D *hH2D = (TH2D*)gROOT->FindObject("hTaggerTime");

	gROOT->cd();

	TH1D *hButBg = hBut2D->ProjectionY("TempBut1D", 800, 950);
	TH1D *hHBg = hH2D->ProjectionY("TempH1D", 800, 950);


	TCanvas *c1 = new TCanvas();
	gStyle->SetPalette(1);
	c1->Divide(2,2);
	c1->cd(1);
	hButBg->Draw("");
	c1->cd(2);
	hHBg ->Draw("");
	c1->cd(3);
	delete gROOT->FindObject("Diff");
	TH1D *hDiff = (TH1D*) hButBg->Clone("Diff");
	hDiff->Divide(hHBg);
	hDiff->Draw();
	

	RootFileBut->Close();
	RootFileH->Close();

	printf("Finished.\n");
}
