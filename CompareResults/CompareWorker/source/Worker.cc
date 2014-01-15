#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

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

#include <Worker.h>

Char_t AbsolutePathToData[] = "/datapool/home/otte/NewAnalysis/CompareResults/input";

typedef struct TValue {
    double Theta;
    double ThetaError;
    double ValueF;
    double ValueFError;
    double ValueT;
    double ValueTError;
} TValue;

typedef struct TTheoryValue {
    double Theta;
    double ValueDSG;
    double ValueF;
    double ValueT;
} TTheoryValue;

typedef struct TDataAll {
    int TaggCh;
    double TaggerChE; //Energy of this TaggerCh

    std::vector<TValue> ValuesPauline;
    int NValuesPauline;
    double ThetaBinWidthPauline;
    std::vector<TValue> ValuesSven;
    int NValuesSven;
    double ThetaBinWidthSven;
    std::vector<TValue> ValuesPeter;
    int NValuesPeter;
    double ThetaBinWidthPeter;
    std::vector<TTheoryValue> TheoryValues;
    int NTheoryValue;
} TDataAll;

std::vector<TDataAll> DataAll;
int NDataAll = 0; //NumberOfEntries in DataAll
const Int_t NumberOfThetaBins = 181;

std::vector<TH2D*> FitResults(4);


