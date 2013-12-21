#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButResultsP[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButP.root";
const char Str_RootFilesResultsSignal[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumButPScaled.root"; 

const double ScaleFactor = 1.18E+13 / 9.24E+12;

TFile *RootFile, *RootFile2;

void Scale1D(char *fTempStr) {
	RootFile->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Scale(ScaleFactor);
		RootFile2->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void Scale2D(char *fTempStr) {
	RootFile->cd();
	TH2D *h2 = (TH2D*)gROOT->FindObject(fTempStr);
	if (h2) {
		h2->Scale(ScaleFactor);
		RootFile2->cd();
		h2->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void CombineButPMScaled() {
	printf("Scale by: %f\n", ScaleFactor);

	//Connect to all existing histograms
	RootFile = new TFile(Str_RootFilesButResultsP);
	RootFile2 = new TFile(Str_RootFilesResultsSignal, "RECREATE");

	Scale2D("MissingMassCombinedSignal");
	Scale1D("CountNumberOfHistos");
	Scale1D("hDroppedEvents");
	Scale1D("TaggerScalerAccum");
	Scale2D("hTaggerTime");
	Scale1D("LiveTimeAccum");
	Scale2D("MissingMassCombinedSignalLTCorrected");
	Scale2D("MissingMassCombinedSignalLTCorrectedFP");
	Scale2D("MissingMassCombinedSignalLTCorrectedFM");
	Scale2D("MissingMassCombinedSignalLTCorrectedTP");
	Scale2D("MissingMassCombinedSignalLTCorrectedTM");
	Scale1D("TaggerScalerAccumLTCorrected");
	Scale1D("PhotonFluxLTCorrectedWOTaggEff");
	Scale1D("PhotonFluxLTCorrected");
	Scale1D("PhotonFluxFobsLTCorrected");
	Scale1D("BeamPol");
	Scale2D("TargetPolF");
	Scale2D("TargetPolT");
	Scale1D("TaggEffAbs");
	Scale1D("TaggEffAbsAllMesons");

	RootFile->Close();

	RootFile2->Write();
	RootFile2->Close();

	printf("Finished.\n");
}
