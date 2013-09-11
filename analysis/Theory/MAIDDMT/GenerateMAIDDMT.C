void DownloadData() {
	const Int_t NumberOfThetaBins = 181;
	Char_t Name[256];
	Char_t* VarName;
	const Double_t ProtonMass = 938.272;
	Double_t EnergyW;
	
//1st step, because of MAIDDMT Webserver
	for (Int_t k=145;k<=400;k++) {

//2nd step
//	for (Int_t k=340;k<=400;k++) {
		printf("\nProcess for Gamma Energy %f: ",k);
		
		EnergyW = TMath::Sqrt(2*ProtonMass*k+ProtonMass**2);

		sprintf(Name, ".! ./GetDataScript.sh %f", EnergyW);
		VarName = new Char_t[strlen(Name)+1];
		strcpy(VarName,Name);
		gROOT->ProcessLine(VarName);
	}
}






const Int_t NumberOfThetaBins = 181;
Double_t yT[NumberOfThetaBins];
Double_t yF[NumberOfThetaBins];

void ProcessData() {
	Char_t Name[256];
	Char_t* VarName;
	
	TFile *MyRootTreeFile = new TFile("MAIDDMTResults.root","RECREATE");
	TTree* MyMAIDDMTTree = new TTree("MyMAIDDMTTree","Event Tree");
	Double_t MyTreeF[NumberOfThetaBins];
	Double_t MyTreeT[NumberOfThetaBins];
	Double_t MyTreeGammaLabE;
	Double_t MyTreeW;
	Int_t NumberOfEvents = 0;
	const Double_t ProtonMass = 938.272;

	MyMAIDDMTTree->Branch("MyTreeGammaLabE",&MyTreeGammaLabE,"MyTreeGammaLabE/D");
	MyMAIDDMTTree->Branch("MyTreeW",&MyTreeW,"MyTreeW/D");
    sprintf(Name, "MyTreeT[%i]/D", NumberOfThetaBins);
    VarName = new Char_t[strlen(Name)+1];
    strcpy(VarName,Name);
	MyMAIDDMTTree->Branch("MyTreeT",MyTreeT,VarName);

    sprintf(Name, "MyTreeF[%i]/D", NumberOfThetaBins);
    VarName = new Char_t[strlen(Name)+1];
    strcpy(VarName,Name);
	MyMAIDDMTTree->Branch("MyTreeF",MyTreeF,VarName);


	for (Int_t k=145;k<=400;k++) {
		printf("\nProcess for Gamma Energy %f.\n",k);
		
		MyTreeGammaLabE = k;
		MyTreeW = TMath::Sqrt(2*ProtonMass*MyTreeGammaLabE+ProtonMass**2);

		sprintf(Name, ".x MAIDDMTData/MAIDDMTValues_%f.C", MyTreeW);
		VarName = new Char_t[strlen(Name)+1];
		strcpy(VarName,Name);
		gROOT->ProcessLine(VarName);
		printf("%s\n",VarName);
	
//		gROOT->ProcessLine(".x MAIDDMTData/MAIDDMTValues.C");
	
		for (Int_t i=0;i<NumberOfThetaBins;i++) {
			MyTreeF[i] = yF[i];
			MyTreeT[i] = yT[i];
		}

		MyMAIDDMTTree->Fill();
	}

	MyRootTreeFile->Write();
	MyRootTreeFile->Close();
}