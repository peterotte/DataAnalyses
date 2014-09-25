//
// Passt die beiden Photonen-Flüsse von Butanol und Wasserstoff aufeinander an.
//

#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButResults[] = "output/sumBut.root"; 
const char Str_RootFilesHResults[]   = "../../step0/output/sumH.root"; 
const char Str_RootFilesHResults_Rescaled[] = "output/sumH_Rescaled.root"; 

TFile *RootFileButHistograms, *RootFileHHistograms, *RootFileHHistograms_Rescaled;

//********************************************************************
// Debug Option: Set Flux Correction to 1 for all channels

int DebugOptionAllChannelsRatioEqual = 0; //0 = Debug disabled, scaling for each ch individually

//********************************************************************

void Scale1D(char *fTempStr, TH1D *hRatio) {
	printf("Scale: %s.\n", fTempStr);
	RootFileHHistograms->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Multiply(hRatio);
		RootFileHHistograms_Rescaled->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void Scale1DSimple(char *fTempStr, double fRatio) {
	printf("Scale: %s.\n", fTempStr);
	RootFileHHistograms->cd();
	TH1D *h1 = (TH1D*)gROOT->FindObject(fTempStr);
	if (h1) {
		h1->Scale(fRatio);
		RootFileHHistograms_Rescaled->cd();
		h1->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

void CopyElement(char *fTempStr) {
	printf("Copy: %s.\n", fTempStr);
	RootFileHHistograms->cd();
	TObject *o = gROOT->FindObject(fTempStr);
	if (o) {
		RootFileHHistograms_Rescaled->cd();
		o->Write();
	} else {
		printf("Object %s not found.\n", fTempStr);
	}
}

void Scale2D(char *fTempStr, TH1D *hRatio) {
	printf("Scale: %s.\n", fTempStr);
	RootFileHHistograms->cd();
	TH2D *h2 = (TH2D*)gROOT->FindObject(fTempStr);
	if (h2) {
		for (int i=0; i<=(h2->GetNbinsX()+1); i++) {
			for (int k=0; k<=(h2->GetNbinsY()+1); k++) {
				h2->SetBinContent(i, k, h2->GetBinContent(i, k) * hRatio->GetBinContent(i));
				h2->SetBinError(i, k, h2->GetBinError(i, k) * hRatio->GetBinError(i));
			}
		}
		RootFileHHistograms_Rescaled->cd();
		h2->Write();
	} else {
		printf("Histogram %s not found.\n", fTempStr);
	}
}

//****************************************************************

void RescaleH() {
	Char_t Name[256];
	Char_t *VarName;

	TH1D *h1PhotonFluxBut, *h1PhotonFluxH;
	TH1D *h1PhotonFluxCorrection;

	//Connect to all existing histograms
	RootFileButHistograms = new TFile(Str_RootFilesButResults);
	h1PhotonFluxBut = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	RootFileHHistograms = new TFile(Str_RootFilesHResults);
	h2TempH = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrected");
	h1PhotonFluxH = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	//Calculate
	RootFileHHistograms_Rescaled = new TFile(Str_RootFilesHResults_Rescaled,"RECREATE");

	//Ratio of Photon Flux betwen Combined But and H
	TH1D *hFluxRatio = (TH1D*)h1PhotonFluxBut->Clone("PhotonFluxRatio");
	hFluxRatio->Divide(h1PhotonFluxH);
	for (int i=1;i<=(hFluxRatio->GetNbinsX());i++) {
		if (hFluxRatio->GetBinContent(i) > 1E3) {
			hFluxRatio->SetBinContent(i,0);
			hFluxRatio->SetBinError(i,0);
		}
	}
	TF1 *AverageRatioFit = new TF1("AvgRatioFit","[0]",0,351);
	TFitResultPtr MyFitResult = hFluxRatio->Fit("AvgRatioFit", "0qFUW"); //0 = do not draw, q=quiet, R = respect range, f = special min finder, W=Set all weights to 1 for non empty bins; ignore error bars
	Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, 
					     //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
	double AverageRatio = 0;
	if (MyFitStatus == 0) {
		AverageRatio = AverageRatioFit->GetParameter(0);
		printf("Flux Ratio Fit result: %f +- %f\n", AverageRatioFit->GetParameter(0), AverageRatioFit->GetParError(0));
	} else {
		printf("ERROR: Fit did not converge.\n");
	}
	double ScaleFactor = 1/AverageRatio;
	hFluxRatio->Scale(ScaleFactor);

	if (DebugOptionAllChannelsRatioEqual) {
		printf("WARNING: Debug Option active: Flux Ratio is set to 1 for channels.\n");
		for (int i=1;i<=(hFluxRatio->GetNbinsX());i++) {
			hFluxRatio->SetBinContent(i,1);
			hFluxRatio->SetBinError(i,0);
		}
	}

	//Now Rescale the H-Results individually
	Scale2D("MissingMassCombinedSignal", hFluxRatio); //This histo is needed in the next step
	CopyElement("CountNumberOfHistos");
	CopyElement("hDroppedEvents");
	Scale1D("TaggerScalerAccum", hFluxRatio);
	Scale2D("hTaggerTime", hFluxRatio);
	CopyElement("LiveTimeAccum");
	Scale2D("MissingMassCombinedSignalLTCorrected", hFluxRatio);
	Scale1D("TaggerScalerAccumLTCorrected", hFluxRatio);
	Scale1D("PhotonFluxLTCorrectedWOTaggEff", hFluxRatio);
//For Debug Reasons, to test a different H-Flux:
//	printf("WARNING: Flux ratio not adapted by hand.\n");
//	hFluxRatio->Scale(0.8);
	Scale1D("PhotonFluxLTCorrected", hFluxRatio);  // This histo ist needed in the next step
	CopyElement("TaggEffAbsAll");

	RootFileHHistograms_Rescaled->Write();
	RootFileButHistograms->Close();
	RootFileHHistograms->Close();
	RootFileHHistograms_Rescaled->Close();

	printf("Finished.\n");
}
