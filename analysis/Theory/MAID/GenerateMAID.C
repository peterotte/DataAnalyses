void DownloadData() {
	const Int_t NumberOfThetaBins = 181;
	Char_t Name[256];
	Char_t* VarName;
	const Double_t ProtonMass = 938.272;
	Double_t EnergyW;
	
//1st step, because of MAID Webserver
//	for (Int_t k=145;k<=300;k++) {

//2nd step
	for (Int_t k=300;k<=400;k++) {
		printf("\nProcess for Gamma Energy %f.\n",k);
		
		EnergyW = TMath::Sqrt(2*ProtonMass*k+ProtonMass**2);

		sprintf(Name, ".! ./GetDataScript.sh %f", EnergyW);
		VarName = new Char_t[strlen(Name)+1];
		strcpy(VarName,Name);
		gROOT->ProcessLine(VarName);
	}
}






const Int_t NumberOfThetaBins = 181;
Double_t yT[NumberOfThetaBins];
Double_t yDSG[NumberOfThetaBins];
Double_t yF[NumberOfThetaBins];

void ProcessData() {
	Char_t Name[256];
	Char_t* VarName;
	
	TFile *MyRootTreeFile = new TFile("MAIDResults.root","RECREATE");
	TTree* MyMAIDTree = new TTree("MyMAIDTree","Event Tree");
	Double_t MyTreeF[NumberOfThetaBins];
	Double_t MyTreeDSG[NumberOfThetaBins];
	Double_t MyTreeT[NumberOfThetaBins];
	Double_t MyTreeGammaLabE;
	Double_t MyTreeW;
	Int_t NumberOfEvents = 0;
	const Double_t ProtonMass = 938.272;

	MyMAIDTree->Branch("MyTreeGammaLabE",&MyTreeGammaLabE,"MyTreeGammaLabE/D");
	MyMAIDTree->Branch("MyTreeW",&MyTreeW,"MyTreeW/D");

       sprintf(Name, "MyTreeT[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeT",MyTreeT,VarName);

       sprintf(Name, "MyTreeDSG[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeDSG",MyTreeDSG,VarName);

       sprintf(Name, "MyTreeF[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeF",MyTreeF,VarName);


	for (Int_t k=145;k<=400;k++) {
		printf("\nProcess for Gamma Energy %f.\n",k);
		
		MyTreeGammaLabE = k;
		MyTreeW = TMath::Sqrt(2*ProtonMass*MyTreeGammaLabE+ProtonMass**2);

		sprintf(Name, ".x MAIDData/MAIDValues_%f.C", MyTreeW);
		VarName = new Char_t[strlen(Name)+1];
		strcpy(VarName,Name);
		gROOT->ProcessLine(VarName);
		printf("%s\n",VarName);
	
//		gROOT->ProcessLine(".x MAIDData/MAIDValues.C");
	
		for (Int_t i=0;i<NumberOfThetaBins;i++) {
			MyTreeF[i] = yF[i];
			MyTreeDSG[i] = yDSG[i];
			MyTreeT[i] = yT[i];
		}

		MyMAIDTree->Fill();
	}

	MyRootTreeFile->Write();
	MyRootTreeFile->Close();
}