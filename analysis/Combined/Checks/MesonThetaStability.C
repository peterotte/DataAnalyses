//
// Diese Datei erzeugt aus 
// den hPIDECalibration Histo vom CBEnergyFitter
// neue Bilder

{
	TH2D *hScaler3 = (TH2D*)gROOT->FindObject("MesonTheta_Stability");

	char tempStr[100];

	printf("Histo found: %08x \n", hScaler3);
//	hScaler3->SetAxisRange(0,1420,"X");

	delete gROOT->FindObject("CanvasMesonPhiStability");
	TCanvas *c3 = new TCanvas("CanvasMesonPhiStability");

//	int Anzahl = 9;
//	int TempCHToCheck[Anzahl] = { 1,50,300,500,700,920,1030,1160,1460};

	int Anzahl = 4;
	int TempCHToCheck[Anzahl] = { 300,700,1160,1460};

	for (int i=0; i<Anzahl; i++) {
		int k=TempCHToCheck[i];//i+188; //+34
//		int k=i;
		sprintf(tempStr, "Time1D_TaggSc_%d", k);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler3->ProjectionY(tempStr,1+k,2+1+k);
//		for (int k=0; k<1422;k++) hTime1D->SetBinError(k+1, 1E-10);
		hTime1D->SetLineColor(i);
		hTime1D->SetMarkerColor(i);

		if (i==0) {
			hTime1D->Draw();
			hTime1D->SetAxisRange(0,2800,"Y");
		} else {
			hTime1D->Draw("SAME");
		}
	}


}
