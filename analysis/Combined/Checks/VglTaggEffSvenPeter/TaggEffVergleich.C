void TaggEffVergleich() {
	TFile *f1 = new TFile("TaggEff_Peter_Feb_Jan14.root");
	TH1D *hTagg1 = gROOT->FindObject("TaggEffMeanNorm");

	TFile *f2 = new TFile("TaggEff_Peter_Feb.root");
//	TFile *f2 = new TFile("TaggEff_Sven_Feb.root");
	TH1D *hTagg2 = gROOT->FindObject("TaggEff");

	gROOT->cd();

	TH1D *hDiff = hTagg1->Clone("Diff");
	hDiff->Divide(hTagg2);
	hDiff->Draw();
}