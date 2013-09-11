#ifndef __worker_h_INCLUDED__
#define __worker_h_INCLUDED__

#include <GeneralConstants.h>

//-----------------------------------------------------------------------------


int SigIntCalled = 0; //To catch CTRL-C


//-----------------------------------------------------------------------------

typedef struct THitElement {
    int DetectorID;
    int ElementID;
    //raw data
    std::vector<int> RawTDC; //Takes the MultiHits of a TDC
    int RawADC;
    //calibrated
    std::vector<double> Time; //in NS
    double Energy; //in MeV
    int ParticipatingClusterID; //If Part of some ClusterID, here comes its ID
} THitElement;

typedef struct TScalerRead {
    int DetectorID;
    int ElementID;
    int Value;
} TScalerRead;

typedef struct TCBCluster {
    double Time; //in NS
    double Energy; //in MeV
    int IsCharged; //if set to != 0, then its charged
    int CentralElementID; //ElementID des Clusters mit dem höchsten Energiewert
    int NumberOfCrystals; //Number of Crystals in Cluster above 0 MeV
    //Position
    TPosition Position; //+x pointing to MAMI C, +z gamma downstream
} TCBCluster;

typedef struct TEvent {
    int EventID;
    int HelicityBit;
    int ReferenceRawTDCTagger; //TDC ch 1400
    int ReferenceRawTDCCB;     //TDC ch 2000
    std::vector<THitElement> HitElements;
    std::vector<TCBCluster> CBClusters; //for Clusters from CB
} TEvent;

typedef struct TEventBlock { //Should include one scaler read and several Events
    int ErrorDuringReading; //if 0, then everything went fine, data can be used
    int ScalerErrors; //if 0, everything is fine
    int NumberErrorBlocks;
    int ExperimentLiveCounter;
    int UngatedLiveCounter;
    int TaggerLiveCounter;
    std::vector<TScalerRead> Scalers;
    std::vector<TEvent> Events;
} TEventBlock;

//--------------------------------------------------------------------------------------

TEvent TempEvent; //Here the ADC info from a complete event is saved
int Clear_TempEvent() {
    TempEvent.EventID = -1;
    TempEvent.HelicityBit = -1;
    TempEvent.ReferenceRawTDCTagger = NoValuePresent;
    TempEvent.ReferenceRawTDCCB = NoValuePresent;
    TempEvent.HitElements.clear();
}

TEventBlock EventBlock; //Here the ADC and scaler info since last scaler read gets saved
int Clear_EventBlock() {
    EventBlock.Events.clear();
    EventBlock.Scalers.clear();
    EventBlock.ErrorDuringReading = 0;
    EventBlock.ScalerErrors = 0;
    EventBlock.NumberErrorBlocks = 0;
    EventBlock.ExperimentLiveCounter = 0;
    EventBlock.UngatedLiveCounter = 0;
    EventBlock.TaggerLiveCounter = 0;

    Clear_TempEvent();
}

//-----------------------------------------------------------------------------

int IndexRunMetaInfomation = -1; //This variable is set after loading the raw data file to point to the
                                 //actual entry in RunsMetaInformation
typedef struct TRunsMetaInformation {
    int RunNumber;
    std::string FileName;
    int RunType; //0 = normal, good, Production Run
    int TaggEffCorrespondingRun;
    int TaggEffPartNo;
    int BeamTimeBlock;
    float RunDateTime;
    int TargetNMRSignal;
    int TargetOrient;
    int HelBitPresent;
    int AcquCrash;
    double LaddP2Ratio;
    double TargetPolDegree;
    double ErrorTargetPolDegree;
    double BeamPolDegree;
    double AbsTaggEff;
    std::string DataPath;
} TRunsMetaInformation;
std::vector<TRunsMetaInformation> RunsMetaInformation;
int NRunsMetaInformation = 0;


//-----------------------------------------------------------------------------
// Histograms
//-----------------------------------------------------------------------------
TH1D *hADCOverview;
TH2D *hErrorBlocks;
//Scaler
TH1D *hAllScalerAccum, *hTaggerScalerAccum, *hTriggerScalerAccum;
TH1D *hTaggerScalerAccumLTCorrected;
TH1D *hLiveTimeAccum;
TH1D *hTaggerScalerAccumPhotonsLTCorrected, *hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff;

