#include <stdio.h>
#include <TMath.h>
#include <cstddef>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

const int NTaggerElements = 352;
std::vector<double> TaggerCentralEnergy;


#include <readTaggerCalib.cc>

char tempStr[200]; //Für Strings zusammenklopfen
const char Str_RootFilesResultsSignal[] = "output/BeamPolValues.root"; 


Float_t GetHelTransfer(Float_t Egamma, Float_t E0) {
	return Egamma*(E0+1./3.*(E0-Egamma)) / (E0*E0+(E0-Egamma)*(E0-Egamma)-2./3.*E0*(E0-Egamma));
}

void CreatePolValues() {

	const Int_t NumberOfThetaBins = 18;  //alle 10° ein Bin

	//Create Output File
	TFile *RootFileHistogramsSignal = new TFile(Str_RootFilesResultsSignal,"RECREATE");
	TH1D *h1New = new TH1D("BeamPol", "BeamPol", NTaggerElements, 0, NTaggerElements);
	TH2D *h2New = new TH2D("BeamPol2D", "BeamPol2D", NTaggerElements, 0, NTaggerElements, NumberOfThetaBins, 0, 180);

	ReadTaggerConfigurationFile();

	Double_t TempValue = 0;
	const Float_t MaxBeamEnergy = 450.2;
	for (Int_t i = 0; i < NTaggerElements; i++) {
		//printf("%d %f\n",i, TaggerCentralEnergy.at(i));
		TempValue = GetHelTransfer((MaxBeamEnergy-TaggerCentralEnergy.at(i)), MaxBeamEnergy);
		h1New->SetBinContent(i+1, TempValue);
		h1New->SetBinError(i+1, 0.02);

		for (Int_t k=0; k<NumberOfThetaBins;k++) {
			h2New->SetBinContent(i+1, k+1, h1New->GetBinContent(i+1) );
			h2New->SetBinError(i+1, k+1, h1New->GetBinError(i+1) );
		}
	}

	printf("Finished.\n\n");

	RootFileHistogramsSignal->Write();
	RootFileHistogramsSignal->Close();

}
