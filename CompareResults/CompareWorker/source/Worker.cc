#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include <TRint.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <TThread.h>
#include <TFile.h>

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

const Int_t NumberOfThetaBins = 181;

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
    Counter = 0;
    ActTheta = 20;
    ThetaBinWidth = 15;
    while (ActTheta<170) {
        Counter++;
        TValue TempTValue;
        TempTValue.Theta = ActTheta;
        TempTValue.ThetaError = ThetaBinWidth/sqrt(12);
        TempTValue.ValueF = -10;
        TempTValue.ValueFError = -10;
        TempTValue.ValueT = -10;
        TempTValue.ValueTError = -10;
        TempDataAll.ValuesPauline.push_back(TempTValue);
        TempDataAll.NValuesPauline = Counter;
        TempDataAll.ThetaBinWidthPauline = ThetaBinWidth;

        ActTheta+=ThetaBinWidth;
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
    ActTheta = 10;
    ThetaBinWidth = 20;
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
        ReadDataFileTheory(i);
    }
}

int ReadDataPauline() {
    for (int i=0;i<212; i++) { //Pauline went down to Tagger Channel 211 (including)
        ReadDataFilePauline(0, i); //F
        ReadDataFilePauline(1, i); //T
    }
}

int ReadDataSven() {
    ReadDataFileSven(0);
    ReadDataFileSven(1);
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


int DrawCanvas(int fTaggCh) {
    TCanvas* Canvas = new TCanvas();

    gROOT->SetStyle("Plain");

    TH1F *HistPauline, *HistSven;

    HistPauline = new TH1F("Pauline", "Pauline", DataAll.at(fTaggCh).NValuesPauline,
                           DataAll.at(fTaggCh).ValuesPauline.at(0).Theta - DataAll.at(fTaggCh).ThetaBinWidthPauline/2,
                           DataAll.at(fTaggCh).ValuesPauline.at(0).Theta +
                           DataAll.at(fTaggCh).ThetaBinWidthPauline * DataAll.at(fTaggCh).NValuesPauline );
    HistSven = new TH1F("Sven", "Sven", DataAll.at(fTaggCh).NValuesSven,
                           DataAll.at(fTaggCh).ValuesSven.at(0).Theta - DataAll.at(fTaggCh).ThetaBinWidthSven/2,
                           DataAll.at(fTaggCh).ValuesSven.at(0).Theta +
                           DataAll.at(fTaggCh).ThetaBinWidthSven * DataAll.at(fTaggCh).NValuesSven );

    for(Int_t t=0; t < DataAll.at(fTaggCh).NValuesPauline; t++) {
        HistPauline->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueF );
        HistPauline->SetBinError(t+1, DataAll.at(fTaggCh).ValuesPauline.at(t).ValueFError );
    }
    HistPauline->SetLineColor(kRed);
    HistPauline->SetMarkerColor(kRed);
    HistPauline->SetMarkerStyle(20);
    HistPauline->SetMarkerSize(0.9);

    for(Int_t t=0; t < DataAll.at(fTaggCh).NValuesSven; t++) {
        HistSven->SetBinContent(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueF /
                                DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG );
        HistSven->SetBinError(t+1, DataAll.at(fTaggCh).ValuesSven.at(t).ValueFError /
                                DataAll.at(fTaggCh).TheoryValues.at( lround(DataAll.at(fTaggCh).ValuesSven.at(t).Theta) ).ValueDSG);
    }
    HistSven->SetLineColor(kBlack);
    HistSven->SetMarkerColor(kBlack);
    HistSven->SetMarkerStyle(20);
    HistSven->SetMarkerSize(0.9);

    HistSven->Draw("");
    HistPauline->Draw("same");
}


int main(int argc, char **argv) {

    //-------------- Ok nun eingelesen, mache etwas mit den Daten
    //Prevent Root from showing a big info screen.
    const int NArgs = 2;
    int Myargc = NArgs;
    char *Myargv[NArgs] = {argv[0], (char*)"-l"};
    TRint *theApp = new TRint("example", &Myargc, Myargv);
    gStyle->SetPalette(1);


    int i;

    for (i=0;i<257; i++) {
        CreateVector(i);
    }

    ReadTheoryData();

    ReadDataPauline();
    ReadDataSven();

/*
//    for (i = 0;i<NDataAll;i++) {
        for (i = 0;i<200;i++) {
        printf("TaggCh: %i\tNValuePauline: %i\tNValueSven: %i\tNValuePeter: %i\n", DataAll.at(i).TaggCh,
               DataAll.at(i).NValuesPauline, DataAll.at(i).NValuesSven, DataAll.at(i).NValuesPeter);
        PrintDataInformation(i);
    }
*/
    DrawCanvas(10);
    DrawCanvas(210);


    theApp->SetPrompt("NewCompare %d: ");

    //TBrowser *TB = new TBrowser();

    theApp->Run();

    return 0;
}
