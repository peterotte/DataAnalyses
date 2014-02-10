#include <stdio.h>
#include "TMath"
#include <cstddef>

const char Str_RootFilesButPlus[] = "../step0/output/sumButDWithCBSumSpek.root"; 
const char Str_RootFilesButMinus[] = "../step0/output/sumButCWithCBSumSpek.root"; 

TFile *RootFileButPlus, *RootFileButMinus;

//********************************************************************

void CheckCBEnergyDistribution() {
	Char_t Name[256];
	Char_t *VarName;

	//Connect to all existing histograms
	RootFileButPlus = new TFile(Str_RootFilesButPlus);
	TH1D *h1CBESum_TargetP_TP = (TH1D*)gROOT->FindObject("CBEnergySumTP");
	TH1D *h1CBESum_TargetP_TM = (TH1D*)gROOT->FindObject("CBEnergySumTM");
	TH1D *h1CBESum_TargetP_FP = (TH1D*)gROOT->FindObject("CBEnergySumFP");
	TH1D *h1CBESum_TargetP_FM = (TH1D*)gROOT->FindObject("CBEnergySumFM");
	TH1D *h1PhotonFlux_TargetP = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	RootFileButMinus = new TFile(Str_RootFilesButMinus);
	TH1D *h1CBESum_TargetM_TP = (TH1D*)gROOT->FindObject("CBEnergySumTP");
	TH1D *h1CBESum_TargetM_TM = (TH1D*)gROOT->FindObject("CBEnergySumTM");
	TH1D *h1CBESum_TargetM_FP = (TH1D*)gROOT->FindObject("CBEnergySumFP");
	TH1D *h1CBESum_TargetM_FM = (TH1D*)gROOT->FindObject("CBEnergySumFM");
	TH1D *h1PhotonFlux_TargetM = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	gROOT->cd();

	printf("Integral Target Plus: %f\n", h1PhotonFlux_TargetP->Integral());
	printf("Integral Target Minus: %f\n", h1PhotonFlux_TargetM->Integral());


	TCanvas *c1 = new TCanvas();
	c1->Divide(2,2);
	c1->cd(1);
	h1CBESum_TargetM_TP->SetLineColor(kRed);
	h1CBESum_TargetM_TP->Scale(h1PhotonFlux_TargetP->Integral() / h1PhotonFlux_TargetM->Integral());
	h1CBESum_TargetM_TP->Draw("");
	h1CBESum_TargetP_TP->Draw("same");

	c1->cd(2);
	h1CBESum_TargetM_TM->SetLineColor(kRed);
	h1CBESum_TargetM_TM->Scale(h1PhotonFlux_TargetP->Integral() / h1PhotonFlux_TargetM->Integral());
	h1CBESum_TargetM_TM->Draw("");
	h1CBESum_TargetP_TM->Draw("same");

	c1->cd(3);
	h1CBESum_TargetM_FM->SetLineColor(kRed);
	h1CBESum_TargetM_FM->Scale(h1PhotonFlux_TargetP->Integral() / h1PhotonFlux_TargetM->Integral());
	h1CBESum_TargetM_FM->Draw("");
	h1CBESum_TargetP_FM->Draw("same");

	c1->cd(4);
	h1CBESum_TargetM_FP->SetLineColor(kRed);
	h1CBESum_TargetM_FP->Scale(h1PhotonFlux_TargetP->Integral() / h1PhotonFlux_TargetM->Integral());
	h1CBESum_TargetM_FP->Draw("");
	h1CBESum_TargetP_FP->Draw("same");

	
	//RootFileButPlus->Close();
	//RootFileButMinus->Close();

	printf("Finished.\n");
}