//ADCs TDCs
TH2D *hCBADCHits_VS_EventID, *hCBTDCHits_VS_EventID, *hCBHits_VS_EventID;
TH2D *hCBDeletedHits_VS_EventID_DueEnergy, *hCBDeletedHits_VS_EventID_DueTime;
TH2D *hCommonCounter_VS_EventID;

TH2D *hTaggerChTDC, *hCBChTDC, *hPIDChTDC; //Raw TDC information is put here
TH2D *hTaggerTime, *hCBTime, *hPIDTime; //Calibrated TDC information goes here

TH2D *hTaggerNMultiHits, *hCBNMultiHits, *hPIDNMultiHits; //Number of Multihits per Event before any cut
TH2D *hTaggerNMultiHitsCuts, *hCBNMultiHitsCuts, *hPIDNMultiHitsCuts; //Number of Multihits per Event after time cuts and calibration

TH2D *hCBChADC, *hPIDChADC; //Raw ADC information is put here
TH2D *hPIDChADCCutTDCRequired; //RawADC information, but only, if TDC hit was present
TH2D *hCBChEnergy, *hPIDChEnergy; //Calibrated ADC information is put here
TH2D *hPIDChEnergyUncut; //All raw ADC converted into Energy, no cuts at all

TH2D *PIDCorrelation;
TH2D *hCB_DeltaPhi; //Difference between all hits (clusters) in CB and PID

TH1D *hTaggerNHits, *hCBNHits, *hPIDNHits;
TH1D *CBNCluster;
TH2D *CBClusterEnergy_VS_CBNElemCluster; //Number of participation elements in a cluster vs ClusterEnergy

//Physics histograms
TH1D *hMesonInvariantMass, *hTwoParticlesInvariantMass;
TH1D *hMesonInvariantMassCorrected;
TH1D *hNPhotons;
TH1D *hCBEnergySum;
TH2D *hCBEnergySum_VS_EventID;
TH1D *hCB2GammaDeltaTime;
TH2D *hMesonPhi_VS_EventID, *hMesonThetaLab_VS_EventID;

//Histograms for Physics Analysis
/*std::vector<TH2D*> hMissingMassPrompt;
std::vector<TH2D*> hMissingMassBg;
std::vector<TH2D*> hMissingMassSignal;
*/
TH2D *hMissingMassCombinedPrompt, *hMissingMassCombinedBg, *hMissingMassCombinedSignal;
TH2D *hMissingMassCombinedPromptP, *hMissingMassCombinedBgP, *hMissingMassCombinedSignalP;
TH2D *hMissingMassCombinedPromptM, *hMissingMassCombinedBgM, *hMissingMassCombinedSignalM;
TH2D *hMissingMassCombinedSignalLTCorrected;
TH2D *hMissingMassCombinedSignalLTCorrectedP;
TH2D *hMissingMassCombinedSignalLTCorrectedM;
TH1D *hCountNumberOfHistos;

TH1D *hBeamPol, *hTargetPol, *hTaggEffAbs, *hTaggEffAbsAllMesons;

//-----------------------------------------------------------------------------
// Root files
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

