#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <cmath>

#include <TRint.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TFile.h>
#include <TMath.h>
#include <TF1.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>

#include <Worker.h>

Char_t AbsolutePathToData[] = "/datapool/home/otte/NewAnalysis/CompareResults/input";

typedef struct TValue {
    double Theta;
    double ThetaError;
    double ValueF;
    double ValueFError;
    double ValueT;
    double ValueTError;
    double ValueDSG;
    double ValueDSGError;
} TValue;

typedef struct TDataSetPoint {
	double ThetaBinWidth;
	std::vector<TValue> Values;
} TDataSetPoint;

enum Data_DataSets {EMAID, ESven, EPeter, EPauline, EMAIDDMT, EPeterPrime, NumberOfDataSets};
const int Data_Colors[NumberOfDataSets] = {4, 1, 2, 3, 5, 6}; //The datasets from above do get the following colors: black, red, green, blue,
const char Data_Names[NumberOfDataSets][20] = {"MAID", "Sven", "Peter", "Pauline", "MAIDDMT", "PeterPrime"};
const double Data_ThetaStartAngle[NumberOfDataSets] = {0, 5, 5, 20, 0, 5};
const double Data_ThetaBinWidth[NumberOfDataSets] = {1, 10, 10, 15, 1, 10};
int Data_NumberOfSkippedChannelsDuringFitting[NumberOfDataSets];

const int QualityOfTaggCh[272] = {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1,
                                  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
                                  1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
                                  1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                  0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
                                  1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0,
                                  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1};

typedef struct TDataAll {
    int TaggCh;
    double TaggerChE; //Energy of this TaggerCh
    std::vector<TDataSetPoint> DataSets;
} TDataAll;

std::vector<TDataAll> DataAll;
const Int_t NumberOfThetaBins = 181;
const Int_t NumberOfMaxTaggerCh = 257;
const Int_t NumberOfLegendreParameters = 4;
std::vector<TH2D*> FitResults(NumberOfLegendreParameters+1);


#include "FitFunctions.cc"


int DoNotDrawMAID = 0;
int DoNotDrawSven = 0;
int DoNotDrawPeter = 0;
int DoNotDrawPeterPrime = 0;
int DoNotDrawPauline = 0;
int DoNotDrawMAIDDMT = 0;
int fMultiplyDSG = -1;
int DrawThresholdsRegion = 0;
int DoBatchMode = 0;
int DoCompareToModel = -1;
int CheckDataNr = EPeter; //Which Dataset should be checked


