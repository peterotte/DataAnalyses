{
	TFile *f1 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1487.root");
//	TFile *f1 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1507.root");
	gDirectory->cd("Output");
	gDirectory->cd("TaggEff");
	TH1D *h1 = (TH1D*)gROOT->FindObject("TaggEffBgFree");
	printf("%x\n", h1);


	TFile *f2 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1502.root");
	gDirectory->cd("Output");
	gDirectory->cd("TaggEff");
	TH1D *h2 = (TH1D*)gROOT->FindObject("TaggEffBgFree");
	printf("%x\n", h2);

	TH1D *hDiff = (TH1D*)h1->Clone("Diff");
	hDiff->Divide(h2);

	gStyle->SetPalette(1);
	hDiff->SetAxisRange(0.6,1.4,"Y");
	hDiff->SetAxisRange(0,271,"X");

	hDiff->Draw();

}