//
// Diese Datei erzeugt aus 
// den hPIDECalibration Histo vom CBEnergyFitter
// neue Bilder

{
	TH2D *hScaler3 = (TH2D*)gROOT->FindObject("hPIDECalibration");

	char tempStr[100];

	printf("Histo found: %08x \n", hScaler3);
	hScaler3->SetAxisRange(0,1421,"X");

	delete gROOT->FindObject("CanvasPIDCalib");
	TCanvas *c3 = new TCanvas("CanvasPIDCalib");

	int Anzahl = 4;
	int TempCHToCheck[Anzahl] = { 2, 12, 14, 20};

	for (int i=0; i<Anzahl; i++) {
		int k=TempCHToCheck[i];//i+188; //+34
//		int k=i;
		sprintf(tempStr, "Time1D_TaggSc_%d", k);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler3->ProjectionX(tempStr,1+k,1+k);
		for (int k=0; k<1422;k++) hTime1D->SetBinError(k+1, 1E-10);
		hTime1D->SetLineColor(i);
		hTime1D->SetMarkerColor(i);

		if (i==0) {
			hTime1D->Draw();
			hTime1D->SetAxisRange(50,250,"Y");
		} else {
			hTime1D->Draw("SAME");
		}
	}


}
