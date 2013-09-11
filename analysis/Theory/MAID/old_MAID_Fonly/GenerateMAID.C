{
	const Int_t NumberOfThetaBins = 181;
	Char_t Name[256];
	Char_t* VarName;
	
	TFile *MyRootTreeFile = new TFile("MAIDResults.root","RECREATE");
	TTree* MyMAIDTree = new TTree("MyMAIDTree","Event Tree");
	Double_t MyTreeF[NumberOfThetaBins];
	Double_t MyTreeGammaLabE;
	Double_t MyTreeW;
	Int_t NumberOfEvents = 0;
	const Double_t ProtonMass = 938.272;
	Double_t y[NumberOfThetaBins];

	MyMAIDTree->Branch("MyTreeGammaLabE",&MyTreeGammaLabE,"MyTreeGammaLabE/D");
	MyMAIDTree->Branch("MyTreeW",&MyTreeW,"MyTreeW/D");
    sprintf(Name, "MyTreeF[%i]/D", NumberOfThetaBins);
    VarName = new Char_t[strlen(Name)+1];
    strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeF",MyTreeF,VarName);

	for (Int_t k=145;k<=400;k++) {
		printf("\nProcess for Gamma Energy %f.\n",k);
		
		MyTreeGammaLabE = k;
		MyTreeW = TMath::Sqrt(2*ProtonMass*MyTreeGammaLabE+ProtonMass**2);

	    sprintf(Name, ".! GetDataScript.sh %f", MyTreeW);
	    VarName = new Char_t[strlen(Name)+1];
	    strcpy(VarName,Name);
		gROOT->ProcessLine(VarName);
	
		gROOT->ProcessLine(".x MAIDValues.C");
	
		for (Int_t i=0;i<NumberOfThetaBins;i++) {
			MyTreeF[i] = y[i];
		}

		MyMAIDTree->Fill();
	}

	MyRootTreeFile->Write();
	MyRootTreeFile->Close();
}