int ReadDataFilePauline(int fType, int fTaggCh) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    if (fType == 0) {
        sprintf(TempStr, "%s/Pauline/F/W_F_%i_2sig.dat", AbsolutePathToData, fTaggCh);
    } else {
        sprintf(TempStr, "%s/Pauline/T/W_T_%i_2sig.dat", AbsolutePathToData, fTaggCh);
    }
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file.\n");
        exit(1);
    }

    float Value0, Value1;
    int Counter = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Value0 = 0;
        Value1 = 0;

        if (sscanf(Line, "%f %f ", &Value0, &Value1 ) == 2) {
            DataAll.at(fTaggCh).DataSets.at(EPauline).ThetaBinWidth = 15;
            if (fType == 0) {
                DataAll.at(fTaggCh).DataSets.at(EPauline).Values.at(Counter).ValueF = Value0;
                DataAll.at(fTaggCh).DataSets.at(EPauline).Values.at(Counter).ValueFError = Value1;
            } else {
                DataAll.at(fTaggCh).DataSets.at(EPauline).Values.at(Counter).ValueT = Value0;
                DataAll.at(fTaggCh).DataSets.at(EPauline).Values.at(Counter).ValueTError = Value1;
            }
            Counter++;
        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int ReadDataFileMAID(int fTaggCh) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    sprintf(TempStr, "%s/MAIDData/MAIDValues_ch%i.dat", AbsolutePathToData, fTaggCh);
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file: %s\n",TempStr);
        exit(1);
    }

    int Counter = 0;
    for (int i=0; i<26; i++) { //Skip the header
        if (!fgets(Line, sizeof(Line), readFile)) break;
        //printf("Skip: %s\n", Line);
    }

    float TempValue[10];
    for (int i=0; i<NumberOfThetaBins; i++) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        if (sscanf(Line, "%f %f %f %f %f %f %f %f %f %f ", &TempValue[0], &TempValue[1], &TempValue[2], &TempValue[3], &TempValue[4], &TempValue[5], &TempValue[6], &TempValue[7], &TempValue[8], &TempValue[9] ) == 10) {
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).Theta = TempValue[0];
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueDSG = TempValue[2];
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueDSGError = TempValue[2]*0.00001+0.0000001; //The small errors are for root to draw the points. If value and error =0, root will not plot.
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueF = TempValue[7];
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueFError = TempValue[7]*0.01+0.001;
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueT = TempValue[3];
            DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at(Counter).ValueTError = TempValue[3]*0.01+0.001;
            //printf("%f %f %f %f %f %f %f %f %f %f \n", TempValue[0], TempValue[1], TempValue[2], TempValue[3], TempValue[4], TempValue[5], TempValue[6], TempValue[7], TempValue[8], TempValue[9] );
            Counter++;
        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int ReadDataFileMAIDDMT(int fTaggCh) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    sprintf(TempStr, "%s/MAIDDMTData/MAIDValues_ch%i.dat", AbsolutePathToData, fTaggCh);
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file: %s\n",TempStr);
        exit(1);
    }

    int Counter = 0;
    for (int i=0; i<51; i++) { //Skip the header
        if (!fgets(Line, sizeof(Line), readFile)) break;
        //printf("Skip: %s\n", Line);
    }

    float TempValue[10];
    for (int i=0; i<NumberOfThetaBins+58; i++) { //Die 58 da, zwischen den Datenblöcken in der Datei Müll drinne steht.
        if (!fgets(Line, sizeof(Line), readFile)) break;
        if (sscanf(Line, "%f %f %f %f %f %f %f %f %f %f ", &TempValue[0], &TempValue[1], &TempValue[2], &TempValue[3], &TempValue[4], &TempValue[5], &TempValue[6], &TempValue[7], &TempValue[8], &TempValue[9] ) == 10) {
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).Theta = TempValue[0];
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueDSG = TempValue[2];
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueDSGError = TempValue[2]*0.00001+0.0000001; //The small errors are for root to draw the points. If value and error =0, root will not plot.
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueF = TempValue[7];
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueFError = TempValue[7]*0.01+0.001;
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueT = TempValue[3];
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(Counter).ValueTError = TempValue[3]*0.01+0.001;
            //printf("%f %f %f %f %f %f %f %f %f %f \n", TempValue[0], TempValue[1], TempValue[2], TempValue[3], TempValue[4], TempValue[5], TempValue[6], TempValue[7], TempValue[8], TempValue[9] );
            Counter++;
        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int ReadDataFileSven(int fType) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    if (fType == 0) {
        sprintf(TempStr, "%s/Sven/sigmaF_10deg.txt", AbsolutePathToData);
    } else {
        sprintf(TempStr, "%s/Sven/sigmaT_10deg.txt", AbsolutePathToData);
    }
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file: %s\n", TempStr);
        exit(1);
    }

    int Counter = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;

        int TaggCh;
        if (sscanf(Line, "Channel %d: ", &TaggCh) == 1) {
            //printf("fTaggCh: %d\n", TaggCh);
            Counter = 0;

            for (int i=0;i<2;i++) {
                if (!fgets(Line, sizeof(Line), readFile)) break;
            }
            for (int i=0;i<18;i++) {
                if (!fgets(Line, sizeof(Line), readFile)) break;

                float Value0, Value1, Value2, Value3;
                if (sscanf(Line, "%f %f %f %f ", &Value0, &Value1, &Value2, &Value3) == 4) {
                    //printf("%d: %f %f \n", TaggCh, Value2, Value3);
                    Value2 /= DataAll.at(TaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).Theta) ).ValueDSG;
                    Value3 /= DataAll.at(TaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).Theta) ).ValueDSG;

                    if (fType == 0) {
                        DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).ValueF = Value2;
                        DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).ValueFError = Value3;
                    } else {
                        DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).ValueT = Value2;
                        DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).ValueTError = Value3;
                    }
                    Counter++;
                }
            }

        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int ReadDataFilePeter(int fType) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    if (fType == 0) {
        sprintf(TempStr, "%s/Peter/F.txt", AbsolutePathToData);
    } else {
        sprintf(TempStr, "%s/Peter/T.txt", AbsolutePathToData);
    }
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file: %s\n", TempStr);
        exit(1);
    }

    int Counter = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;

        int TaggCh;
        float Value0, Value1, Value2, Value3;

        if (sscanf(Line, "%d %f %f %f %f ", &TaggCh, &Value0, &Value1, &Value2, &Value3) == 5) {
            Counter = (int)((Value0-5.)/10.);
            //printf("%d: bin: %d \tAsym: %f %f \n", TaggCh, Counter, Value2, Value3);
            if ( (fabs(Value2) > 10) || (fabs(Value3) > 20) ) {/*printf("Drop: %d: bin: %d \tAsym: %f %f \n", TaggCh, Counter, Value2, Value3); */continue; }
            if (fType == 0) {
                DataAll.at(TaggCh).DataSets.at(EPeter).Values.at(Counter).ValueF = Value2;
                DataAll.at(TaggCh).DataSets.at(EPeter).Values.at(Counter).ValueFError = Value3;
            } else {
                DataAll.at(TaggCh).DataSets.at(EPeter).Values.at(Counter).ValueT = Value2;
                DataAll.at(TaggCh).DataSets.at(EPeter).Values.at(Counter).ValueTError = Value3;
            }
        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int ReadDataFilePeterPrime(int fType) {
    FILE *readFile = NULL;
    Char_t Line[1024];
    Char_t TempStr[1024];

    if (fType == 0) {
        sprintf(TempStr, "%s/PeterPrime/FPrime.txt", AbsolutePathToData);
    } else {
        sprintf(TempStr, "%s/PeterPrime/TPrime.txt", AbsolutePathToData);
    }
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL) {
        printf("ERROR: Could not open file: %s\n", TempStr);
        exit(1);
    }

    int Counter = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;

        int TaggCh;
        float Value0, Value1, Value2, Value3;

        if (sscanf(Line, "%d %f %f %f %f ", &TaggCh, &Value0, &Value1, &Value2, &Value3) == 5) {
            Counter = (int)((Value0-5.)/10.);
            //printf("%d: bin: %d \tAsym: %f %f \n", TaggCh, Counter, Value2, Value3);
            if (fType == 0) {
                DataAll.at(TaggCh).DataSets.at(EPeterPrime).Values.at(Counter).ValueF = Value2;
                DataAll.at(TaggCh).DataSets.at(EPeterPrime).Values.at(Counter).ValueFError = Value3;
            } else {
                DataAll.at(TaggCh).DataSets.at(EPeterPrime).Values.at(Counter).ValueT = Value2;
                DataAll.at(TaggCh).DataSets.at(EPeterPrime).Values.at(Counter).ValueTError = Value3;
            }
        }
    }

    int status=fclose(readFile);
    if(status!=0) {
        printf("ERROR: Could not close file!\n");
        exit(1);
    }

    return 0;
}

int CreateVector(int fTaggCh) {
    TDataAll TempDataAll;
    TempDataAll.TaggCh = fTaggCh;

    double ActTheta = 0;
    double ThetaBinWidth = 0;

    for (int i=0; i<NumberOfDataSets; i++ ) {
        TDataSetPoint TempDataSetPoint;

        ActTheta = Data_ThetaStartAngle[i];
        ThetaBinWidth = Data_ThetaBinWidth[i];
        while (ActTheta <= 180) {
            TValue TempTValue;
            TempTValue.Theta = ActTheta;
            TempTValue.ThetaError = ThetaBinWidth/sqrt(12);
            TempTValue.ValueDSG = 0;
            TempTValue.ValueDSGError = 0;
            TempTValue.ValueF = 0;
            TempTValue.ValueFError = 0;
            TempTValue.ValueT = 0;
            TempTValue.ValueTError = 0;
            TempDataSetPoint.Values.push_back(TempTValue);
            TempDataSetPoint.ThetaBinWidth = ThetaBinWidth;

            ActTheta+=ThetaBinWidth;
        }

        TempDataAll.DataSets.push_back(TempDataSetPoint);
    }

    DataAll.push_back(TempDataAll);
}

int ReadDataMAID() {
    for (int i=0;i<257; i++) {
        ReadDataFileMAID(i); //T and F at the same time
    }
}

int ReadDataMAIDDMT() {
    for (int i=0;i<257; i++) {
        ReadDataFileMAIDDMT(i); //T and F at the same time
    }
}

