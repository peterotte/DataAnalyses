#include <stdio.h>
#include "TMath"
#include <cstddef>

//const char Str_RootFilesButResults[] = "../step0/output/sumBut.root"; 
//const char Str_RootFilesButResults[] = "../step0/output/sumBut1TaggChUsed.root"; //Debug: PhotonSpectra adjusted to 1 TaggerCh
const char Str_RootFilesButResults[] = "../step0/output/sumButNoPScale.root";  //Debug: No Scaling of P But
//const char Str_RootFilesButResults[] = "../step0/output/sumButFPScaled.root";  //Debug: F Pos Bin is scaled

const char Str_RootFilesHResults[]   = "../../step0/output/sumH.root";     //No Photon Flux correction between But/H
//const char Str_RootFilesHResults[]   = "../step0/output/sumH1TaggChUsed.root"; //Debug: PhotonSpectra adjusted to 1 TaggerCh
//const char Str_RootFilesHResults[]   = "../step0/output/sumH_Rescaled.root"; //Debug: Photon Flux correction between But/H

const char Str_RootFileBeamPolShape[] = "../../beampolshape/output/BeamPolValues.root";
const char Str_RootFilesResultsSignal[] = "output/results.root"; 

int Convert1Dinto2D(TH2D *fSampleHisto, TH1D *f1DInputHisto, TH2D &*f2DOutputHisto, char *fName) {
	printf("Generate: %s", fName);
	f2DOutputHisto = (TH2D*)fSampleHisto->Clone(fName);
	for (int i=1; i<=f2DOutputHisto->GetNbinsX(); i++) {
		for (int k=1; k<=f2DOutputHisto->GetNbinsY(); k++) {
			f2DOutputHisto->SetBinContent(i, k, f1DInputHisto->GetBinContent(i));
			f2DOutputHisto->SetBinError(i, k, f1DInputHisto->GetBinError(i));
		}
	}
	printf(".\n");
}

void DoPhysics() {
	Char_t Name[256];
	Char_t *VarName;

	TH2D *h2TempP, *h2TempM, *h2TempH, *h2BeamPolShape;
	TH2D *h2TempCopy[10];

	TH1D *h1PhotonFluxBut, *h1PhotonFluxH;
	TH1D *h1PhotonFluxCorrection, *h1TempCopy[10];
	TH2D *h2PhotonFluxCorrection2D;

	//Connect to all existing histograms
	TFile *RootFileButHistograms = new TFile(Str_RootFilesButResults);
	h2TempP = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrectedFP");
	h2TempM = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrectedFM");
	TH2D *h2TargetPol = (TH2D*)gROOT->FindObject("TargetPolF");
	TH2D *h2BeamPol = (TH2D*)gROOT->FindObject("BeamPol");
	h1PhotonFluxBut = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");

	TFile *RootFileHHistograms = new TFile(Str_RootFilesHResults);
	h2TempH = (TH2D*)gROOT->FindObject("MissingMassCombinedSignalLTCorrected");
	h1PhotonFluxH = (TH1D*)gROOT->FindObject("PhotonFluxLTCorrected");


	TFile *RootFileBeamPolShape = new TFile(Str_RootFileBeamPolShape);
	h2BeamPolShape = (TH2D*)gROOT->FindObject("BeamPol2D");


	//Prepare 2D histos
	TH2D *h2PhotonFluxBut, *h2PhotonFluxH;
	Convert1Dinto2D(h2TempP, h1PhotonFluxBut, h2PhotonFluxBut, "PhotonFluxBut2D");
	Convert1Dinto2D(h2TempP, h1PhotonFluxH, h2PhotonFluxH, "PhotonFluxH2D");


	//Calculate
	TFile *RootFileHistogramsSignal = new TFile(Str_RootFilesResultsSignal,"RECREATE");

	//Difference in But + and -
	TH2D *h2FluxButPosBin = (TH2D*)h2PhotonFluxBut->Clone("FluxButPosBin");
	TH2D *h2FluxButNegBin = (TH2D*)h2PhotonFluxBut->Clone("FluxButNegBin");
	//Debug purposes
//	printf("WARNING: Flux for both states altered by hand. This is only for tests.\n");
//	h2FluxButPosBin->Scale(0.5-0.0008);
//	h2FluxButNegBin->Scale(0.5+0.0008);
	h2FluxButPosBin->Scale(0.5);
	h2FluxButNegBin->Scale(0.5);
	h2TempP->Divide(h2FluxButPosBin);
	h2TempM->Divide(h2FluxButNegBin);

	h2TempCopy[0] = (TH2D*)h2TempP->Clone("ButMissingMassSignalDiff");
	h2TempCopy[0]->Add(h2TempP, h2TempM, -1, 1.);
//	h2TempCopy[0]->Scale(2);

	//Divide by H
	h2TempCopy[1] = (TH2D*)h2TempCopy[0]->Clone("ButDivH");
	h2TempCopy[1]->Divide(h2TempH);
	h2TempCopy[1]->Multiply(h2PhotonFluxH);


	//Divide by H, Beamshape of circ. pol. Photons
	h2TempCopy[2] = (TH2D*)h2TempCopy[1]->Clone("ButDivHDivBeamShape");
	h2TempCopy[2]->Divide(h2BeamPolShape);

	//Divide by Polarisations
	//TargetPol for F
	printf("Calculating TargetPol 2D...\n");
	TH2D *hTargetPol2D = new TH2D("TargetPol2D", "TargetPol2D", 352, 0, 352, 18, 0, 180);
	for (int i=1; i<=352; i++) {
		TH1D *h1Temp = h2TargetPol->ProjectionY("Temp",i,i);
		for (int k=1; k<=18; k++) {
			hTargetPol2D->SetBinContent(i, k, h1Temp->GetMean(););
			hTargetPol2D->SetBinError(i, k, h1Temp->GetMeanError(););
		}
		delete h1Temp;
	}
	//BeamPol for F
	printf("Calculating BeamPol 2D...\n");
	TH2D *hBeamPol2D = new TH2D("BeamPol2D", "BeamPol2D", 352, 0, 352, 18, 0, 180);
	for (int i=1; i<=352; i++) {
		TH1D *h1Temp = h2BeamPol->ProjectionY("Temp",i,i);
		for (int k=1; k<=352; k++) {
			hBeamPol2D->SetBinContent(i, k, h1Temp->GetMean(););
			hBeamPol2D->SetBinError(i, k, h1Temp->GetMeanError(););
		}
		delete h1Temp;
	}
	h2TempCopy[3] = (TH2D*)h2TempCopy[2]->Clone("ButDivHDivPols");
	//h2TempCopy[3]->Scale(1/0.8273); //BeamPol
	h2TempCopy[3]->Divide(hBeamPol2D);
	//h2TempCopy[3]->Scale(1/0.627); //TargetPol
	h2TempCopy[3]->Divide(hTargetPol2D);

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
//	h2TempCopy[4]->Multiply(h2PhotonFluxCorrection2D);
//	h2TempCopy[4]->Multiply(h2PhotonFluxH);
//	h2TempCopy[4]->Divide(h2PhotonFluxBut);
	h2TempCopy[4]->Scale(4.599);
	


	RootFileHistogramsSignal->Write();
	RootFileHistogramsSignal->Close();
	RootFileButHistograms->Close();
	RootFileHHistograms->Close();
	RootFileBeamPolShape->Close();

	printf("Finished.\n");
}
