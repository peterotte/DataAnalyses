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
    double ValueDSG;
    double ValueDSGError;
} TValue;

typedef struct TDataSetPoint {
	double ThetaBinWidth;
	std::vector<TValue> Values;
} TDataSetPoint;

enum Data_DataSets {EMAID, ESven, EPeter, EPauline, NumberOfDataSets};
const int Data_Colors[NumberOfDataSets] = {4, 1, 2, 3}; //The datasets from above do get the following colors: black, red, green, blue,
const char Data_Names[NumberOfDataSets][20] = {"MAID", "Sven", "Peter", "Pauline"};
const double Data_ThetaStartAngle[NumberOfDataSets] = {0, 5, 5, 20};
const double Data_ThetaBinWidth[NumberOfDataSets] = {1, 10, 10, 15};
int Data_NumberOfSkippedChannelsDuringFitting[NumberOfDataSets];

typedef struct TDataAll {
    int TaggCh;
    double TaggerChE; //Energy of this TaggerCh
    std::vector<TDataSetPoint> DataSets;
} TDataAll;

std::vector<TDataAll> DataAll;
const Int_t NumberOfThetaBins = 181;
const Int_t NumberOfMaxTaggerCh = 257;

std::vector<TH2D*> FitResults(5);


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
                    Value2 /= DataAll.at(TaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).Theta) ).ValueDSG;
                    Value3 /= DataAll.at(TaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(TaggCh).DataSets.at(ESven).Values.at(Counter).Theta) ).ValueDSG;

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
                                           DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueFError *
                                         DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
            } else {
                //Plot Asymmetry
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueF );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueFError );
            }
        } else { //T
            //PlotAsymmetry * DSG
            if (fMultiplyDSG) {
                MyHist->SetBinContent(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueT *
                                           DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
                MyHist->SetBinError(t+1, DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).ValueTError *
                                         DataAll.at(fTaggCh).DataSets.at(EMAID).Values.at( lround(DataAll.at(fTaggCh).DataSets.at(fDataSetNumber).Values.at(t).Theta) ).ValueDSG );
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
    if (fDrawGraphs) {
        strcpy(StrFitParameters, "RqUF+"); //0 = do not draw, q=quiet, R = respect range, f = special min finder,
                                    // U = adds results to box, + = add multiple fits to the same histo
    } else {
        strcpy(StrFitParameters, "0RqFU+");
    }

    for (int m=0; m<NumberOfDataSets; m++) {
        MyHist = (TH1D*)gROOT->FindObject(Data_Names[m]);
        if (MyHist->Integral() != 0) {
            sprintf(tempStr,"Fit_%s_Ch%i",Data_Names[m],fTaggCh);
            delete gROOT->FindObject(tempStr);
            TF1 *PrevFitTMP = new TF1(tempStr,FitFunction,0,180,4); //From ,to, NumberOfParameters
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
                for (int i=0;i<4;i++){
                    //zuvor: fTaggCh+1
                    FitResults.at(i)->SetBinContent(FitResults.at(i)->GetNbinsX()-fTaggCh, m+1, PrevFitTMP->GetParameter(i));
                    FitResults.at(i)->SetBinError(FitResults.at(i)->GetNbinsX()-fTaggCh, m+1, PrevFitTMP->GetParError(i));
                }
                if ( (PrevFitTMP->GetChisquare() > 0.001) && (m != EMAID) ) FitResults.at(4)->SetBinContent(FitResults.at(4)->GetNbinsX()-fTaggCh, m+1, PrevFitTMP->GetChisquare()/PrevFitTMP->GetNDF());
                FitResults.at(4)->SetBinError(FitResults.at(4)->GetNbinsX()-fTaggCh, m+1, 0);
            } else {
                printf("|%d|",m);
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

int PlotDataAtFixedAngle() {
    TCanvas *c1 = new TCanvas("CanvasFixedAngle");
    char tempStr[1000];

    int NDrawnHisto = 0;
    const int Data_Bins[NumberOfDataSets] = {45, 4, 4, 2};

    TH1D *hFixedAngle[NumberOfDataSets];
    //for (int m=0; m<1; m++) {
        for (int m=0; m<NumberOfDataSets; m++) {
        sprintf(tempStr, "FixedAngle_%s", Data_Names[m]);
        hFixedAngle[m] = new TH1D(tempStr, tempStr, NumberOfMaxTaggerCh, 0, NumberOfMaxTaggerCh );
        hFixedAngle[m]->SetLineColor(Data_Colors[m]);
        hFixedAngle[m]->SetAxisRange(-2,2,"Y");

        printf("Plot for theta angle %f.\n", DataAll.at(0).DataSets.at(m).Values.at(Data_Bins[m]).Theta );
        for (int fTaggCh=0; fTaggCh<NumberOfMaxTaggerCh; fTaggCh++) {
            hFixedAngle[m]->SetBinContent(fTaggCh+1, DataAll.at(fTaggCh).DataSets.at(m).Values.at(Data_Bins[m]).ValueF);
        }

        if (NDrawnHisto) {
            hFixedAngle[m]->Draw("same");
        } else {
            hFixedAngle[m]->Draw("");
        }

        NDrawnHisto++;
    }

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
    int DoNotDrawSven = 0;
    int DoNotDrawPeter = 0;
    int DoNotDrawPauline = 0;
    int fMultiplyDSG = -1;
    int ActualSetupNumber = 0;
    int DrawThresholdsRegion = 0;
    int TempInt;
    for (i=1; i< argc; i++) {
        if (strncmp("--", argv[i], 1)) {
            strcpy(tempStr, argv[i]);
            TempInt = strtol(tempStr, (char **)NULL, 10);
            if (ActualSetupNumber==0) {
                fSelectedAsym = TempInt;
                ActualSetupNumber++;
            } else if (ActualSetupNumber==1) {
                ActualSetupNumber++;
                TaggerChToLookat = TempInt;
                Printf("TaggerChToLookat: %d\n", TaggerChToLookat);
            }
        } else {
            if (!strcmp("-Sven", argv[i])) DoNotDrawSven = -1;
            if (!strcmp("-Peter", argv[i])) DoNotDrawPeter = -1;
            if (!strcmp("-Pauline", argv[i])) DoNotDrawPauline = -1;
            if (!strcmp("-Asymmetry", argv[i])) fMultiplyDSG = 0;
            if (!strcmp("-Threshold", argv[i])) DrawThresholdsRegion = -1;
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

    for (i=0;i<257; i++) {
        CreateVector(i);
    }
    Printf("Vector created.");

    ReadTaggerCalibration();
    Printf("Tagger Calibration read.");

    ReadTheoryData();
    ReadDataPauline();
    ReadDataSven();
    ReadDataPeter();
    Printf("Data read.");

    FitResults.at(0) = new TH2D("FitResults_P0", "FitResults_P0", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(1) = new TH2D("FitResults_P1", "FitResults_P1", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(2) = new TH2D("FitResults_P2", "FitResults_P2", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(3) = new TH2D("FitResults_P3", "FitResults_P3", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);
    FitResults.at(4) = new TH2D("FitResults_Chi2", "FitResults_Chi2", NumberOfMaxTaggerCh,0,NumberOfMaxTaggerCh, NumberOfDataSets,0,NumberOfDataSets);



    for (i=0; i<257; i++) {
        ProcessTaggCh(i, 0, fSelectedAsym, fMultiplyDSG); //int fTaggCh, int fDrawGraphs, int fAsymToAnalyse (0=T, 1=F), Plot Asymmetry or A*DSG
        printf(".");
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


    sprintf(tempStr, "DataCompiled_F");
    if (fSelectedAsym==0) sprintf(tempStr, "DataCompiled_T");

    TCanvas *c2 = new TCanvas(tempStr);
    c2->Divide(2,2);
    int NHistosDrawn = 0;
    for (i=0;i<4;i++) { //i = loop through parameters, normal: i<5
        c2->cd(i+1);
        if (i==3) i=4;
        NHistosDrawn = 0;
        for (int k=0;k<NumberOfDataSets;k++) { //k = loop through results from people
            if ((k==ESven) &&(DoNotDrawSven)) continue;
            if ((k==EPeter) &&(DoNotDrawPeter)) continue;
            if ((k==EPauline) &&(DoNotDrawPauline)) continue;
            if (i<4) {
                sprintf(tempStr, "Parameter_%d_%d", i, k);
            } else {
                sprintf(tempStr, "Chi2/DoF_%d", k);
            }
            TH1D *h1 = (TH1D*)FitResults.at(i)->ProjectionX(tempStr,k+1,k+1);
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

    //PlotDataAtFixedAngle();

    if (TaggerChToLookat>=0) ProcessTaggCh(TaggerChToLookat, -1, fSelectedAsym, 1);

    theApp->SetPrompt("NewCompare %d: ");
    theApp->Run();

    return 0;
}
