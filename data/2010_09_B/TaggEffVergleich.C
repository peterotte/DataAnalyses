void TaggEffVergleich() {
	TFile *f1 = new TFile("TaggEffNorm_Feb14.root");
	TH1D *hTagg1 = gROOT->FindObject("TaggEffMeanNorm");

	TFile *f2 = new TFile("TaggEffNorm.root");
	TH1D *hTagg2 = gROOT->FindObject("TaggEffMeanNorm");

	TH1D *hDiff = hTagg1->Clone("Diff");
	hDiff->Divide(hTagg2);
	hDiff->Draw();
}