#include <stdio.h>
#include "TMath"
#include <cstddef>


//
// This Macro is for debug purposes only! It scales Fpos Bin rel to Fneg Bin
//

const char Str_RootFilesButInput[] = "output/sumBut.root";
const char Str_RootFilesButOutput[] = "output/sumButFPScaled.root"; 

double ScaleFactor = 0.99;

//********************************************************************

TFile *RootFileInput, *RootFileOutput;

void Scale1D(char *fTempStr, TH1D *hRatio) {
	printf("Scale: %s\n", fTempStr);
	RootFileInput->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Multiply(hRatio);
		RootFileOutput->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void Scale1DSimple(char *fTempStr, double fRatio) {
	printf("Scale: %s\n", fTempStr);
	RootFileInput->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Scale(fRatio);
		RootFileOutput->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

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

void Scale2DSimple(char *fTempStr, double fScaleFactor) {
	printf("Scale: %s\n", fTempStr);
	RootFileInput->cd();
	TH2D *h2 = (TH2D*)gROOT->FindObject(fTempStr);
	if (h2) {
		h2->Scale(fScaleFactor);
		RootFileOutput->cd();
		h2->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

int  ScalerFPosBinDebug() {
	//Connect to all existing histograms
	RootFileInput = new TFile(Str_RootFilesButInput);

	printf("Scale by: %f\n", ScaleFactor);

//	gROOT->cd();
	RootFileOutput = new TFile(Str_RootFilesButOutput, "RECREATE");

	CopyElement("MissingMassCombinedSignal");
	CopyElement("CountNumberOfHistos");
	CopyElement("hDroppedEvents");
	CopyElement("TaggerScalerAccum");
	CopyElement("hTaggerTime");
	CopyElement("LiveTimeAccum");
	CopyElement("MissingMassCombinedSignalLTCorrected");
	Scale2DSimple("MissingMassCombinedSignalLTCorrectedFP", ScaleFactor);
	CopyElement("MissingMassCombinedSignalLTCorrectedFM");
	CopyElement("MissingMassCombinedSignalLTCorrectedTP");
	CopyElement("MissingMassCombinedSignalLTCorrectedTM");
	CopyElement("TaggerScalerAccumLTCorrectedInclDroppedEvents");
	CopyElement("TaggerScalerAccumLTCorrected");
	CopyElement ("PhotonFluxLTCorrectedWOTaggEff");
	CopyElement("PhotonFluxLTCorrected");
	CopyElement("BeamPol");
	CopyElement("TargetPolF");
	CopyElement("TargetPolT");
	CopyElement("TaggEffAbsT");
	CopyElement("TaggEffAbsF");
	CopyElement("TaggEffAbsAll");

	RootFileInput->Close();

	RootFileOutput->Write();
	RootFileOutput->Close();

	printf("Finished.\n");
}
