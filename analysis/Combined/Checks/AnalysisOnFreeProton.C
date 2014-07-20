//
// Diese Datei erzeugt aus 
// Datei: /datapool/2011_02/WithPIDSvenSelTFPrime/00Proton.root
// diese Datei wurde mittels  hadd -f 00Proton.root outputF/*.root outputG/*.root outputH/*.root   zuvor erzeugt
// dann die Histos: 
//   - hMesonInvMassVSMissingMass_Prompt_PolP
//   - hMesonInvMassVSMissingMass_Prompt_PolM
// Histo vom normalen Analyser


void AnalysisOnFreeProton() {
	TH2D *hP = (TH2D*)gROOT->FindObject("hMesonInvMassVSMissingMass_Prompt_PolP");
	TH2D *hM = (TH2D*)gROOT->FindObject("hMesonInvMassVSMissingMass_Prompt_PolM");

	char tempStr[100];

	printf("Histo found: %08x \n", hP);
	printf("Histo found: %08x \n", hM);
//	hScaler3->SetAxisRange(0,1420,"X");

	delete gROOT->FindObject("CanvasFreeProton");
	TCanvas *c3 = new TCanvas("CanvasFreeProton");
	
	TH2D *h1 = (TH2D*) hP->Clone("FreeProton");
	h1->Add(hP,hM,-1.0*0.9875,1.*0.96);
	gStyle->SetPalette(1);
	h1->Draw("COLZ");

	delete gROOT->FindObject("CanvasFreeProtonProj");
	TCanvas *c3 = new TCanvas("CanvasFreeProtonProj");
	
//	TH1D *hProj1 = h1->ProjectionY("Proj", 24-5,27+5);
	TH1D *hProj1 = hP->ProjectionY("Proj1", 24-5,27+5);
	hProj1->Scale(0.9875);
	TH1D *hProj2 = hM->ProjectionY("Proj2", 24-5,27+5);
	hProj2->Scale(0.96);
	hProj2->Draw();
	hProj1->SetLineColor(kRed);
	hProj1->Draw("SAME");
	
/*	sprintf(tempStr, "Projection_1D_%d", k);
	delete gROOT->FindObject(tempStr);
	TH1D *hTime1D = hScaler3->ProjectionY(tempStr,1+k,2+1+k);
//		for (int k=0; k<1422;k++) hTime1D->SetBinError(k+1, 1E-10);
	hTime1D->SetLineColor(i);
	hTime1D->SetMarkerColor(i);

	hTime1D->Draw();
	hTime1D->SetAxisRange(0,2800,"Y");
*/
}