int ReadDataPauline() {
    for (int i=0;i<212; i++) { //Pauline went down to Tagger Channel 211 (including)
        ReadDataFilePauline(0, i); //F
        ReadDataFilePauline(1, i); //T
    }
}

int ReadDataSven() {
    ReadDataFileSven(0); //F
    ReadDataFileSven(1); //T
}

int ReadDataPeter() {
    ReadDataFilePeter(0); //F
    ReadDataFilePeter(1); //T
}

int ReadDataPeterPrime() {
    ReadDataFilePeterPrime(0); //FPrime
    ReadDataFilePeterPrime(1); //TPrime
}

int ReadTaggerCalibration() {
    Double_t MyTreeGammaLabE;

    TFile *MyRootTreeFile2;
    MyRootTreeFile2 = new TFile("/datapool/home/otte/NewAnalysis/analysis/Theory/MAID/Results.root");
    TTree *mt;
    mt = (TTree*)gROOT->FindObject("MyMAIDTree");
    mt->SetBranchAddress("MyTreeGammaLabE",&MyTreeGammaLabE);
    for (int i = 0;i<=256; i++) {
        mt->GetEntry(i);
        DataAll.at(i).TaggerChE = MyTreeGammaLabE;
        //printf("MyTreeGammaLabE %d: %f\n",i,MyTreeGammaLabE);
    }
    MyRootTreeFile2->Close();
}

int PrintDataInformation(int fTaggCh) {
    //Theory
/*    printf("Theory\n");
    for (int k=0;k<DataAll.at(fTaggCh).NTheoryValue;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).TheoryValues.at(k).Theta);
    }
    printf("\n DSG:");
    for (int k=0;k<DataAll.at(fTaggCh).NTheoryValue;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).TheoryValues.at(k).ValueDSG);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NTheoryValue;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).TheoryValues.at(k).ValueF);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NTheoryValue;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).TheoryValues.at(k).ValueT);
    }
    printf("\n");

    //Pauline
    printf("Pauline\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).Theta);
    }
    printf("\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).ThetaError);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).ValueF);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).ValueFError);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).ValueT);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPauline;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPauline.at(k).ValueTError);
    }
    printf("\n");

    //Sven
    printf("Sven\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).Theta);
    }
    printf("\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).ThetaError);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).ValueF);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).ValueFError);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).ValueT);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesSven;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesSven.at(k).ValueTError);
    }
    printf("\n");

    //Peter
    printf("Peter\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).Theta);
    }
    printf("\n");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).ThetaError);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).ValueF);
    }
    printf("\n F:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).ValueFError);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).ValueT);
    }
    printf("\n T:");
    for (int k=0;k<DataAll.at(fTaggCh).NValuesPeter;k++) {
        printf("%6.2f ", DataAll.at(fTaggCh).ValuesPeter.at(k).ValueTError);
    }
    printf("\n");*/
    printf("\n");
}

//*************************************************************************************

int FillHistogram(int fDataSetNumber, int fAsymToAnalyse, int fTaggCh, int fMultiplyDSG, TH1D* MyHist) {
    for(Int_t t=0; t < DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.size(); t++) {
        if (fAsymToAnalyse) { //F
            //PlotAsymmetry * DSG
            if (fMultiplyDSG) {
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueF *
                                           DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueFError *
                                         DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
            } else {
                //Plot Asymmetry
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueF );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueFError );
            }
        } else { //T
            //PlotAsymmetry * DSG
            if (fMultiplyDSG) {
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueT *
                                           DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueTError *
                                         DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
            } else {
                //Plot Asymmetry
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueT );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueTError );
            }
        }
    }
    MyHist->SetLineColor(Data_Colors[fDataSetNumber]);
    MyHist->SetMarkerColor(Data_Colors[fDataSetNumber]);
    MyHist->SetMarkerStyle(20);
    MyHist->SetMarkerSize(0.8);
}

int ProcessTaggCh(int fTaggCh, int fDrawGraphs, int fAsymToAnalyse, int fMultiplyDSG) {
    if (fDrawGraphs) TCanvas* Canvas = new TCanvas("Draw");

    gROOT->SetStyle("Plain");
    gStyle->SetOptFit();

    TH1D *MyHist;

    for (int i=0; i<NumberOfDataSets; i++) {
        delete gROOT->FindObject(Data_Names[i]);
        MyHist = new TH1D(Data_Names[i], Data_Names[i], DataAll.at(fTaggCh).DataSets.at(i).Values.size(),
                           DataAll.at(fTaggCh).DataSets.at(i).Values.at(0).Theta - DataAll.at(fTaggCh).DataSets.at(i).ThetaBinWidth/2,
                           DataAll.at(fTaggCh).DataSets.at(i).Values.at(0).Theta - DataAll.at(fTaggCh).DataSets.at(i).ThetaBinWidth/2 +
                           DataAll.at(fTaggCh).DataSets.at(i).ThetaBinWidth * DataAll.at(fTaggCh).DataSets.at(i).Values.size() );

        FillHistogram(i, fAsymToAnalyse, fTaggCh, fMultiplyDSG, MyHist);
        Data_NumberOfSkippedChannelsDuringFitting[i] = 0;
    }


    //********************************************************************************

    ActualTaggCh = fTaggCh;
    char tempStr[1000];
    Int_t MyFitStatus = 0;
    char StrFitParameters[10];

    for (int m=0; m<NumberOfDataSets; m++) {
        if (fDrawGraphs) {
            strcpy(StrFitParameters, "RqU+"); //0 = do not draw, q=quiet, R = respect range, f = special min finder,
                                        // U = adds results to box, + = add multiple fits to the same histo
        } else {
            strcpy(StrFitParameters, "0RqU+");
        }
        if ( (m==EMAID) || (m==EMAIDDMT) ) strcat(StrFitParameters, "W");

        MyHist = (TH1D*)gROOT->FindObject(Data_Names[m]);
        if (MyHist->Integral() != 0) {
            sprintf(tempStr,"Fit_%s_Ch%i",Data_Names[m],fTaggCh);
            delete gROOT->FindObject(tempStr);
            TF1 *PrevFitTMP = new TF1(tempStr,FitFunction,0+1,180-1,NumberOfLegendreParameters); //From ,to, NumberOfParameters
            PrevFitTMP->SetLineColor(Data_Colors[m]);
            PrevFitTMP->SetLineWidth(2);
            PrevFitTMP->SetParameter(0, 1);
            PrevFitTMP->SetParameter(1, 0);
            PrevFitTMP->SetParameter(2, 0);
            PrevFitTMP->SetParameter(3, 0);
            //PrevFitTMP->FixParameter(3, 0);
            PrevFitTMP->SetParNames("StrengthSin","StrengthCos", "StrengthCos2", "StrengthCos3");
            TFitResultPtr MyFitResult = MyHist->Fit(tempStr, StrFitParameters);

            MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data,
                                 //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
            if (MyFitStatus == 0) {
                for (int i=0;i<NumberOfLegendreParameters;i++){
                    //zuvor: fTaggCh+1
                    //int DarstellPos = FitResults.at(i)->GetNbinsX()-fTaggCh;
                    int DarstellPos = fTaggCh+1;
                    FitResults.at(i)->SetBinContent(DarstellPos, m+1, PrevFitTMP->GetParameter(i));
                    if ( (m != EMAID) && (m!=EMAIDDMT) ) { //No Errors for Simulated Data
                        FitResults.at(i)->SetBinError(DarstellPos, m+1, PrevFitTMP->GetParError(i));
                    } else {
                        FitResults.at(i)->SetBinError(DarstellPos, m+1, 0);
                    }
                }

                if ( (PrevFitTMP->GetChisquare() > 0.001) && (m != EMAID) && (m != EMAIDDMT) )
                    FitResults.at(NumberOfLegendreParameters)->SetBinContent(fTaggCh+1, m+1, PrevFitTMP->GetChisquare()/PrevFitTMP->GetNDF()); //FitResults.at(NumberOfLegendreParameters)->GetNbinsX()-fTaggCh
                //int DarstellPos2 = FitResults.at(4)->GetNbinsX()-fTaggCh;
                int DarstellPos2 = fTaggCh+1;
                FitResults.at(NumberOfLegendreParameters)->SetBinError(DarstellPos2, m+1, 0);
            } else {
                printf("|%d:%d:%d|",fTaggCh,m,MyFitStatus);
                fflush(stdout);
            }
        } else {
            printf("-%d-",m);
            fflush(stdout);
            Data_NumberOfSkippedChannelsDuringFitting[m]++;
        }
    }

    //*****************************************************************

    if (fDrawGraphs) {
        int NAlreadyDrawn = 0;
        for (int i=0; i<NumberOfDataSets; i++) {
            if (NAlreadyDrawn) {
                ((TH1D*)(gROOT->FindObject(Data_Names[i])))->Draw("same");
            } else {
                ((TH1D*)(gROOT->FindObject(Data_Names[i])))->Draw();
            }
            NAlreadyDrawn++;
        }
    }

}

