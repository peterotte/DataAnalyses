#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButResults[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumBut.root"; 
const char Str_RootFilesHResults[]   = "/datapool/home/otte/NewAnalysis/analysis/Combined/step0/output/sumH.root"; 
const char Str_RootFileBeamPolShape[] = "/datapool/home/otte/NewAnalysis/analysis/Combined/beampolshape/output/BeamPolValues.root";
const char Str_RootFilesResultsSignal[] = "output/results.root"; 

void DoPhysicsT() {
	Char_t Name[256];
	Char_t *VarName;

	TH2D *h2TempP, *h2TempM, *h2TempH, *h2BeamPolShape;
	TH2D *h2TempCopy[10];

	TH1D *h1PhotonFluxBut, *h1PhotonFluxH;
	TH1D *h1PhotonFluxCorrection, *h1TempCopy[10];
	TH2D *h2PhotonFluxCorrection2D;

	//Connect to all existing histograms
	TFile *RootFileButHistograms = new TFile(Str_RootFilesButResults);
	h2TempP = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrectedTP");
	h2TempM = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrectedTM");
	h1PhotonFluxBut = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	TFile *RootFileHHistograms = new TFile(Str_RootFilesHResults);
	h2TempH = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrected");
	h1PhotonFluxH = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");


	TFile *RootFileBeamPolShape = new TFile(Str_RootFileBeamPolShape);
	h2BeamPolShape = (TH2D*)gROOT->FindObject("BeamPol2D");



	//Calculate
	TFile *RootFileHistogramsSignal = new TFile(Str_RootFilesResultsSignal,"RECREATE");

	//Difference in But + and -
	h2TempCopy[0] = (TH2D*)h2TempP->Clone("ButMissingMassSignalDiff");
	h2TempCopy[0]->Add(h2TempP, h2TempM, 1., -1.);
	h2TempCopy[0]->Scale(2);

	//Divide by H
	h2TempCopy[1] = (TH2D*)h2TempCopy[0]->Clone("ButDivH");
	h2TempCopy[1]->Divide(h2TempH);

	//Divide by H
	h2TempCopy[3] = (TH2D*)h2TempCopy[1]->Clone("ButDivHDivPols");
	h2TempCopy[3]->Scale(1/0.627); //TargetPol

	//Correction for photon flux
	h1PhotonFluxCorrection = (TH1D*)h1PhotonFluxH->Clone("PhotonFluxCorrection");
	h1PhotonFluxCorrection->Divide(h1PhotonFluxBut);
	h2PhotonFluxCorrection2D = (TH2D*)h2TempCopy[3]->Clone("PhotonFluxCorrection2D");
	for (int i=1; i<=h2PhotonFluxCorrection2D->GetNbinsX(); i++) {
		for (int k=1; k<=h2PhotonFluxCorrection2D->GetNbinsY(); k++) {
			if (h1PhotonFluxBut->GetBinContent(i) < 10E5) { //Correct for inefficient Tagger channels
				h1PhotonFluxCorrection->SetBinContent(i, 0);
				h1PhotonFluxCorrection->SetBinError(i, 0);
				h2PhotonFluxCorrection2D->SetBinContent(i, k, 0);
				h2PhotonFluxCorrection2D->SetBinError(i, k, 0);
			} else {
				h2PhotonFluxCorrection2D->SetBinContent(i, k, h1PhotonFluxCorrection->GetBinContent(i));
				h2PhotonFluxCorrection2D->SetBinError(i, k, h1PhotonFluxCorrection->GetBinError(i));
			}
		}
	}
	

	//Put photon flux into calculation
	h2TempCopy[4] = (TH2D*)h2TempCopy[3]->Clone("ButDivHDivPolsFlux");
	h2TempCopy[4]->Multiply(h2PhotonFluxCorrection2D);
	h2TempCopy[4]->Scale(4.599);
	


	RootFileHistogramsSignal->Write();
	RootFileHistogramsSignal->Close();
	RootFileButHistograms->Close();
	RootFileHHistograms->Close();
	RootFileBeamPolShape->Close();

	printf("Finished.\n");
}
