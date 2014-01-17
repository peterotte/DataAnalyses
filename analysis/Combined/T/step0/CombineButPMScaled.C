#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButResultsP[] = "output/sumButP.root";
const char Str_RootFilesButResultsM[] = "output/sumButM.root";
const char Str_RootFilesResultsSignal[] = "output/sumButPScaled.root"; 

double ScaleFactor = 1.;

TFile *RootFileP, *RootFileM, *RootFile2;

void Scale1D(char *fTempStr, TH1D *hRatio) {
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


	gROOT->cd();
	TH1D *hDiff = hM->Clone("PhotonFluxRatioT");
	hDiff->Divide(hP);
	//hDiff->Draw();

	RootFile2 = new TFile(Str_RootFilesResultsSignal, "RECREATE");

	Scale2D("MissingMassCombinedSignal", hDiff);
	CopyElement("CountNumberOfHistos");
	Scale1DSimple("hDroppedEvents", ScaleFactor);
	Scale1D("TaggerScalerAccum", hDiff);
	Scale2D("hTaggerTime", hDiff);
	Scale1DSimple("LiveTimeAccum", ScaleFactor);
	Scale2D("MissingMassCombinedSignalLTCorrected", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedFP", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedFM", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedTP", hDiff);
	Scale2D("MissingMassCombinedSignalLTCorrectedTM", hDiff);
	Scale1D("TaggerScalerAccumLTCorrected", hDiff);
	Scale1D("PhotonFluxLTCorrectedWOTaggEff", hDiff);
	Scale1D("PhotonFluxLTCorrected", hDiff);
	Scale1DSimple("BeamPol", ScaleFactor);
	Scale2D("TargetPolF", hDiff);
	Scale2D("TargetPolT", hDiff);
	Scale1DSimple("TaggEffAbs", ScaleFactor);
	Scale1DSimple("TaggEffAbsAllMesons", ScaleFactor);

	gROOT->cd();
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