int PlotDataAtFixedAngle(int fSelectedAsym) {
    TCanvas *c1 = new TCanvas("CanvasFixedAngle");
    char tempStr[1000];

    int NDrawnHisto = 0;
//    const int Data_Bins[NumberOfDataSets] = {45, 4, 4, 2, 45}; //45 Grad
//    const int Data_Bins[NumberOfDataSets] = {95, 9, 9, 5, 95}; //95 Grad
    const int Data_Bins[NumberOfDataSets] = {135, 13, 13, 8, 135}; //135 Grad

    //Arrays for ErrorGraph
    double DataPeterX[NumberOfMaxTaggerCh];
    double DataPeterY[NumberOfMaxTaggerCh];
    double DataPeterXE[NumberOfMaxTaggerCh];
    double DataPeterYE[NumberOfMaxTaggerCh];
    // create the multigraph
    TMultiGraph *mg = new TMultiGraph();
    mg->SetTitle("Asymmetrie");


    TH1D *hFixedAngle[NumberOfDataSets];
    //for (int m=0; m<1; m++) {
    for (int m=0; m<NumberOfDataSets; m++) {
        if ((m==EMAID) && (DoNotDrawMAID)) continue;
        if ((m==ESven) && (DoNotDrawSven)) continue;
        if ((m==EPeter) && (DoNotDrawPeter)) continue;
        if ((m==EPauline) && (DoNotDrawPauline)) continue;
        if ((m==EMAIDDMT) && (DoNotDrawMAIDDMT)) continue;

        sprintf(tempStr, "FixedAngle_%s", Data_Names[m]);
        hFixedAngle[m] = new TH1D(tempStr, tempStr, NumberOfMaxTaggerCh, 0, NumberOfMaxTaggerCh );
        hFixedAngle[m]->SetLineColor(Data_Colors[m]);
        hFixedAngle[m]->SetAxisRange(-2,2,"Y");

        printf("Plot %s for theta angle %f.\n", Data_Names[m], DataAll.at(0).DataSets.at(m).Values.at(Data_Bins[m]).Theta );
        for (int fTaggCh=0; fTaggCh<NumberOfMaxTaggerCh; fTaggCh++) {
            if (fSelectedAsym==1) {
                hFixedAngle[m]->SetBinContent(fTaggCh+1, DataAll.at(fTaggCh).DataSets.at(m).Values.at(Data_Bins[m]).ValueF);
                hFixedAngle[m]->SetBinError(fTaggCh+1, DataAll.at(fTaggCh).DataSets.at(m).Values.at(Data_Bins[m]).ValueFError);
            } else {
                hFixedAngle[m]->SetBinContent(fTaggCh+1, DataAll.at(fTaggCh).DataSets.at(m).Values.at(Data_Bins[m]).ValueT);
                hFixedAngle[m]->SetBinError(fTaggCh+1, DataAll.at(fTaggCh).DataSets.at(m).Values.at(Data_Bins[m]).ValueTError);
            }
        }

        if (NDrawnHisto) {
            hFixedAngle[m]->Draw("same");
        } else {
            hFixedAngle[m]->Draw("");
        }
        NDrawnHisto++;
    }

    //******************** TErrorGraph *******************

    TCanvas *c2 = new TCanvas("c2","Results at fixed angle");
    c2->SetGrid();
    NDrawnHisto=0;

    for (int m=0; m<NumberOfDataSets; m++) {
        int AnzDatenPunkte = 0;

        if ((m==EMAID) && (DoNotDrawMAID)) continue;
        if ((m==ESven) && (DoNotDrawSven)) continue;
        if ((m==EPeter) && (DoNotDrawPeter)) continue;
        if ((m==EPauline) && (DoNotDrawPauline)) continue;
        if ((m==EMAIDDMT) && (DoNotDrawMAIDDMT)) continue;

        for (int fTaggCh=0; fTaggCh<NumberOfMaxTaggerCh; fTaggCh++) {
            if (hFixedAngle[m]->GetBinError(fTaggCh+1) != 0) {
                DataPeterX[AnzDatenPunkte] = DataAll.at(fTaggCh).TaggerChE;
                DataPeterXE[AnzDatenPunkte] = 1.1/sqrt(12);
                DataPeterY[AnzDatenPunkte] = hFixedAngle[m]->GetBinContent(fTaggCh+1);
                DataPeterYE[AnzDatenPunkte] = hFixedAngle[m]->GetBinError(fTaggCh+1);
                AnzDatenPunkte++;
            }
        }

        char DrawArg[10] = "Z";
        void *ttt;
        // create the TGraphErrors and draw it
        if ( (m==EMAID) || (m==EMAIDDMT)) {
            //TGraph *gr = new TGraph(AnzDatenPunkte,DataPeterX,DataPeterY);
            ttt = new TGraph(AnzDatenPunkte,DataPeterX,DataPeterY);
            strcat(DrawArg, "C");
            ((TGraph*)ttt)->SetLineWidth(2);
        } else {
            //TGraphErrors *gr = new TGraphErrors(AnzDatenPunkte,DataPeterX,DataPeterY,DataPeterXE,DataPeterYE);
            ttt = new TGraphErrors(AnzDatenPunkte,DataPeterX,DataPeterY,DataPeterXE,DataPeterYE);
            strcat(DrawArg, "P");
        }
        ((TGraph*)ttt)->SetTitle(Data_Names[m]);
        ((TGraph*)ttt)->SetMarkerColor(Data_Colors[m]);
        ((TGraph*)ttt)->SetLineColor(Data_Colors[m]);
        ((TGraph*)ttt)->SetMarkerStyle(22); //21 = Quadrat, 22 = Dreieck
        ((TGraph*)ttt)->SetMarkerSize(0.5);

        // put the graphs in the multigraph
        //mg->Add((TGraph*)ttt);

        if (!NDrawnHisto) { strcat(DrawArg, "A"); }

        printf("Zeichne: %s %i\n", Data_Names[m], AnzDatenPunkte);
        ((TGraph*)ttt)->Draw(DrawArg);
        NDrawnHisto++;

    }


    // draw the multigraph
    // "A"	Axis are drawn around the graph
    // "C"	A smooth Curve is drawn
    // "P"	The current marker is plotted at each point
    // "Z"	Do not draw small horizontal and vertical lines the end of the error bars. Without "Z", the default is to draw these.
 //   mg->Draw("APZ");

    c2->Update();

}