#include "FitFunctions.cc"


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
            DataAll.at(fTaggCh).ThetaBinWidthPauline = 15;
            if (fType == 0) {
                DataAll.at(fTaggCh).ValuesPauline.at(Counter).ValueF = Value0;
                DataAll.at(fTaggCh).ValuesPauline.at(Counter).ValueFError = Value1;
            } else {
                DataAll.at(fTaggCh).ValuesPauline.at(Counter).ValueT = Value0;
                DataAll.at(fTaggCh).ValuesPauline.at(Counter).ValueTError = Value1;
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

int ReadDataFileTheory(int fTaggCh) {
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
            DataAll.at(fTaggCh).TheoryValues.at(Counter).Theta = TempValue[0];
            DataAll.at(fTaggCh).TheoryValues.at(Counter).ValueDSG = TempValue[2];
            DataAll.at(fTaggCh).TheoryValues.at(Counter).ValueF = TempValue[7];
            DataAll.at(fTaggCh).TheoryValues.at(Counter).ValueT = TempValue[3];
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
                    if (fType == 0) {
                        DataAll.at(TaggCh).ValuesSven.at(Counter).ValueF = Value2;
                        DataAll.at(TaggCh).ValuesSven.at(Counter).ValueFError = Value3;
                    } else {
                        DataAll.at(TaggCh).ValuesSven.at(Counter).ValueT = Value2;
                        DataAll.at(TaggCh).ValuesSven.at(Counter).ValueTError = Value3;
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
            if (fType == 0) {
                DataAll.at(TaggCh).ValuesPeter.at(Counter).ValueF = Value2;
                DataAll.at(TaggCh).ValuesPeter.at(Counter).ValueFError = Value3;
            } else {
                DataAll.at(TaggCh).ValuesPeter.at(Counter).ValueT = Value2;
                DataAll.at(TaggCh).ValuesPeter.at(Counter).ValueTError = Value3;
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
    TempDataAll.NValuesPauline = 0;
    TempDataAll.NValuesSven = 0;
    TempDataAll.NValuesPeter = 0;

    int Counter = 0;
    double ActTheta = 0;
    double ThetaBinWidth = 0;

    //Theory
    Counter = 0;
    ActTheta = 0;
    ThetaBinWidth = 1;
    while (ActTheta <= 180) {
        Counter++;
        TTheoryValue TempTValue;
        TempTValue.Theta = -10;
        TempTValue.ValueDSG = -10;
        TempTValue.ValueF = -10;
        TempTValue.ValueT= -10;
        TempDataAll.TheoryValues.push_back(TempTValue);
        TempDataAll.NTheoryValue = Counter;

        ActTheta+=ThetaBinWidth;
    }

    //Pauline
    //if (fTaggCh<=211)
    {
        Counter = 0;
        ActTheta = 20;
        ThetaBinWidth = 15;
        while (ActTheta<170) {
            Counter++;
            TValue TempTValue;
            TempTValue.Theta = ActTheta;
            TempTValue.ThetaError = ThetaBinWidth/sqrt(12);
            TempTValue.ValueF = 0;
            TempTValue.ValueFError = 0;
            TempTValue.ValueT = 0;
            TempTValue.ValueTError = 0;
            TempDataAll.ValuesPauline.push_back(TempTValue);
            TempDataAll.NValuesPauline = Counter;
            TempDataAll.ThetaBinWidthPauline = ThetaBinWidth;

            ActTheta+=ThetaBinWidth;
        }
    }

    //Sven
    Counter = 0;
    ActTheta = 5;
    ThetaBinWidth = 10;
    while (ActTheta<180) {
        Counter++;
        TValue TempTValue;
        TempTValue.Theta = ActTheta;
        TempTValue.ThetaError = ThetaBinWidth/sqrt(12);
        TempTValue.ValueF = -10;
        TempTValue.ValueFError = -10;
        TempTValue.ValueT = -10;
        TempTValue.ValueTError = -10;
        TempDataAll.ValuesSven.push_back(TempTValue);
        TempDataAll.NValuesSven = Counter;
        TempDataAll.ThetaBinWidthSven = ThetaBinWidth;

        ActTheta+=ThetaBinWidth;
    }

    //Peter
    Counter = 0;
    ActTheta = 5;
    ThetaBinWidth = 10;
    while (ActTheta<180) {
        Counter++;
        TValue TempTValue;
        TempTValue.Theta = ActTheta;
        TempTValue.ThetaError = ThetaBinWidth/sqrt(12);
        TempTValue.ValueF = -10;
        TempTValue.ValueFError = -10;
        TempTValue.ValueT = -10;
        TempTValue.ValueTError = -10;
        TempDataAll.ValuesPeter.push_back(TempTValue);
        TempDataAll.NValuesPeter = Counter;
        TempDataAll.ThetaBinWidthPeter = ThetaBinWidth;

        ActTheta+=ThetaBinWidth;
    }

    DataAll.push_back(TempDataAll);
    NDataAll++;
}

int ReadTheoryData() {
    for (int i=0;i<257; i++) {
        ReadDataFileTheory(i); //T and F at the same time
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
    printf("Theory\n");
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
    printf("\n");
    printf("\n");
}

int ProcessTaggCh(int fTaggCh, int fDrawGraphs, int fAsymToAnalyse) {
    if (fDrawGraphs) TCanvas* Canvas = new TCanvas("Draw");

    gROOT->SetStyle("Plain");

    TH1F *HistPauline, *HistSven, *HistPeter;

    delete gROOT->FindObject("Pauline");
    delete gROOT->FindObject("Sven");
    delete gROOT->FindObject("Peter");
    HistPauline = new TH1F("Pauline", "Pauline", DataAll.at(fTaggCh).NValuesPauline,
                           DataAll.at(fTaggCh).ValuesPauline.at(0).Theta - DataAll.at(fTaggCh).ThetaBinWidthPauline/2,
                           DataAll.at(fTaggCh).ValuesPauline.at(0).Theta +
                           DataAll.at(fTaggCh).ThetaBinWidthPauline * DataAll.at(fTaggCh).NValuesPauline );
    HistSven = new TH1F("Sven", "Sven", DataAll.at(fTaggCh).NValuesSven,
                           DataAll.at(fTaggCh).ValuesSven.at(0).Theta - DataAll.at(fTaggCh).ThetaBinWidthSven/2,
                           DataAll.at(fTaggCh).ValuesSven.at(0).Theta +
                           DataAll.at(fTaggCh).ThetaBinWidthSven * DataAll.at(fTaggCh).NValuesSven );
    HistPeter = new TH1F("Peter", "Peter", DataAll.at(fTaggCh).NValuesPeter,
                           DataAll.at(fTaggCh).ValuesPeter.at(0).Theta - DataAll.at(fTaggCh).ThetaBinWidthPeter/2,
                           DataAll.at(fTaggCh).ValuesPeter.at(0).Theta +
                           DataAll.at(fTaggCh).ThetaBinWidthPeter * DataAll.at(fTaggCh).NValuesPeter );

    for(Int_t t=0; t < DataAll.at(fTaggCh).NValuesPauline; t++) {
        if (fAsymToAnalyse) {
            //Plot Asymmetry
            HistPauline->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueF );
            HistPauline->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueFError );
            //PlotAsymmetry * DSG
            HistPauline->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueF *
                                       DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPauline.at(t).Theta) ).ValueDSG );
            HistPauline->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueFError *
                                     DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPauline.at(t).Theta) ).ValueDSG );
        } else {
            //Plot Asymmetry
            HistPauline->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueT );
            HistPauline->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueTError );
            //PlotAsymmetry * DSG
            HistPauline->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueT *
                                       DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPauline.at(t).Theta) ).ValueDSG );
            HistPauline->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueTError *
                                     DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPauline.at(t).Theta) ).ValueDSG );
        }
    }
    HistPauline->SetLineColor(kGreen);
    HistPauline->SetMarkerColor(kGreen);
    HistPauline->SetMarkerStyle(20);
    HistPauline->SetMarkerSize(0.9);

    for(Int_t t=0; t < DataAll.at(fTaggCh).NValuesSven; t++) {
        if (fAsymToAnalyse) {
            //Plot Asymmetry
            HistSven->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueF /
                                    DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG );
            HistSven->SetBinError(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueFError /
                                    DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG);
            //Plot Asymmetry * DSG
            HistSven->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueF );
            HistSven->SetBinError(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueFError );
        } else {
            //Plot Asymmetry
            HistSven->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueT /
                                    DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG );
            HistSven->SetBinError(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueTError /
                                    DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG);
            //Plot Asymmetry * DSG
            HistSven->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueT );
            HistSven->SetBinError(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueTError );
        }
    }
    HistSven->SetLineColor(kBlack);
    HistSven->SetMarkerColor(kBlack);
    HistSven->SetMarkerStyle(20);
    HistSven->SetMarkerSize(0.9);

    for(Int_t t=0; t < DataAll.at(fTaggCh).NValuesPeter; t++) {
        if (fAsymToAnalyse) {
            //Plot Asymmetry
            HistPeter->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueF );
            HistPeter->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueFError );
            //Plot Asymmetry * DSG
            HistPeter->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueF *
                                     DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPeter.at(t).Theta) ).ValueDSG );
            HistPeter->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueFError *
                                   DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPeter.at(t).Theta) ).ValueDSG );
        } else {
            //Plot Asymmetry
            HistPeter->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueT );
            HistPeter->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueTError );
            //Plot Asymmetry * DSG
            HistPeter->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueT *
                                     DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPeter.at(t).Theta) ).ValueDSG );
            HistPeter->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPeter.at(t).ValueTError *
                                   DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesPeter.at(t).Theta) ).ValueDSG );
        }
    }
    HistPeter->SetLineColor(kRed);
    HistPeter->SetMarkerColor(kRed);
    HistPeter->SetMarkerStyle(20);
    HistPeter->SetMarkerSize(0.9);

    //********************************************************************************

    ActualTaggCh = fTaggCh;
    gStyle->SetOptFit();
    char tempStr[1000];
    Int_t MyFitStatus = 0;
    char StrFitParameters[10];
    if (fDrawGraphs) {
        strcpy(StrFitParameters, "RFU+"); //0 = do not draw, q=quiet, R = respect range, f = special min finder,
                                    // U = adds results to box, + = add multiple fits to the same histo
    } else {
        strcpy(StrFitParameters, "0RFU+");
    }

    sprintf(tempStr,"DataFit_Sven_%i",fTaggCh);
    delete gROOT->FindObject(tempStr);
    TF1 *PrevFitTMP = new TF1(tempStr,FitFunction,0,180,4); //From ,to, NumberOfParameters
    PrevFitTMP->SetLineColor(kBlack);
    PrevFitTMP->SetLineWidth(2);
    PrevFitTMP->SetParameter(0, 1);
    PrevFitTMP->SetParameter(1, 0);
    PrevFitTMP->SetParameter(2, 0);
    PrevFitTMP->SetParameter(3, 0);
