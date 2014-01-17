#include <stdio.h>

const Int_t NumberOfThetaBins = 181;

const char Str_File[] = "output/F.txt";


//**************************************************************************

void BatchTextOutTaggCh(TH2D *h2, Int_t ATaggCh, Int_t ANBinsToCombine){
	char tempStr[1000], tempStr2[1000]; //Für Strings zusammenklopfen

	TH1D *h1P;
	sprintf(tempStr,"h1forTaggCh%i", ATaggCh);
	printf("%d, ", ATaggCh);
	h1P = h2->ProjectionY(tempStr,1+ATaggCh,1+ATaggCh+ANBinsToCombine-1);

	h1P->Scale(1./(1.*ANBinsToCombine));

	FILE *f1;
	sprintf(tempStr, Str_File, ATaggCh);
	f1 = fopen (tempStr,"a");
	if (f1 == NULL) printf("ERROR: Ausgabedatei konnte nicht angelegt werden.\n");
	
	for (int i=1; i<=18; i++) {
		fprintf(f1, "%d\t%f\t%f\t%f\t%f\n", ATaggCh, (i-1)*10+5, 5, h1P->GetBinContent(i), h1P->GetBinError(i));
	}
	fprintf(f1, "\n\n");
	fclose(f1);
}

void BatchTextOutAll(Int_t ANBinsToCombine){
	char tempStr[1000];
	FILE *f1;
	sprintf(tempStr, Str_File);
	f1 = fopen (tempStr,"w");
	if (f1 == NULL) printf("ERROR: Ausgabedatei konnte nicht angelegt werden.\n");

	fprintf(f1, "TaggerCh\tTheta\tDTheta\tAsym\tDAsym\n\n");
	fclose(f1);


	TFile *MyFile = new TFile("../step1/output/results.root");
	TH2D *h2 = (TH2D*)gROOT->FindObject("ButDivHDivPolsFlux");
	if (!h2) {
		printf("TH2D ButDivHDivPolsFlux not found.\n");
		exit(1);
	} else {
		printf("TH2D ButDivHDivPolsFlux found at 0x%8x.\n",h2);
	}


	printf("Generate Output for each TaggCh: \n");
	for (int i=0; i<=256; i++) {
		BatchTextOutTaggCh(h2, i, ANBinsToCombine);
	}
	printf("\n");
}

void WriteTextFile() {
	BatchTextOutAll(1);
}