int PlotDataAtFixedEnergy(int fTaggCh, int fSelectedAsym) {
    int NDrawnHisto = 0;

    //Arrays for ErrorGraph
    double DataPeterX[181];
    double DataPeterY[181];
    double DataPeterXE[181];
    double DataPeterYE[181];

    //******************** TErrorGraph *******************

    TCanvas *c2 = new TCanvas("c2FixedEnergy","Results at fixed energy");
    c2->SetGrid();
    NDrawnHisto=0;

    for (int m=0; m<NumberOfDataSets; m++) {
        int AnzDatenPunkte = 0;

        if ((m==EMAID) && (DoNotDrawMAID)) continue;
        if ((m==ESven) && (DoNotDrawSven)) continue;
        if ((m==EPeter) && (DoNotDrawPeter)) continue;
        if ((m==EPauline) && (DoNotDrawPauline)) continue;
        if ((m==EMAIDDMT) && (DoNotDrawMAIDDMT)) continue;

//        printf("Plot %s for energy %f.\n", Data_Names[m], DataAll.at(0).DataSets.at(m).Values.at(Data_Bins[m]).Theta );

        for (int ThetaBin=0; ThetaBin<DataAll.at(fTaggCh).DataSets.at(m).Values.size(); ThetaBin++) {
            DataPeterX[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).Values.at(ThetaBin).Theta;
            DataPeterXE[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).ThetaBinWidth/sqrt(12);
            if (fSelectedAsym==1) {
                DataPeterY[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).Values.at(ThetaBin).ValueF;
                DataPeterYE[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).Values.at(ThetaBin).ValueFError;
            } else {
                DataPeterY[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).Values.at(ThetaBin).ValueT;
                DataPeterYE[AnzDatenPunkte] = DataAll.at(fTaggCh).DataSets.at(m).Values.at(ThetaBin).ValueTError;
            }

            if (DataPeterYE[AnzDatenPunkte] != 0) AnzDatenPunkte++;
        }

        if (AnzDatenPunkte) {
            char DrawArg[10] = "Z";
            void *ttt;
            // create the TGraphErrors and draw it
            if ( (m==EMAID) || (m==EMAIDDMT)) {
                //TGraph *gr = new TGraph(AnzDatenPunkte,DataPeterX,DataPeterY);
                ttt = new TGraph(AnzDatenPunkte,DataPeterX,DataPeterY);
                strcat(DrawArg, "C");
                ((TGraph*)ttt)->SetLineWidth(2);
                ((TGraph*)ttt)->GetXaxis()->SetLimits(0,180.);
            } else {
                //TGraphErrors *gr = new TGraphErrors(AnzDatenPunkte,DataPeterX,DataPeterY,DataPeterXE,DataPeterYE);
                ttt = new TGraphErrors(AnzDatenPunkte,DataPeterX,DataPeterY,DataPeterXE,DataPeterYE);
                strcat(DrawArg, "P");
            }
            ((TGraph*)ttt)->SetTitle(Data_Names[m]);
            ((TGraph*)ttt)->SetMarkerColor(Data_Colors[m]);
            ((TGraph*)ttt)->SetLineColor(Data_Colors[m]);
            ((TGraph*)ttt)->SetMarkerStyle(22); //21 = Quadrat, 22 = Dreieck
            ((TGraph*)ttt)->SetMarkerSize(0.5);

            if (!NDrawnHisto) { strcat(DrawArg, "A"); }

            printf("Zeichne: %s %i\n", Data_Names[m], AnzDatenPunkte);
            ((TGraph*)ttt)->Draw(DrawArg);
            NDrawnHisto++;
        } else {
            printf("Datensatz: %s hat keine Punkte.\n", Data_Names[m], AnzDatenPunkte);
        }

    }


    // draw the multigraph
    // "A"	Axis are drawn around the graph
    // "C"	A smooth Curve is drawn
    // "P"	The current marker is plotted at each point
    // "Z"	Do not draw small horizontal and vertical lines the end of the error bars. Without "Z", the default is to draw these.

    c2->Update();

}

