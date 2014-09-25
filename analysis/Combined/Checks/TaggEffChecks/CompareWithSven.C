//
// Vergleiche eine TaggEff Messung von 
// Peter mit Sven
//

{
	TFile *f1 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1487.root");
//	TFile *f1 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1502.root");
//	TFile *f1 = new TFile("/datapool/2010_09/taggeff/output_WithPID/results/TaggEff_M_1507.root");
	gDirectory->cd("Output");
	gDirectory->cd("TaggEff");
	TH1D *h1 = (TH1D*)gROOT->FindObject("TaggEffBgFreeC");
	printf("%x\n", h1);


	TH1D *h2 = (TH1D*)h1->Clone("h2");
	h2->Reset();

	FILE *readFile = NULL;
	Char_t Line[1024];
    
	readFile=fopen("/datapool/home/schumans/TaggEff/2010_09/on/TaggEff_26731_hist.root.txt", "r");// text Datei zum Schreiben "r" aufmachen.
//	readFile=fopen("/datapool/home/schumans/TaggEff/2010_09/on/TaggEff_26948_hist.root.txt", "r");// text Datei zum Schreiben "r" aufmachen.
//	readFile=fopen("/datapool/home/schumans/TaggEff/2010_09/on/TaggEff_27041_hist.root.txt", "r");// text Datei zum Schreiben "r" aufmachen.
	if (readFile == NULL) { printf("Could not open analysis config file!\n"); }

	int AktElementNr = 0;
	while (!feof(readFile)) {
		AktElementNr++;
		if (!fgets(Line, sizeof(Line), readFile)) break;

		int Kanal;
		float Werte[6];

		if (sscanf(Line, "%d %f %f %f %f %f %f", &Kanal, &Werte[0], &Werte[1], &Werte[2], &Werte[3], &Werte[4], &Werte[5] ) == 7) {
//			printf("%d %f %f\n",Kanal, Werte[4], Werte[5]);
			h2->SetBinContent(Kanal+1, Werte[4]);
			h2->SetBinError(Kanal+1, Werte[5]);
		}
	}
	fclose(readFile);


	TH1D *hDiff = (TH1D*)h1->Clone("Diff");
	hDiff->Divide(h2);

	gStyle->SetPalette(1);
	hDiff->SetAxisRange(0.6,1.8,"Y");
	hDiff->SetAxisRange(0,271,"X");

	h1->SetAxisRange(0.06,0.14,"Y");
	h1->SetAxisRange(0,271,"X");

	h2->SetAxisRange(0.06,0.14,"Y");
	h2->SetAxisRange(0,271,"X");

	TCanvas *c1 = new TCanvas();
/*	c1->Divide(3,1);
	c1->cd(1); h1->Draw();
	c1->cd(2); h2->Draw();
	c1->cd(3); 
*/	hDiff->Draw();

}