int InitCalibHistograms() {
    gROOT->mkdir("RawDataDetails");
    gROOT->cd("RawDataDetails");
    //Overview: All Scalers and ADCs
    hADCOverview = new TH1D("hADCOverview","hADCOverview",10000,0,66000);
    hAllScalerAccum = new TH1D("AllScalerAccum", "AllScalerAccum", 1000, 0, 1000);
    //Error checks
    hErrorBlocks = new TH2D("hErrorBlocks", "hErrorBlocks", 2000, 0, 2000000, 1000, 0, 10000);

    gDirectory->mkdir("EventID");
    gDirectory->cd("EventID");
    //ErrorChecking for CBADCs
    hCBADCHits_VS_EventID = new TH2D("CBADCHits_VS_EventID", "CBADCHits_VS_EventID", (2000000./1000.), 0, 2000000, NCBElements, 0, NCBElements);
    hCBTDCHits_VS_EventID = new TH2D("CBTDCHits_VS_EventID", "CBTDCHits_VS_EventID", (2000000./1000.), 0, 2000000, NCBElements, 0, NCBElements);
    hCBHits_VS_EventID = new TH2D("CBHits_VS_EventID", "CBHits_VS_EventID", (2000000./1000.), 0, 2000000, NCBElements, 0, NCBElements);
    hCBDeletedHits_VS_EventID_DueEnergy = new TH2D("hCBDeletedHits_VS_EventID_DueEnergy", "hCBDeletedHits_VS_EventID_DueEnergy", 2000, 0, 2000000, NCBElements, 0, NCBElements);
    hCBDeletedHits_VS_EventID_DueTime = new TH2D("hCBDeletedHits_VS_EventID_DueTime", "hCBDeletedHits_VS_EventID_DueTime", 2000, 0, 2000000, NCBElements, 0, NCBElements);
    hCommonCounter_VS_EventID = new TH2D("hCommonCounter", "hCommonCounter", 2000, 0, 2000000, 100, -2, 2.);

    gROOT->cd("RawDataDetails");
    gDirectory->mkdir("TDC");
    gDirectory->cd("TDC");
    //Uncalibrated TDC
    hTaggerChTDC = new TH2D("hTaggerChTDC", "hTaggerChTDC", 200, -10000, 10000, NTaggerElements, 0, NTaggerElements);
    hCBChTDC = new TH2D("hCBChTDC", "hCBChTDC", 200, -10000, 10000, NCBElements, 0, NCBElements);
    hPIDChTDC = new TH2D("hPIDChTDC", "hPIDChTDC", 200, -10000, 10000, NPIDElements, 0, NPIDElements);

    gROOT->mkdir("Calibrated");
    gROOT->cd("Calibrated");
    gDirectory->mkdir("Trigger");
    gDirectory->cd("Trigger");
    hTriggerScalerAccum = new TH1D("TriggerScalerAccum", "TriggerScalerAccum", 256, 0, 256); hTriggerScalerAccum->Sumw2();
    hLiveTimeAccum = new TH1D("LiveTimeAccum", "LiveTimeAccum", 3, 0, 3); hLiveTimeAccum->Sumw2(); //Bin 1: Ungated, Bin 2: CB Gated, Bin3: Tagger gated

    gROOT->cd("Calibrated");
    gDirectory->mkdir("Tagger");
    gDirectory->cd("Tagger");
    hTaggerScalerAccum = new TH1D("TaggerScalerAccum", "TaggerScalerAccum", NTaggerElements, 0, NTaggerElements); hTaggerScalerAccum->Sumw2(); //Tagger und Trigger Scaler
    hTaggerTime = new TH2D("hTaggerTime", "hTaggerTime", 2000, -1000, 1000, NTaggerElements, 0, NTaggerElements);//Calibrated Time
    hTaggerNMultiHits = new TH2D("hTaggerNMultiHits", "hTaggerNMultiHits", 10,0,10, NTaggerElements, 0, NTaggerElements); //Number of Multihits per Event | Without cuts, raw signal
    hTaggerNMultiHitsCuts = new TH2D("hTaggerNMultiHitsCuts", "hTaggerNMultiHitsCuts", 10,0,10, NTaggerElements, 0, NTaggerElements); //Number of Multihits per Event | After calibration and time cuts
    hTaggerNHits = new TH1D("TaggerNHits", "TaggerNHits", 50, 0, 50);//NHits spectra

    gROOT->cd("Calibrated");
    gDirectory->mkdir("CB");
    gDirectory->cd("CB");
    hCBTime = new TH2D("hCBTime", "hCBTime", 2000, -1000, 1000, NCBElements, 0, NCBElements);  //Calibrated Time
    hCBChADC = new TH2D("hCBChADC", "hCBChADC", 1000, -100, 10000, NCBElements, 0, NCBElements);//Raw ADC information
    hCBChEnergy = new TH2D("hCBChEnergy", "hCBChEnergy", 2000, -10, 1000, NCBElements, 0, NCBElements); //Calibrated ADC information
    hCBNMultiHits = new TH2D("hCBNMultiHits", "hCBNMultiHits", 10,0,10, NCBElements, 0, NCBElements); //Number of Multihits per Event | Without cuts, raw signal
    hCBNMultiHitsCuts = new TH2D("hCBNMultiHitsCuts", "hCBNMultiHitsCuts", 10,0,10, NCBElements, 0, NCBElements);  //Number of Multihits per Event | After calibration and time cuts
    hCBNHits = new TH1D("CBNHits", "CBNHits", 50, 0, 50); //NHits spectra
    gDirectory->mkdir("Cluster");
    gDirectory->cd("Cluster");
    CBNCluster = new TH1D("CBNCluster", "CBNCluster", 25, 0, 25);
    CBClusterEnergy_VS_CBNElemCluster = new TH2D("CBClusterEnergy_VS_CBNElemCluster", "CBClusterEnergy_VS_CBNElemCluster", 100, 0, 300, 15, 0, 15);

    gROOT->cd("Calibrated");
    gDirectory->mkdir("PID");
    gDirectory->cd("PID");
    hPIDTime = new TH2D("hPIDTime", "hPIDTime", 2000, -1000, 1000, NPIDElements, 0, NPIDElements);   //Calibrated Time
    hPIDChADC = new TH2D("hPIDChADC", "hPIDChADC", 1000, 0, 10000, NPIDElements, 0, NPIDElements); //Raw ADC information
    hPIDChADCCutTDCRequired = new TH2D("hPIDChADCCutTDCRequired", "hPIDChADCCutTDCRequired", 1000, 0, 10000, NPIDElements, 0, NPIDElements); //Raw ADC information
    hPIDChEnergy = new TH2D("hPIDChEnergy", "hPIDChEnergy", 1000, -10, 190, NPIDElements, 0, NPIDElements); //Calibrated ADC information
    hPIDChEnergyUncut = new TH2D("hPIDChEnergyUncut", "hPIDChEnergyUncut", 1000, -10, 190, NPIDElements, 0, NPIDElements); //Calibrated ADC information
    hPIDNMultiHits = new TH2D("hPIDNMultiHits", "hPIDNMultiHits", 10,0,10, NPIDElements, 0, NPIDElements); //Number of Multihits per Event | Without cuts, raw signal
    hPIDNMultiHitsCuts = new TH2D("hPIDNMultiHitsCuts", "hPIDNMultiHitsCuts", 10,0,10, NPIDElements, 0, NPIDElements);  //Number of Multihits per Event | After calibration and time cuts
    hPIDNHits = new TH1D("PIDNHits", "PIDNHits", 50, 0, 50); //NHits spectra
    //PID
    gDirectory->mkdir("PID-CB correclation");
    gDirectory->cd("PID-CB correclation");
    PIDCorrelation = new TH2D("PIDCorrelation", "PIDCorrelation", 45,-180, 180, 48, -180, 180); //x=CB, y=PID
    hCB_DeltaPhi = new TH2D("hCB_DeltaPhi", "hCB_DeltaPhi", 45, -180, 180, 24, 0, 24); hCB_DeltaPhi->Sumw2();

    gROOT->cd();
    gROOT->mkdir("Physics");
    gROOT->cd("Physics");
    //Physics histograms
    hMesonInvariantMass = new TH1D("MesonInvariantMass", "MesonInvariantMass", 250, 0, 250); hMesonInvariantMass->Sumw2();
    hMesonInvariantMassCorrected = new TH1D("MesonInvariantMassCorrected", "MesonInvariantMassCorrected", 250, 0, 250); hMesonInvariantMassCorrected->Sumw2();
    hTwoParticlesInvariantMass = new TH1D("TwoParticlesInvariantMass", "TwoParticlesInvariantMass", 250,0,250); hTwoParticlesInvariantMass->Sumw2();
    hNPhotons = new TH1D("NPhotons", "NPhotons", 20, 0, 20);
    hCBEnergySum = new TH1D("CBEnergySum", "CBEnergySum", 250, 0, 500); hCBEnergySum->Sumw2();
    hCBEnergySum_VS_EventID = new TH2D("hCBEnergySum_VS_EventID", "hCBEnergySum_VS_EventID", 2000, 0, 2000000, 250, 0, 500);
    hCB2GammaDeltaTime = new TH1D("hCB2GammaDeltaTime", "hCB2GammaDeltaTime", 100, -50, 50);

    hMesonPhi_VS_EventID = new TH2D("MesonPhi_VS_EventID", "MesonPhi_VS_EventID", (2000000./1000.), 0, 2000000, 45, -180, 180);
    hMesonThetaLab_VS_EventID = new TH2D("MesonThetaLab_VS_EventID", "MesonThetaLab_VS_EventID", (2000000./1000.), 0, 2000000, 45, 0, 180);


    //Physics analysis
    hMissingMassCombinedPrompt = new TH2D("MissingMassCombinedPrompt", "MissingMassCombinedPrompt", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPrompt->Sumw2();
    hMissingMassCombinedBg = new TH2D("MissingMassCombinedBg", "MissingMassCombinedBg", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBg->Sumw2();
    hMissingMassCombinedSignal = new TH2D("MissingMassCombinedSignal", "MissingMassCombinedSignal", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignal->Sumw2();

    hMissingMassCombinedPromptP =   new TH2D("MissingMassCombinedPromptP",  "MissingMassCombinedPromptP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptP->Sumw2();
    hMissingMassCombinedBgP =       new TH2D("MissingMassCombinedBgP",      "MissingMassCombinedBgP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgP->Sumw2();
    hMissingMassCombinedSignalP =   new TH2D("MissingMassCombinedSignalP",  "MissingMassCombinedSignalP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalP->Sumw2();

    hMissingMassCombinedPromptM =   new TH2D("MissingMassCombinedPromptM",  "MissingMassCombinedPromptM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptM->Sumw2();
    hMissingMassCombinedBgM =       new TH2D("MissingMassCombinedBgM",      "MissingMassCombinedBgM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgM->Sumw2();
    hMissingMassCombinedSignalM =   new TH2D("MissingMassCombinedSignalM",  "MissingMassCombinedSignalM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalM->Sumw2();

    gROOT->cd();
    gROOT->mkdir("BeamTarget");
    gROOT->cd("BeamTarget");
    hBeamPol = new TH1D("BeamPol", "BeamPol", 1000, 0, 1); hBeamPol->Sumw2();
    hTargetPol = new TH1D("TargetPol", "TargetPol", 1000, 0, 1); hTargetPol->Sumw2();
    hTaggEffAbs = new TH1D("TaggEffAbs", "TaggEffAbs", 1000, 0, 1); hTaggEffAbs->Sumw2();
    hTaggEffAbsAllMesons = new TH1D("TaggEffAbsAllMesons", "TaggEffAbsAllMesons", 1000, 0, 1); hTaggEffAbsAllMesons->Sumw2();

    /*hMissingMassPrompt.resize(NTaggerElements);
    hMissingMassBg.resize(NTaggerElements);
    hMissingMassSignal.resize(NTaggerElements);
    for (int i=0;i<NTaggerElements;i++) {
        Char_t TempStr[256];
        sprintf(TempStr, "hMissingMassPrompt_%d", i);
        hMissingMassPrompt.at(i) = new TH2D(TempStr, TempStr, 100,800,1000,  18, 0, 180);
        hMissingMassPrompt.at(i)->Sumw2();
        sprintf(TempStr, "hMissingMassBg_%d", i);
        hMissingMassBg.at(i) = new TH2D(TempStr, TempStr, 100,800,1000,  18, 0, 180);
        hMissingMassBg.at(i)->Sumw2();
        sprintf(TempStr, "hMissingMassSignal_%d", i);
        hMissingMassSignal.at(i) = new TH2D(TempStr, TempStr, 100,800,1000, 18, 0, 180);
        hMissingMassSignal.at(i)->Sumw2();
    }
    */

    //Histogram counter for later hadd
    gROOT->cd();
    hCountNumberOfHistos = new TH1D("CountNumberOfHistos", "CountNumberOfHistos", 1,1,2);
    hCountNumberOfHistos->Fill(1);

    Printf("INFO: All Histograms created.");
}




#endif