//	PrevFitTMP->FixParameter(2, 0);
    PrevFitTMP->SetParNames("StrengthSin","StrengthCos", "StrengthCos2", "StrengthCos3");
    TFitResultPtr MyFitResult = HistSven->Fit(tempStr, StrFitParameters);

    MyFitStatus = MyFitResult; //0 = alles okay, 4 fehler beim Fit, -1 = no data,
                         //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        for (int i=0;i<4;i++){
            FitResults.at(i)->SetBinContent(fTaggCh+1, 1, PrevFitTMP->GetParameter(i));
            FitResults.at(i)->SetBinError(fTaggCh+1, 1, PrevFitTMP->GetParError(i));
        }
    } else {
        printf("ERROR: Fit did not converge.\n");
    }

    //************************************************************************

    sprintf(tempStr,"DataFit_Peter_%i",fTaggCh);
    delete gROOT->FindObject(tempStr);
    PrevFitTMP = new TF1(tempStr,FitFunction,0,180,4); //From ,to, NumberOfParameters
    PrevFitTMP->SetLineColor(kRed);
    PrevFitTMP->SetLineWidth(2);
    PrevFitTMP->SetParNames("StrengthSin","StrengthCos", "StrengthCos2", "StrengthCos3");
    TFitResultPtr MyFitResult_Peter = HistPeter->Fit(tempStr, StrFitParameters);

    MyFitStatus = MyFitResult_Peter; //0 = alles okay, 4 fehler beim Fit, -1 = no data,
                         //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        for (int i=0;i<4;i++){
            FitResults.at(i)->SetBinContent(fTaggCh+1, 2, PrevFitTMP->GetParameter(i));
            FitResults.at(i)->SetBinError(fTaggCh+1, 2, PrevFitTMP->GetParError(i));
        }
    } else {
        printf("ERROR: Fit did not converge.\n");
    }

    //************************************************************************

    sprintf(tempStr,"DataFit_Pauline_%i",fTaggCh);
    delete gROOT->FindObject(tempStr);
    PrevFitTMP = new TF1(tempStr,FitFunction,0,180,4); //From ,to, NumberOfParameters
    PrevFitTMP->SetLineColor(kGreen);
    PrevFitTMP->SetLineWidth(2);
    PrevFitTMP->SetParNames("StrengthSin","StrengthCos", "StrengthCos2", "StrengthCos3");
    TFitResultPtr MyFitResult_Pauline = HistPauline->Fit(tempStr, StrFitParameters);

    MyFitStatus = MyFitResult_Pauline; //0 = alles okay, 4 fehler beim Fit, -1 = no data,
                         //see: http://root.cern.ch/root/html/TH1.html#TH1:Fit%1
    if (MyFitStatus == 0) {
        for (int i=0;i<4;i++){
            FitResults.at(i)->SetBinContent(fTaggCh+1, 3, PrevFitTMP->GetParameter(i));
            FitResults.at(i)->SetBinError(fTaggCh+1, 3, PrevFitTMP->GetParError(i));
        }
    } else {
        printf("ERROR: Fit did not converge.\n");
    }

    //************************************************************************

    if (fDrawGraphs) {
        HistSven->Draw("");
        HistPeter->Draw("same");
        HistPauline->Draw("same");
    }
}

