#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesHResultsFT[] = "output/sumH_FT.root";
const char Str_RootFilesHResultsET[] = "output/sumH_ET.root";
const char Str_RootFilesResultsSignal[] = "output/sumH.root"; 

// Number of Photons (Livetime and Taggeff corrected) FT / ET
double ScaleFactor = 0;

TFile *RootFileFT, *RootFileET, *RootFile2;

void Scale1D(char *fTempStr, Double_t fScaleFactor) {
	RootFileFT->cd();
	TH1D *h1FT = (TH1D*)gROOT->FindObject(fTempStr);

	RootFileET->cd();
	TH1D *h1ET = (TH1D*)gROOT->FindObject(fTempStr);

	if ( (h1FT) && (h1ET) ) {
		h1FT->Add(h1ET, -1.*fScaleFactor);
		RootFile2->cd();
		h1FT->Write();
	} else {
		printf("Histogram(s) %s not found.\n", fTempStr);
	}
}
void Scale1D(char *fTempStr) {
	Scale1D(fTempStr, 0);
}

void Scale2D(char *fTempStr, Double_t fScaleFactor) {
	RootFileFT->cd();
	TH2D *h2FT = (TH2D*)gROOT->FindObject(fTempStr);

	RootFileET->cd();
	TH2D *h2ET = (TH2D*)gROOT->FindObject(fTempStr);

	if ( (h2FT) && (h2ET) ) {
		h2FT->Add(h2ET, -1*fScaleFactor);
		RootFile2->cd();
		h2FT->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}
void Scale2D(char *fTempStr) {
	Scale2D(fTempStr, 0);
}

void H_ET_Subtraction() {
	//Connect to all existing histograms
	RootFileFT = new TFile(Str_RootFilesHResultsFT);
	double SumFT = ((TH1D*)(gROOT->FindObject("PhotonFluxLTCorrected")))->Integral();
	RootFileET = new TFile(Str_RootFilesHResultsET);
	double SumET = ((TH1D*)(gROOT->FindObject("PhotonFluxLTCorrected")))->Integral();
	RootFile2 = new TFile(Str_RootFilesResultsSignal, "RECREATE");

	printf("Total Photonflux: %f\t%f\n", SumFT, SumET);
	ScaleFactor = SumFT / SumET;
	printf("Scale by: %f\n", ScaleFactor);


	Scale2D("MissingMassCombinedSignal", ScaleFactor);
	Scale1D("CountNumberOfHistos");
	Scale1D("hDroppedEvents");
	Scale1D("TaggerScalerAccum");
	Scale2D("hTaggerTime");
	Scale1D("LiveTimeAccum");
	Scale2D("MissingMassCombinedSignalLTCorrected", ScaleFactor);
	Scale2D("MissingMassCombinedSignalLTCorrectedFP");
	Scale2D("MissingMassCombinedSignalLTCorrectedFM");
	Scale2D("MissingMassCombinedSignalLTCorrectedTP");
	Scale2D("MissingMassCombinedSignalLTCorrectedTM");
	Scale1D("TaggerScalerAccumLTCorrected");
	Scale1D("PhotonFluxLTCorrectedWOTaggEff");
	Scale1D("PhotonFluxLTCorrected");
	Scale1D("BeamPol");
	Scale2D("TargetPolF");
	Scale2D("TargetPolT");
	Scale1D("TaggEffAbs");
	Scale1D("TaggEffAbsAllMesons");

	RootFileFT->Close();
	RootFileET->Close();

	RootFile2->Write();
	RootFile2->Close();

	printf("Finished.\n");

	gApplication->Terminate();
}