int CalculateChi2AtFixedEnergy(int fTaggCh, int fSelectedAsym, int fSelectedData = EPeter, int fCompareToModel = -1) {

    double Chi2Model = 0;
    double Chi2NullTest = 0;

    TH2D *hVerteilung2D = (TH2D*) gROOT->FindObject("hVerteilung2D");
    if (!hVerteilung2D) printf("ERROR: Histo hVerteilung2D not found.\n");

    //Model Werte aus den Fits nehmen:
    Double_t Param[NumberOfLegendreParameters];
    for (int i=0;i<NumberOfLegendreParameters;i++) {
        Param[i] = FitResults.at(i)->GetBinContent(fTaggCh+1, fSelectedData+1);
        //printf("%6f\t",Param[i]);
    }
    //printf("\n");
    Double_t WertX[1];

    //Alle Theta Bins durchgehen
    for (int ThetaBin=0; ThetaBin<DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.size(); ThetaBin++) {
        double ThetaPeter = DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.at(ThetaBin).Theta;
        int ThetaBinModel = DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at((int)round(ThetaPeter)).Theta;
//        printf("X: %f", ThetaPeter);
//        printf("\tX: %f", DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at((int)round(ThetaPeter)).Theta);
        double WertPeter = 0;
        double FehlerPeter = 0;
        double WertModel = 0;
        //Werte aus den Daten und dem Model heraussuchen
        if (fSelectedAsym==1) {
            WertPeter = DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.at(ThetaBin).ValueF;
            FehlerPeter = DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.at(ThetaBin).ValueFError;
            WertModel = DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(ThetaBinModel).ValueF;
        } else {
            WertPeter = DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.at(ThetaBin).ValueT;
            FehlerPeter = DataAll.at(fTaggCh).DataSets.at(fSelectedData).Values.at(ThetaBin).ValueTError;
            WertModel = DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at(ThetaBinModel).ValueT;
        }
        //printf("Wert Model: %8f",WertModel);

        //Den WertModel durch den Wert bestimmt durch den Fit ersetzen
        ActualTaggCh = fTaggCh;
        WertX[0] = ThetaPeter;
        WertModel = FitFunction(&(WertX[0]), &(Param[0])) /
            DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( ThetaBinModel ).ValueDSG;

        //printf("\t Wert Fit: %8f",WertModel);

        //Modell Werte aus der Erwartung =0 nehmen
        if (!fCompareToModel) WertModel = 0; //Füe Tests bei FPrime und TPrime
        //printf("\t Wert Ende: %8f\n",WertModel);

        double ActChi2Model = 0;
        double ActChi2NullTest = 0;
        if (FehlerPeter != 0) {
            ActChi2Model = pow(WertPeter - WertModel,2)/pow(FehlerPeter,2);
            ActChi2NullTest = pow(WertPeter - 0,2)/pow(FehlerPeter,2);
            if (std::isfinite(ActChi2Model)) {
                Chi2Model += ActChi2Model;
                Chi2NullTest += ActChi2NullTest;
            }

            hVerteilung2D->Fill( (WertPeter - WertModel)/FehlerPeter, fTaggCh );
       //     printf("\t%f", ActChi2Model);
       //     printf("\t%f", ActChi2NullTest);
        }

       // printf("\n");
    }

    printf("TaggCh, Chi2Model und NullTest: %i\t%f\t %f\n", fTaggCh, Chi2Model, Chi2NullTest);

}

int SavePeterDataInFitFileFormat(int fSelectedAsym) {
    //******************** TErrorGraph *******************

    for (int fTaggCh=NumberOfMaxTaggerCh-1; fTaggCh>=0; fTaggCh--) {
        printf("E =  %f MeV, Wght = 1.00, Syst = 0.05\n", DataAll.at(fTaggCh).TaggerChE);
        for (int ThetaBin=0; ThetaBin<DataAll.at(fTaggCh).DataSets.at(EPeter).Values.size(); ThetaBin++) {
            if (fSelectedAsym==1) {
                printf("%f\t%f\t%f\n", DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).Theta,
                       DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).ValueF,
                       DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).ValueFError);
            } else {
                printf("%f\t%f\t%f\n", DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).Theta,
                       DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).ValueT,
                       DataAll.at(fTaggCh).DataSets.at(EPeter).Values.at(ThetaBin).ValueTError);
            }
        }
        printf("-------------------------------------------\n");
    }




}

// ********************************************************************

int CalculateSystematicDifference() {
    Double_t WertX[1];
    char tempStr[1000];

    //Fahrplan:
    //1. Legendre Parameter 0, 1, 2, 3 aus den phänologischen Fits bestimmen
    //2. Fehler nicht benutzen
    //3. Diese Werte dann in die LegendreFunktion einsetzen

    TF1 *PhFitsPeter[NumberOfLegendreParameters];
    TF1 *PhFitsSven[NumberOfLegendreParameters];
    for (int i=0;i<NumberOfLegendreParameters;i++) { //i = loop through parameters, normal
        sprintf(tempStr,"Fit_%s_LegendreP%i",Data_Names[EPeter],i);
        PhFitsPeter[i] = (TF1*) gROOT->FindObject(tempStr);
        if (!PhFitsPeter[i]) { printf("FEHLER: Fits %s nicht gefunden.\n",tempStr); exit; }

        sprintf(tempStr,"Fit_%s_LegendreP%i",Data_Names[ESven],i);
        PhFitsSven[i] = (TF1*) gROOT->FindObject(tempStr);
        if (!PhFitsSven[i]) { printf("FEHLER: Fits %s nicht gefunden.\n",tempStr); exit; }
    }


    //Model Werte aus den Fits nehmen:
    Double_t ParamPeter[NumberOfLegendreParameters];
    Double_t ParamSven[NumberOfLegendreParameters];
    for (int fTaggCh=110;fTaggCh<111;fTaggCh++) { //NumberOfMaxTaggerCh
        for (int i=0;i<NumberOfLegendreParameters;i++) {
            ParamPeter[i] = PhFitsPeter[i]->Eval(fTaggCh);
            ParamSven[i] = PhFitsSven[i]->Eval(fTaggCh);
            //printf("%6f\t",ParamPeter[i]);
        }

        //Den WertModel durch den Wert bestimmt durch den Fit ersetzen
        ActualTaggCh = fTaggCh;

        //Alle Theta Bins durchgehen
        for (int ThetaBin=0; ThetaBin<DataAll.at(fTaggCh).DataSets.at(ESven).Values.size(); ThetaBin++) {
            double ThetaSven = DataAll.at(fTaggCh).DataSets.at(ESven).Values.at(ThetaBin).Theta;

            WertX[0] = ThetaSven;
            double WertModelPeter = FitFunction(&(WertX[0]), &(ParamPeter[0])); //DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( ThetaBinModel ).ValueDSG
            double WertModelSven = FitFunction(&(WertX[0]), &(ParamSven[0])); //DataAll.at(fTaggCh).DataSets.at(EMAIDDMT).Values.at( ThetaBinModel ).ValueDSG

            printf("Theta: %f=%8f %8f\n", ThetaSven, WertModelPeter, WertModelSven);

        }
    }
    printf("\n");

}


