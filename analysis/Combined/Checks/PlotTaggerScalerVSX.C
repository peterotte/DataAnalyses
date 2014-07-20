//
// Diese Datei erzeugt aus 
// /datapool/home/otte/StabilityChecks_withPID_AllDetectors_20140322
// Verläufe für 8 Tagger Kanäle

{
	TH2D *hScaler = (TH2D*)gROOT->FindObject("hScalerVSTDCRatio_Meson");
	TH2D *hScaler2 = (TH2D*)gROOT->FindObject("hScalerVSTDCRatio_Tagg");
	TH2D *hScaler3 = (TH2D*)gROOT->FindObject("hScalerVSScalerRatio_Tagg");

	char tempStr[100];

	printf("Histo found: %08x \n", hScaler);
	hScaler->SetAxisRange(0,1392,"X");

	delete gROOT->FindObject("Canvas");
	TCanvas *c1 = new TCanvas("Canvas");

	for (int i=0; i<8; i++) {
		sprintf(tempStr, "Time1D_CB_%d", i+34);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler->ProjectionX(tempStr,35+i,35+i);
		hTime1D->SetLineColor(i);

		if (i==0) {
			hTime1D->Draw();
			hTime1D->SetAxisRange(400,1200,"Y");
		} else {
			hTime1D->Draw("SAME");
		}
	}



/////////////////////////////////////////////////////////////////////


	printf("Histo found: %08x \n", hScaler2);
	hScaler2->SetAxisRange(0,1392,"X");

	delete gROOT->FindObject("CanvasTagg");
	TCanvas *c2 = new TCanvas("CanvasTagg");

	for (int i=0; i<8; i++) {
		sprintf(tempStr, "Time1D_Tagg_%d", i+34);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler2->ProjectionX(tempStr,35+i,35+i);
		hTime1D->SetLineColor(i);

		if (i==0) {
			hTime1D->Draw();
		} else {
			hTime1D->Draw("SAME");
		}
	}

/////////////////////////////////////////////////////////////////////


	printf("Histo found: %08x \n", hScaler3);
	hScaler3->SetAxisRange(0,1392,"X");

	delete gROOT->FindObject("CanvasTaggScaler");
	TCanvas *c3 = new TCanvas("CanvasTaggScaler");

	int TempCHToCheck[14] = { 188, 189, 190, 191,195,206,34,35,37,38,39,40,41,42};

	for (int i=0; i<14; i++) {
		int k=TempCHToCheck[i];//i+188; //+34
		sprintf(tempStr, "Time1D_TaggSc_%d", k);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler3->ProjectionX(tempStr,1+k,1+k);
		for (int k=0; k<1400;k++) hTime1D->SetBinError(k+1, 1E-10);
		hTime1D->SetLineColor(i);
		hTime1D->SetMarkerColor(i);

		if (i==0) {
			hTime1D->Draw();
			hTime1D->SetAxisRange(0.0,5.0,"Y");
		} else {
			hTime1D->Draw("SAME");
		}
	}


}
