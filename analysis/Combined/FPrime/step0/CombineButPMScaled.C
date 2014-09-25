#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButResultsP[] = "output/sumButP.root";
const char Str_RootFilesButResultsM[] = "output/sumButM.root";
const char Str_RootFilesResultsSignal[] = "output/sumButPScaled.root"; 

double ScaleFactor = 1.;

//********************************************************************
// Photon Flux Correction

// Debug Option: Set Flux Correction Ratio to 1 for all channels
int DebugOptionAllChannelsRatioEqual = 0; //0 = Debug disabled, scaling for each ch individually

//Debug Option: Scale Target-Minus Beamtime by this Factor. Normal is 1.0
int DebugOptionScaleTargetMinusBeamtime = 0; //0 = Debug option disabled
double ScaleTargetMinusBeamtime = 1.2;

//********************************************************************

TFile *RootFileP, *RootFileM, *RootFile2;

void Scale1D(char *fTempStr, TH1D *hRatio) {
	printf("Scale: %s\n", fTempStr);
	RootFileP->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Multiply(hRatio);
		RootFile2->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void Scale1DSimple(char *fTempStr, double fRatio) {
	printf("Scale: %s\n", fTempStr);
	RootFileP->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Scale(fRatio);
		RootFile2->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void CopyElement(char *fTempStr) {
	printf("Copy: %s\n", fTempStr);
	RootFileP->cd();
	TObject *o = gROOT->FindObject(fTempStr);
	if (o) {
		RootFile2->cd();
		o->Write();
	} else {
		printf("Object %s not found.\n", fTempStr);
	}
}

void Scale2D(char *fTempStr, TH1D *hRatio) {
	printf("Scale: %s\n", fTempStr);
	RootFileP->cd();
	TH2D *h2 = (TH2D*)gROOT->FindObject(fTempStr);
	if (h2) {
		for (int i=0; i<=(h2->GetNbinsX()+1); i++) {
			for (int k=0; k<=(h2->GetNbinsY()+1); k++) {
				h2->SetBinContent(i, k, h2->GetBinContent(i, k) * hRatio->GetBinContent(i));
				h2->SetBinError(i, k, h2->GetBinError(i, k) * hRatio->GetBinError(i));
			}
		}
		//h2->Scale(ScaleFactor);
		RootFile2->cd();
		h2->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

int  CombineButPMScaled() {
	//Connect to all existing histograms
	RootFileP = new TFile(Str_RootFilesButResultsP);
	TH1D *hP = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");
	double SumP = hP->Integral();
	RootFileM = new TFile(Str_RootFilesButResultsM);
	TH1D *hM = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");
	double SumM = hM->Integral();
	ScaleFactor = SumM/SumP;

	printf("Scale by: %f\n", ScaleFactor);

//	gROOT->cd();
	RootFile2 = new TFile(Str_RootFilesResultsSignal, "RECREATE");

	TH1D *hDiff = hM->Clone("PhotonFluxRatioT");
	hDiff->Divide(hP);

	if (DebugOptionScaleTargetMinusBeamtime) {
		printf("WARNING: Debug Option active: Scale Target Minus Beamtime.\n");
		ScaleFactor = ScaleFactor * ScaleTargetMinusBeamtime;
		hDiff->Scale(ScaleTargetMinusBeamtime);
		printf("Now: Scale by: %f\n", ScaleFactor);
	}

	if (DebugOptionAllChannelsRatioEqual) {
		printf("WARNING: Debug Option active: Flux Ratio is set to 1 for channels.\n");
		for (int i=1;i<=(hDiff->GetNbinsX());i++) {
			hDiff->SetBinContent(i,ScaleFactor);
			hDiff->SetBinError(i,0);
		}
	}
	//hDiff->Draw();


	Scale2D("MissingMassCombinedSignal", hDiff);
	CopyElement("CountNumberOfHistos");
	Scale1DSimple("hDroppedEvents", ScaleFactor);
	Scale1D("TaggerScalerAccum", hDiff);
	Scale2D("hTaggerTime", hDiff);
	Scale1DSimple("LiveTimeAccum", ScaleFactor);
	Scale2D("MissingMassCombinedSignalLTCorrected", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedFPrimeP", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedFPrimeM", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedTPrimeP", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedTPrimeM", hDiff);
	Scale1D("TaggerScalerAccumLTCorrectedInclDroppedEvents", hDiff);
	Scale1D("TaggerScalerAccumLTCorrected", hDiff);
	Scale1D("PhotonFluxLTCorrectedWOTaggEff", hDiff);
	Scale1D("PhotonFluxLTCorrected", hDiff);
	Scale2D("BeamPol", hDiff);
	Scale2D("TargetPolFPrime", hDiff);
	Scale2D("TargetPolTPrime", hDiff);
	Scale2D("TaggEffAbsTPrime", hDiff);
	Scale2D("TaggEffAbsFPrime", hDiff);
	Scale2D("TaggEffAbsAll", hDiff);

	//gROOT->cd();
	TH1D *hDiffAfter = hP->Clone("PhotonFluxRatioTAfter");
	hDiffAfter->Divide(hM);
	hDiffAfter->SetLineColor(kRed);
	//hDiffAfter->Draw("same");


	RootFileP->Close();
	RootFileM->Close();

	RootFile2->Write();
	RootFile2->Close();

	printf("Finished.\n");
}