int main(int argc, char **argv) {

    //-------------- Ok nun eingelesen, mache etwas mit den Daten
    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};
    TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);
    //The following line needs to be before each Draw, does not work otherwise in compiled program
    gStyle->SetOptFit(); //To get the Fit Parameters into the FitPanel.


    int i;
    char tempStr[1000];

    for (i=0;i<257; i++) {
        CreateVector(i);
    }
    ReadTaggerCalibration();
    FitResults.at(0) = new TH2D("FitResults_P0", "FitResults_P0", 257,0,256, 3,0,2);
    FitResults.at(1) = new TH2D("FitResults_P1", "FitResults_P1", 257,0,256, 3,0,2);
    FitResults.at(2) = new TH2D("FitResults_P2", "FitResults_P2", 257,0,256, 3,0,2);
    FitResults.at(3) = new TH2D("FitResults_P3", "FitResults_P3", 257,0,256, 3,0,2);

    ReadTheoryData();

    ReadDataPauline();
    ReadDataSven();
    ReadDataPeter();

/*
//    for (i = 0;i<NDataAll;i++) {
        for (i = 0;i<200;i++) {
        printf("TaggCh: %i\tNValuePauline: %i\tNValueSven: %i\tNValuePeter: %i\n", DataAll.at(i).TaggCh,
               DataAll.at(i).NValuesPauline, DataAll.at(i).NValuesSven, DataAll.at(i).NValuesPeter);
        PrintDataInformation(i);
    }
*/

//    ProcessTaggCh(230,-1, 0);
//    PrintDataInformation(230);
    for (i=0; i<257; i++) {
        ProcessTaggCh(i, 0, 1);
    }
    //PrintDataInformation(100);

/*    TCanvas *c1 = new TCanvas("CanvasFitResults");
    c1->Divide(4,1);
    for (i=0;i<4;i++) {
        c1->cd(i+1); FitResults.at(i)->Draw("COLZ");
    }
*/

    TCanvas *c2 = new TCanvas("CanvasCompiled");
    c2->Divide(2,2);
    for (i=0;i<4;i++) {
        c2->cd(i+1);
        for (int k=0;k<2;k++) {
            sprintf(tempStr, "Parameter_%d_%d", i, k);
            TH1D *h1 = (TH1D*)FitResults.at(i)->ProjectionX(tempStr,k+1,k+1);
            h1->SetLineColor(k+1);
            if (k==0) {
                h1->Draw("");
            } else {
                h1->Draw("same");
            }
        }
    }




    theApp->SetPrompt("NewCompare %d: ");

    //TBrowser *TB = new TBrowser();

    theApp->Run();

    return 0;
}
