//
// Diese Datei erzeugt aus 
// /datapool/home/otte/StabilityChecks_withPID_TaggerScalers_20140701.root
// Verläufe für 8 Tagger Kanäle

void CheckTaggerScalersGoodness(int MyStart=0, int MyDraw = -1) {
	static TH2D *hScaler3;
	if (hScaler3) {
		//printf("Using old Histo.\n");
	} else {
		hScaler3 = (TH2D*)gROOT->FindObject("hScalerVSScalerRatio_Tagg");
		printf("Histo found: %08x \n", hScaler3);
	}

	char tempStr[100];
	gStyle->SetPalette(1);
	
	hScaler3->SetAxisRange(0,1392,"X");

	if (MyDraw) delete gROOT->FindObject("CanvasTaggScaler");
	if (MyDraw) TCanvas *c3 = new TCanvas("CanvasTaggScaler");
	if (MyDraw) c3->Divide(2,1);

	for (int i=0; i<1; i++) {
		int k=i+MyStart; //+34
		sprintf(tempStr, "Time1D_TaggSc_%d", k);
		delete gROOT->FindObject(tempStr);
		TH1D *hTime1D = hScaler3->ProjectionX(tempStr,1+k,1+k);
		for (int kk=0; kk<1400;kk++) hTime1D->SetBinError(kk+1, 1E-10);
		hTime1D->SetLineColor(i+1);
		hTime1D->SetMarkerColor(i+1);
		
		hTime1D->Scale(1./hTime1D->Integral()*1400);

		if (MyDraw) c3->cd(1);
		if (i==0) {
			if (MyDraw) hTime1D->Draw();
			if (MyDraw) hTime1D->SetAxisRange(0.0,2.0,"Y");			
		} else {
			if (MyDraw) hTime1D->Draw("SAME");
		}
		

		////////////////////////////////////////////////////////////
		if (MyDraw) c3->cd(2);

		sprintf(tempStr, "Time1D_TaggSc_%d_test", k);
		delete gROOT->FindObject(tempStr);
		TH1D *h2 = new TH1D(tempStr, tempStr, 500, -1, 1);
		double LetzterWert = hTime1D->GetBinContent(1);
		for (int m=2;m<hTime1D->GetNbinsX(); m++) {
			if ((hTime1D->GetBinContent(m) != 0)) {
				h2->Fill(hTime1D->GetBinContent(m)-LetzterWert);
				LetzterWert = hTime1D->GetBinContent(m);
			}
		}
		
		// Verhältnis Mitte zu Ausläufern
		for (int m=2;m<h2->GetNbinsX(); m++) {
			//printf("%i: %d\n", m, h2->GetBinContent(m));
		}
		
		double SummeLinks = 0;
		double SummeGes = 0;
		for (int n=0;n<=200;n++) { SummeLinks += h2->GetBinContent(250-n)*n*1.0; SummeGes += h2->GetBinContent(250-n); }
		for (int n=0;n<=200;n++) { SummeLinks += h2->GetBinContent(251+n)*n*1.0; SummeGes += h2->GetBinContent(251+n); }
		
		double Score = SummeLinks*1./SummeGes;
		
		printf("%d\t%f\t%f\t%f\t%f\t%f\n", k, SummeLinks, SummeGes, Score, h2->GetMean(), h2->GetMeanError());
		// //
		
		
/*		sprintf(tempStr,"PrevFitTMP");
        delete gROOT->FindObject(tempStr);

        TF1 *PrevFitTMP = new TF1(tempStr,"gaus",-1,1);
        //PrevFitTMP->SetParameter(1, 0.09);

        char FitParameters[10] = "QRF0";
		strcpy(FitParameters, "R");
        TFitResultPtr MyFitResult = h2->Fit(tempStr, FitParameters); //0 = do not draw, q=quiet, R = respect range, f = special min finder

        Int_t MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data, see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
        if (MyFitStatus == 0) {
            printf("INFO: Fit result: %f +- %f\n", PrevFitTMP->GetParameter(1), PrevFitTMP->GetParError(1));
        } else {
            printf("ERROR: Fit did not converge.\n");
        }
		*/
//////////////////////////////////////////
		
		if (MyDraw) h2->Draw();
		if (MyDraw) gPad->SetLogy();
	}


}


void CheckAllChs() {

	for (int i=0; i<272; i++) {
		CheckTaggerScalersGoodness(i,0);
	}

}