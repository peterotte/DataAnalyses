#include <stdio.h>
#include "TMath"
#include <cstddef>


//
// This Macro is for debug purposes only! It changes the PhotonCounts
//

const char Str_RootFilesButInput[] = "../../step0/output/sumH.root";
const char Str_RootFilesButOutput[] = "output/sumH1TaggChUsed.root"; 

//********************************************************************

TFile *RootFileInput, *RootFileOutput;

void CopyElement(char *fTempStr) {
	printf("Copy: %s\n", fTempStr);
	RootFileInput->cd();
	TObject *o = gROOT->FindObject(fTempStr);
	if (o) {
		RootFileOutput->cd();
		o->Write();
	} else {
		printf("Object %s not found.\n", fTempStr);
	}
}

int SetButToUse1TaggScH() {
	//Connect to all existing histograms
	RootFileInput = new TFile(Str_RootFilesButInput);

//	gROOT->cd();
	RootFileOutput = new TFile(Str_RootFilesButOutput, "RECREATE");

	CopyElement("MissingMassCombinedSignal"); //This histo is needed in the next step
	CopyElement("CountNumberOfHistos");
	CopyElement("hDroppedEvents");
	CopyElement("TaggerScalerAccum");
	CopyElement("hTaggerTime");
	CopyElement("LiveTimeAccum");
	CopyElement("MissingMassCombinedSignalLTCorrected");
	CopyElement("TaggerScalerAccumLTCorrected");
	CopyElement("PhotonFluxLTCorrectedWOTaggEff");
//	CopyElement("PhotonFluxLTCorrected");
	CopyElement("TaggEffAbsAll");


	//Start Manipulation
	RootFileInput->cd();
	TH1D *h2 = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");
	RootFileOutput->cd();
	if (!h2) printf("Original PhotonFlux histo not found.\n");
	TH1D *h1 = (TH1D*)h2->Clone("PhotonFluxLTCorrectedOld");
	int AnpassungTaggCh = 30; //181,30
	double AnpassungAnWert = h2->GetBinContent(AnpassungTaggCh+1);
	double ObereGrenze = 414; //340;
	 // TaggerCh, Annahme, dass ein TaggerCh 1,0864 MeV entspricht
	 // TaggerCh 0 liegt dann bei 29
	double Skalierung = AnpassungAnWert * (ObereGrenze - (AnpassungTaggCh+29));
	printf("%f\t%f\n", AnpassungAnWert, Skalierung);
//	for (int i=1; i<=(h2->GetNbinsX()); i++) {
	for (int i=1; i<=272; i++) { //All eingeschalteten TaggCh
		h2->SetBinContent(i, Skalierung / (ObereGrenze - (i+29-1)) );
//		h2->SetBinContent(i, AnpassungAnWert*2 );
	}
	h2->Write();

	//End Manipulation


	RootFileInput->Close();

	RootFileOutput->Write();
	RootFileOutput->Close();

	printf("Finished.\n");
}
