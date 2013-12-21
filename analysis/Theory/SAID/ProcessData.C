#include <TMath.h>

const int NTaggerEnergies = 352;
double TaggerEnergies[NTaggerEnergies];
const double MAMIBeamEnergy = 450;

const Int_t NumberOfThetaBins = 181;
Double_t yT[NumberOfThetaBins];
Double_t yDSG[NumberOfThetaBins];
Double_t yF[NumberOfThetaBins];

// ****************************************************************************************

int ReadTaggerConfigurationFile() {
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t Buffer2[1024];

    float CentralEnergyMEV = -1;

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, "Tagger_Ladder.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("Konnte die Datei %s nicht zum Lesen oeffnen!\n", TempStr);
        exit(1);
    } else {
        printf("INFO: Tagger file: %s\n", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        if (sscanf(Line, "%f", &CentralEnergyMEV) == 1) {
		TaggerEnergies[AktElementNr] = CentralEnergyMEV;
		AktElementNr++;
        } else {
            printf("ERROR reading file.");
        }
    }

    if (AktElementNr != NTaggerEnergies) {
        printf("ERROR: Tagger: Read only %d elements.\n", AktElementNr);
        exit(1);
    } else {
        printf("INFO: Tagger: Read %i elements.\n", AktElementNr);
    }

    int status=fclose(readFile);
    if(status!=0) {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schlieﬂen!\n");
        exit(1);
    }

    return 0;
}


// ****************************************************************************************

void DownloadData() {
	const Int_t NumberOfThetaBins = 181;
	Char_t Name[256];
	Char_t* VarName;
	const Double_t ProtonMass = 938.272;
	Double_t EnergyW;

	
//	for (Int_t k=0; k<NTaggerEnergies; k++) { //complete
//	for (Int_t k=0; k<190; k++) { //1st step
	for (Int_t k=190; k<NTaggerEnergies; k++) { //2nd step
		double PhotonEnergy = MAMIBeamEnergy - TaggerEnergies[k];
		EnergyW = TMath::Sqrt(2*ProtonMass*PhotonEnergy+ProtonMass**2);

		if (EnergyW > 1073.2) {
			printf("\nProcess Bin %d for Gamma Energy %f.\n",k,PhotonEnergy);
		
			sprintf(Name, ".! ./GetDataScript2.sh %f %d", EnergyW, k);
			VarName = new Char_t[strlen(Name)+1];
			strcpy(VarName,Name);
			printf("%s\n", VarName);
			gROOT->ProcessLine(VarName);
		} else {
			//printf("Photonenergy %f below production threshold.\n",PhotonEnergy);
		}
	}
}

// ****************************************************************************************



void GenerateRootFile() {
	Char_t Name[256];
	Char_t* VarName;
	

	TFile *MyRootTreeFile = new TFile("Results.root","RECREATE");
	TTree* MyMAIDTree = new TTree("MyMAIDTree","Event Tree");
	Double_t MyTreeF[NumberOfThetaBins];
	Double_t MyTreeDSG[NumberOfThetaBins];
	Double_t MyTreeT[NumberOfThetaBins];
	Double_t MyTreeGammaLabE;
	Double_t MyTreeW;
	Int_t MyTreeTaggCh;
	Int_t NumberOfEvents = 0;
	const Double_t ProtonMass = 938.272;

	MyMAIDTree->Branch("MyTreeGammaLabE",&MyTreeGammaLabE,"MyTreeGammaLabE/D");
	MyMAIDTree->Branch("MyTreeW",&MyTreeW,"MyTreeW/D");
	MyMAIDTree->Branch("MyTreeTaggCh",&MyTreeTaggCh,"MyTreeTaggCh/I");

       sprintf(Name, "MyTreeT[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeT",MyTreeT,VarName);

       sprintf(Name, "MyTreeDSG[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeDSG",MyTreeDSG,VarName);

       sprintf(Name, "MyTreeF[%i]/D", NumberOfThetaBins);
       VarName = new Char_t[strlen(Name)+1];
       strcpy(VarName,Name);
	MyMAIDTree->Branch("MyTreeF",MyTreeF,VarName);


	for (Int_t k=0;k<352;k++) {
		printf("\nProcess for Tagger Ch %f.\n",k);

		Double_t EnergyW;
		double PhotonEnergy = MAMIBeamEnergy - TaggerEnergies[k];
		EnergyW = TMath::Sqrt(2*ProtonMass*PhotonEnergy+ProtonMass**2);

		if (EnergyW < 1073.2) {
			printf("Energy too low. This is the forseen end.\n");
			break;
		}

		MyTreeGammaLabE = PhotonEnergy;
		MyTreeW = EnergyW;
		MyTreeTaggCh = k;

		FILE *readFile = NULL;

		sprintf(Name, "MAIDData/MAIDValues_ch%d.dat", k);
		VarName = new Char_t[strlen(Name)+1];
		strcpy(VarName,Name);
		
		readFile=fopen(VarName, "r");// text Datei zum Schreiben "r" aufmachen.
		if(readFile == NULL) {
			printf("Konnte die Datei %s nicht zum Lesen oeffnen!\n", VarName);
			exit(1);
		} else {
			printf("INFO: Tagger file: %s\n", VarName);
		}

		int NElements = 0;
		int AktElementNr = 0;
		float TempValues[5];
		Char_t Line[1024];
		while (!feof(readFile)) {
			if (!fgets(Line, sizeof(Line), readFile)) break;
			if (sscanf(Line, "%f %f %f %f %f %f %f %f %f %f", &TempValues[0], &TempValues[1], &TempValues[2], &TempValues[3], &TempValues[4]) == 5) {
				//TaggerEnergies[AktElementNr] = CentralEnergyMEV;
				AktElementNr = TMath::Nint(TempValues[0]);
				if (AktElementNr > NumberOfThetaBins) {
					printf("Number of Theta bins in file to big.\n");
					exit(1);
				} else {
					MyTreeDSG[AktElementNr] = 0;
					if (NElements > 180) {
						MyTreeT[AktElementNr] = TempValues[1];
						///printf("T\tAngle: %d\tValue:%f\n",TMath::Nint(TempValues[0]), TempValues[1]);
					} else {
						MyTreeF[AktElementNr] = TempValues[1];
						///printf("F\tAngle: %d\tValue:%f\n",TMath::Nint(TempValues[0]), TempValues[1]);
					}
					NElements++;
				}
			}
		}
		if (NElements != (NumberOfThetaBins*2)) {
			printf("Number of Theta bins (%d) not correct in file.\n", NElements);
			exit(1);
		}
		MyMAIDTree->Fill();


		int status=fclose(readFile);
		if(status!=0) {
			fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schlieﬂen!\n");
			exit(1);
		}

	}

	printf("Write root file. ");
	MyRootTreeFile->Write();
	MyRootTreeFile->Close();
	printf("Done.\n");

	exit(0);
}



//*********************************************************************************


int ProcessData() {
	ReadTaggerConfigurationFile();
//	DownloadData();
	GenerateRootFile();

	exit(1);
}