//*********************************************************************

int main(int argc, char **argv) {
    int i;
    char tempStr[1000];

    //-------------- Ok nun eingelesen, mache etwas mit den Daten
    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};
    TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);
    //The following line needs to be before each Draw, does not work otherwise in compiled program
    gStyle->SetOptFit(); //To get the Fit Parameters into the FitPanel.
    gStyle->SetOptStat("100000000001"); //Only the Text in the Stat Window

    Int_t fSelectedAsym = 0; //0=T, 1=F
    int ActualSetupNumber = 0;
    int TempInt;
    for (i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(tempStr, argv[i]);
            TempInt = strtol(tempStr, (char **)NULL, 10);
            if (ActualSetupNumber==0) {
                ActualSetupNumber++;
                TaggerChToLookat = TempInt;
                Printf("TaggerChToLookat: %d\n", TaggerChToLookat);
            } else if (ActualSetupNumber==1) {
                ActualSetupNumber++;
            }
        } else {
            if (!strcmp("-F", argv[i])) fSelectedAsym = 1;
            if (!strcmp("-T", argv[i])) fSelectedAsym = 0;
            if (!strcmp("-MAID", argv[i])) DoNotDrawMAID = -1;
            if (!strcmp("-Sven", argv[i])) DoNotDrawSven = -1;
            if (!strcmp("-Peter", argv[i])) DoNotDrawPeter = -1;
            if (!strcmp("-PeterPrime", argv[i])) DoNotDrawPeterPrime = -1;
            if (!strcmp("-Pauline", argv[i])) DoNotDrawPauline = -1;
            if (!strcmp("-MAIDDMT", argv[i])) DoNotDrawMAIDDMT = -1;
            if (!strcmp("-Asymmetry", argv[i])) fMultiplyDSG = 0;
            if (!strcmp("-Threshold", argv[i])) DrawThresholdsRegion = -1;
            if (!strcmp("-DoNotCompareToModel", argv[i])) DoCompareToModel = 0;
            if (!strcmp("-CheckPeter", argv[i])) CheckDataNr = EPeter;
            if (!strcmp("-CheckPeterPrime", argv[i])) { CheckDataNr = EPeterPrime; DoCompareToModel = 0;}
            if (!strcmp("-CheckSven", argv[i])) CheckDataNr = ESven;
            if (!strcmp("-CheckPauline", argv[i])) CheckDataNr = EPauline;
            if (!strcmp("-b", argv[i])) DoBatchMode = -1;
        }
    }

    switch (fSelectedAsym) {
    case 0: Printf("INFO: Selected Asym T\n");
        break;
    case 1: Printf("INFO: Selected Asym F\n");
        break;
    default: Printf("INFO: Selected Asym T\n");
        fSelectedAsym = 0;
        break;
    }
    if (CheckDataNr>NumberOfDataSets-1) CheckDataNr = NumberOfDataSets-1;
    printf("Setting DoCompareToModel: %d\n", DoCompareToModel);
    printf("Setting CheckDataNr: %s\n", Data_Names[CheckDataNr]);

    for (i=0;i<257; i++) {
        CreateVector(i);
    }
    Printf("Vector created.");

    ReadTaggerCalibration();
    Printf("Tagger Calibration read.");

    ReadDataMAID();
    ReadDataMAIDDMT();
    ReadDataPauline();
    ReadDataSven();
    ReadDataPeter();
    ReadDataPeterPrime();
    Printf("Data read.");

    FitResults.at(0) = new TH2D("FitResults_P0", "FitResults_P0", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(1) = new TH2D("FitResults_P1", "FitResults_P1", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(2) = new TH2D("FitResults_P2", "FitResults_P2", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(3) = new TH2D("FitResults_P3", "FitResults_P3", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(4) = new TH2D("FitResults_Chi2", "FitResults_Chi2", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);



    for (i=0; i<257; i++) {
        //if (QualityOfTaggCh[i]) { //Nur gute Kanäle
        if (1) {
            ProcessTaggCh(i, 0, fSelectedAsym, fMultiplyDSG); //int fTaggCh, int fDrawGraphs, int fAsymToAnalyse (0=T, 1=F), Plot Asymmetry or A*DSG
            printf(".");
        } else {
            printf("X");
        }
    }
    Printf("Fitting completed.");

    for (int i=0; i<NumberOfDataSets; i++) {
        printf("Skipped Histos for DataSet %s: %d\n", Data_Names[i], Data_NumberOfSkippedChannelsDuringFitting[i]);
    }

    /*
    TCanvas *c1 = new TCanvas("CanvasFitResults");
    c1->Divide(4,1);
    for (i=0;i<4;i++) { //Number of free Fit Parameters put in here
        c1->cd(i+1); FitResults.at(i)->Draw("COLZ");
    }*/

    //***********************************************************************************************************
    TCanvas *cMR = new TCanvas("CanvasModelRatioResults");
    cMR->Divide(3,2);
    int ParameterToLookAt = 1;
    TH1D *hModel = (TH1D*)FitResults.at(ParameterToLookAt)->ProjectionX("Model_Parameter",1+EMAIDDMT,1+EMAIDDMT);
//    TH1D *hPeterResults = (TH1D*)FitResults.at(ParameterToLookAt)->ProjectionX("Sven_Parameter",1+ESven,1+ESven);
    TH1D *hPeterResults = (TH1D*)FitResults.at(ParameterToLookAt)->ProjectionX("Peter_Parameter",1+EPeter,1+EPeter);
    TH1D *hDiff = (TH1D*)hModel->Clone("ModelPeterDiff");
    hDiff->Add(hPeterResults,-1.);
    TH1D *hDiffRel = (TH1D*)hDiff->Clone("ModelPeterDiffRel");
    hDiffRel->Divide(hModel);

    TH1D *hDiffRelHistoAll = new TH1D("DiffRelHistoAll","DiffRelHistoAll", 100,-2,2);
    TH1D *hDiffRelHistoGood = new TH1D("DiffRelHistoThreshold","DiffRelHistoThreshold", 100,-2,2);
    for (i=1; i<257; i++) {
        if ((hDiffRel->GetBinContent(i)!=0) && (hDiffRel->GetBinError(i) != 0)) {
            hDiffRelHistoAll->Fill(hDiffRel->GetBinContent(i));
//            if (QualityOfTaggCh[i]) {
            if (i>220) {
                hDiffRelHistoGood->Fill(hDiffRel->GetBinContent(i));
            }
        }
    }

    cMR->cd(1);  hModel->Draw();
    cMR->cd(2);  hPeterResults->Draw();
    cMR->cd(3);  hDiffRelHistoAll->Draw();
    cMR->cd(4);  hDiff->Draw();      hDiff->SetAxisRange(-50,50,"Y");
    cMR->cd(5);  hDiffRel->Draw();   hDiffRel->SetAxisRange(-2,2,"Y");
    cMR->cd(6);  hDiffRelHistoGood->Draw();
    //***********************************************************************************************************



    sprintf(tempStr, "DataCompiled_F");
    if (fSelectedAsym==0) sprintf(tempStr, "DataCompiled_T");

    TCanvas *c2 = new TCanvas(tempStr);
    c2->Divide(3,2);
    int NHistosDrawn = 0;
    for (i=0;i<NumberOfLegendreParameters+1;i++) { //i = loop through parameters plus Chi2
        c2->cd(i+1);
        //if (i==3) i=4; //Skip Legendre Parameter i==3
        NHistosDrawn = 0;

        for (int k=0;k<NumberOfDataSets;k++) { //k = loop through results from people
            if ( ( (k==ESven) || (k==EPeter) || (k==EPauline) ) && (i<NumberOfLegendreParameters) ) {
                sprintf(tempStr, "Parameter_%d_%d", i, k);

                TH1D *h1 = (TH1D*)FitResults.at(i)->ProjectionX(tempStr,k+1,k+1);

                printf("Start to Fit %s Parameter %d: ",Data_Names[k],i);
                //Fit Start
                sprintf(tempStr,"Fit_%s_LegendreP%i",Data_Names[k],i);
                delete gROOT->FindObject(tempStr);
                TF1 *PrevFitTMP = new TF1(tempStr,"pol3(0)/pol2(4)",0,257);
                PrevFitTMP->SetLineColor(Data_Colors[k]);
                PrevFitTMP->SetLineWidth(2);
                PrevFitTMP->SetParameter(4, 1);
                TFitResultPtr MyFitResult;
                MyFitResult = h1->Fit(tempStr, "MQRF");
                MyFitResult = h1->Fit(tempStr, "WMQRF");
                MyFitResult = h1->Fit(tempStr, "MQRF");
                Int_t MyFitStatus = 0;
                MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data,
                                     //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
                printf("FitStatus: %d\n", MyFitStatus);
                //Fit Ende
            }
        }

        for (int k=0;k<NumberOfDataSets;k++) { //k = loop through results from people
            if ((k==EMAID) &&(DoNotDrawMAID)) continue;
            if ((k==ESven) &&(DoNotDrawSven)) continue;
            if ((k==EPeter) &&(DoNotDrawPeter)) continue;
            if ((k==EPauline) &&(DoNotDrawPauline)) continue;
            if ((k==EMAIDDMT) &&(DoNotDrawMAIDDMT)) continue;
            if (i<4) {
                sprintf(tempStr, "Parameter_%d_%d", i, k);
            } else {
                sprintf(tempStr, "Chi2/DoF_%d", k);
            }
            if (i==4) { //Falls Chi/DoF
                if (k==EMAID) continue;
                if (k==EMAIDDMT) continue;
            }

            TH1D *h1 = (TH1D*)gROOT->FindObject(tempStr);
            if (!h1) {
                h1 = (TH1D*)FitResults.at(i)->ProjectionX(tempStr,k+1,k+1);
            }

            h1->SetLineColor(Data_Colors[k]);

            if (NHistosDrawn==0) {
                h1->Draw("");
                if (DrawThresholdsRegion) h1->SetAxisRange(0,56,"X");
                if (fSelectedAsym==0) { //T
                    switch (i) {
                    case 0: h1->SetAxisRange(-4,7,"Y"); break;
                    case 1: h1->SetAxisRange(-15,3,"Y"); break;
                    case 2: h1->SetAxisRange(-5,10,"Y"); break;
                    case 3: h1->SetAxisRange(-7,7,"Y"); break;
                    case 4: h1->SetAxisRange(0,8,"Y"); break;
                    }
                } else {
                    switch (i) {
                    case 0: h1->SetAxisRange(-8,3,"Y"); break;
                    case 1: h1->SetAxisRange(-50,10,"Y"); break;
                    case 2: h1->SetAxisRange(-10,12,"Y"); break;
                    case 3: h1->SetAxisRange(-10,10,"Y"); break;
                    case 4: h1->SetAxisRange(0,8,"Y"); break;
                    }
                }
            } else {
                h1->Draw("same");
            }
            NHistosDrawn++;
        }
    }

    TH2D *hVerteilung2D = new TH2D("hVerteilung2D", "hVerteilung2D", 10, -5, 5, NumberOfMaxTaggerCh, 0, NumberOfMaxTaggerCh);
    //PlotDataAtFixedAngle(fSelectedAsym); //Bei Bedarf anzeigen lassen
    if (TaggerChToLookat>=0) PlotDataAtFixedEnergy(TaggerChToLookat, fSelectedAsym);
    //for (i=0; i<257; i++) { CalculateChi2AtFixedEnergy(i, fSelectedAsym, CheckDataNr, DoCompareToModel); } //Bei Bedarf ausrechnen lassen

    if (TaggerChToLookat>=0) CalculateChi2AtFixedEnergy(TaggerChToLookat, fSelectedAsym, CheckDataNr, DoCompareToModel);
    //TCanvas *c3 = new TCanvas(); hVerteilung2D->Draw("COLZ"); //Bei Bedarf anzeigne lassen

    CalculateSystematicDifference();

    //if (TaggerChToLookat>=0) ProcessTaggCh(TaggerChToLookat, -1, fSelectedAsym, 0);
    //SavePeterDataInFitFileFormat(fSelectedAsym); //Print out Peters Data in Svens File Format

    theApp->SetPrompt("NewCompare %d: ");
    if (!DoBatchMode) theApp->Run();

    return 0;
}
