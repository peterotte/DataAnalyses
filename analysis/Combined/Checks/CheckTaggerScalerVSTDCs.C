{
	//gROOT->cd("Calibration");
	//gDirectory->cd("Tagger");
	TH1D *hScaler = (TH1D*)gROOT->FindObject("TaggerScalerAccum");
	TH2D *hTime2D = (TH2D*)gROOT->FindObject("hTaggerTime");
	printf("%08x %08x\n", hScaler, hTime2D);

	TH1D *hTime1D = hTime2D->ProjectionY("Time1D",1,900);

	delete gROOT->FindObject("Canvas");
	TCanvas *c1 = new TCanvas("Canvas");

	delete gROOT->FindObject("Divide");
	TH1D* hDiff = (TH1D*)hScaler->Clone("Divide");
	
	hDiff->Divide(hTime1D);
	hDiff->Draw();
}
