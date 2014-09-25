//
// Diese Datei erzeugt aus 
// /datapool/home/otte/NewAnalysis/analysis/Combined/F/step0/output/sumBut.root
// für alle Taggerkanäle das Verhältnis Zufällig zu Prompt

TH2D *hTaggTime = 0;

void CheckTaggerTDCRatio(int MyStart=0, int MyDraw = -1) {
	if (hTaggTime) {
		//printf("Using old Histo.\n");
	} else {
		hTaggTime = (TH2D*)gROOT->FindObject("hTaggerTime");
		printf("Histo found: %08x \n", hTaggTime);
	}

	char tempStr[100];
	gStyle->SetPalette(1);
	
	hTaggTime->SetAxisRange(-200,200,"X");

	if (MyDraw) delete gROOT->FindObject("CanvasTaggRatio");
	if (MyDraw) TCanvas *c3 = new TCanvas("CanvasTaggRatio");

	sprintf(tempStr, "Time1D_TaggRatio_%d", MyStart);
	delete gROOT->FindObject(tempStr);
	TH1D *hTime1D = hTaggTime->ProjectionX(tempStr,1+MyStart,1+MyStart);
//	for (int kk=0; kk<1400;kk++) hTime1D->SetBinError(kk+1, 1E-10);
	//hTime1D->SetLineColor(1);
	//hTime1D->SetMarkerColor(i+1);
	
	//hTime1D->Scale(1./hTime1D->Integral()*1400);

	if (MyDraw) hTime1D->Draw();

	int Mitte = hTime1D->GetNbinsX()/2;
	double Error = 0;

	//Prompt
	double PromptFehler = 0;
	double PromptLaenge = Mitte+15 - (Mitte-15) +1;
	double PromptIntegral = hTime1D->IntegralAndError(Mitte-15, Mitte+15, Error);
	PromptFehler = Error;
	if (MyDraw) printf("Prompt: %f (%f)\t %f\n", PromptIntegral, PromptFehler, PromptLaenge);

	//Zufall
	double ZufallLaenge = 401 - (Mitte+30) +1;
	double ZufallFehler = 0;
	double ZufallIntegral = hTime1D->IntegralAndError(Mitte+30, 401, Error);
	ZufallFehler = Error;
	ZufallLaenge += Mitte-30 - 1 +1;
	ZufallIntegral += hTime1D->IntegralAndError(1, Mitte-30, Error);
	ZufallFehler = sqrt(ZufallFehler*ZufallFehler + Error*Error);
	if (MyDraw) printf("Zufall: %f (%f)\t%f\n", ZufallIntegral, ZufallFehler, ZufallLaenge);
	
	//Prompt-Zufall
	double Signal = PromptIntegral-ZufallIntegral/ZufallLaenge*PromptLaenge;
	double SignalFehler = sqrt(
		PromptFehler*PromptFehler + 
		ZufallFehler*ZufallFehler /ZufallLaenge*PromptLaenge /ZufallLaenge*PromptLaenge
		);
	if (MyDraw) printf("Signal: %f\t%f\n", Signal, SignalFehler);

	//Signal / Zufall
	double SignalZufall = Signal/(ZufallIntegral/ZufallLaenge*PromptLaenge);
	double SignalZufallFehler = SignalZufall * 
			sqrt( SignalFehler*SignalFehler/Signal/Signal + 
				 ZufallFehler*ZufallFehler/ZufallIntegral/ZufallIntegral);
	printf("Signal/Zufall: %d\t%f\t%f\n", 
		MyStart, SignalZufall, SignalZufallFehler);

	TH1D *h1 = (TH1D*)gROOT->FindObject("AllTaggCh");
	if (!TMath::IsNaN(SignalZufall)) {
		printf("yeah");
		h1->SetBinContent(MyStart+1, SignalZufall);
		h1->SetBinError(MyStart+1, SignalZufallFehler);
	}
}


void CheckTaggerTDCPrmptToBg(int MyDraw = 0) {

	if (MyDraw) delete gROOT->FindObject("CanvasAllTaggerCh");
	if (MyDraw) TCanvas *c1 = new TCanvas("CanvasAllTaggerCh");
	delete gROOT->FindObject("AllTaggCh");
	TH1D *h1 = new TH1D("AllTaggCh", "AllTaggCh", 272, 0, 271);

	for (int i=0; i<272; i++) {
		CheckTaggerTDCRatio(i,0);
	}
	h1->Draw();

}
