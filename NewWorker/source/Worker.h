#ifndef __worker_h_INCLUDED__
#define __worker_h_INCLUDED__

#include <GeneralConstants.h>

//-----------------------------------------------------------------------------
//#d efine DO_hMissingMassPrompt 1
//#d efine DO_CBADC3Sums 1
// Do ADC_Overview, Scaler_Overview, ErrorBlocks
//#d efine DO_ExtendedRawDataChecks 1



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
    int NErrorBlocks;          //Number of Errorblocks
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
    TempEvent.NErrorBlocks = 0;
    TempEvent.HelicityBit = -1;
    TempEvent.ReferenceRawTDCTagger = NoValuePresent;
    TempEvent.ReferenceRawTDCCB = NoValuePresent;
    TempEvent.HitElements.clear();
    TempEvent.CBClusters.clear();
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
    double CBEnergyScale;
} TRunsMetaInformation;
std::vector<TRunsMetaInformation> RunsMetaInformation;
int NRunsMetaInformation = 0;


//-----------------------------------------------------------------------------
// Histograms
//-----------------------------------------------------------------------------
#ifdef DO_ExtendedRawDataChecks
    TH1D *hADCOverview;
    TH2D *hErrorBlocks;

    //Scaler
    TH1D *hAllScalerAccum;
#endif
//Scaler
TH1D *hTaggerScalerAccum, *hTriggerScalerAccum;
TH1D *hTaggerScalerAccumLTCorrectedAllEvents, *hTaggerScalerAccumLTCorrected; //With Dropped Events and without
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


#ifdef DO_CBADC3Sums
    TH2D *hCBChADCPart1, *hCBChADCPart2, *hCBChADCPart3; //Raw CB ADC values, pedestal, signal, tail. For debug of ADC delay setting
#endif
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
TH1D *hTimeMesonTagger;
TH1D *hNPhotons;
TH1D *hCBEnergySum, *hCBEnergySumAllEvents; //W/O and with Dropped Events
TH1D *hCBEnergySumTP, *hCBEnergySumTM, *hCBEnergySumFP, *hCBEnergySumFM; //CB EnergySum for T and F for + and -
TH2D *hCBEnergySum_VS_EventID;
TH1D *hCB2GammaDeltaTime;
TH2D *hMesonPhi_VS_EventID, *hMesonThetaLab_VS_EventID;

//Histograms for Physics Analysis
#ifdef DO_hMissingMassPrompt
    std::vector<TH2D*> hMissingMassPrompt;
    std::vector<TH2D*> hMissingMassBg;
    std::vector<TH2D*> hMissingMassSignal;
#endif
TH2D *hMissingMassCombinedPrompt, *hMissingMassCombinedBg, *hMissingMassCombinedSignal; //TaggCh VS Meson-Theta
TH2D *hMissingMassVsMesonThetaPrompt, *hMissingMassVsMesonThetaBg, *hMissingMassVsMesonThetaSignal; //MissingMass (= Proton) VS Meson-Theta
TH2D *hInvariant2GammaMassVsThetaPrompt, *hInvariant2GammaMassVsThetaPromptKinFit; //Meson Mass vs Theta

TH1D *hKinFitPulls;

TH2D *hMissingMassCombinedSignalLTCorrected;

//F
TH2D *hMissingMassCombinedPromptFP, *hMissingMassCombinedBgFP, *hMissingMassCombinedSignalFP;
TH2D *hMissingMassCombinedPromptFM, *hMissingMassCombinedBgFM, *hMissingMassCombinedSignalFM;
TH2D *hMissingMassCombinedSignalLTCorrectedFP, *hMissingMassCombinedSignalLTCorrectedFM;
TH1D *hDroppedEvents; //Counter, how many events have been skipped because of wrong MAMI Source Bit Pattern etc.

//T
TH2D *hMissingMassCombinedPromptTP, *hMissingMassCombinedBgTP, *hMissingMassCombinedSignalTP;
TH2D *hMissingMassCombinedPromptTM, *hMissingMassCombinedBgTM, *hMissingMassCombinedSignalTM;
TH2D *hMissingMassCombinedSignalLTCorrectedTP, *hMissingMassCombinedSignalLTCorrectedTM;

TH1D *hCountNumberOfHistos;

TH2D *hBeamPol, *hTaggEffAbsT, *hTaggEffAbsF, *hTaggEffAbsAll;
TH2D *hTargetPolF, *hTargetPolT;

//-----------------------------------------------------------------------------
// Root files
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

int InitCalibHistograms() {
    gROOT->mkdir("RawDataDetails");
    gROOT->cd("RawDataDetails");
    #ifdef DO_ExtendedRawDataChecks
        //Overview: All Scalers and ADCs
        hADCOverview = new TH1D("hADCOverview","hADCOverview",66000,0,66000);
        hAllScalerAccum = new TH1D("AllScalerAccum", "AllScalerAccum", 100000, 0, 100000);
        //Error checks
        hErrorBlocks = new TH2D("hErrorBlocks", "hErrorBlocks",   2000, 0, 2000000, 1000, 0, 1000); //Good range for Sep 2010
    //    hErrorBlocks = new TH2D("hErrorBlocks", "hErrorBlocks", 200, 0, 200000, 20000, 30000, 50000); //Good range for May 2013
    #endif
    hDroppedEvents = new TH1D("hDroppedEvents", "hDroppedEvents", 2, 0, 2);

    gDirectory->mkdir("EventID");
    gDirectory->cd("EventID");
    //ErrorChecking for CBADCs
    hCBADCHits_VS_EventID = new TH2D("CBADCHits_VS_EventID", "CBADCHits_VS_EventID", (3000000./1000.), 0, 3000000, NCBElements, 0, NCBElements);
//    hCBADCHits_VS_EventID = new TH2D("CBADCHits_VS_EventID", "CBADCHits_VS_EventID", 1000, 0, 1000, NCBElements, 0, NCBElements);
    hCBTDCHits_VS_EventID = new TH2D("CBTDCHits_VS_EventID", "CBTDCHits_VS_EventID", (3000000./1000.), 0, 3000000, NCBElements, 0, NCBElements);
    hCBHits_VS_EventID = new TH2D("CBHits_VS_EventID", "CBHits_VS_EventID", (3000000./1000.), 0, 3000000, NCBElements, 0, NCBElements);
    hCBDeletedHits_VS_EventID_DueEnergy = new TH2D("hCBDeletedHits_VS_EventID_DueEnergy", "hCBDeletedHits_VS_EventID_DueEnergy", 2000, 0, 2000000, NCBElements, 0, NCBElements);
    hCBDeletedHits_VS_EventID_DueTime = new TH2D("hCBDeletedHits_VS_EventID_DueTime", "hCBDeletedHits_VS_EventID_DueTime", 2000, 0, 2000000, NCBElements, 0, NCBElements);
    hCommonCounter_VS_EventID = new TH2D("hCommonCounter", "hCommonCounter", 2000, 0, 2000000, 100, -2, 2.);
    //ErrorChecking for BaF


    gROOT->cd("RawDataDetails");
    gDirectory->mkdir("ADC");
    gDirectory->cd("ADC");
    //Uncalibrated CB ADC, their single sums
    #ifdef DO_CBADC3Sums
        hCBChADCPart1 = new TH2D("hCBChADCPart1", "hCBChADCPart1", 1000, -100, 66000, NCBElements, 0, NCBElements);//Raw ADC information, 1st sum, pedestal
        hCBChADCPart2 = new TH2D("hCBChADCPart2", "hCBChADCPart2", 1000, -100, 66000, NCBElements, 0, NCBElements);//Raw ADC information, 2nd sum, signal
        hCBChADCPart3 = new TH2D("hCBChADCPart3", "hCBChADCPart3", 1000, -100, 66000, NCBElements, 0, NCBElements);//Raw ADC information, 3rd sum, tail
    #endif

    gROOT->mkdir("Calibration");
    gROOT->cd("Calibration");
    gDirectory->mkdir("Trigger");
    gDirectory->cd("Trigger");
    hTriggerScalerAccum = new TH1D("TriggerScalerAccum", "TriggerScalerAccum", 256, 0, 256); //Do not call SumW2 here, but after filling. Otherwise wrong errors
    hLiveTimeAccum = new TH1D("LiveTimeAccum", "LiveTimeAccum", 4, 0, 4);  //Bin 1: Ungated, Bin 2: CB Gated, Bin3: Tagger gated, Bin4: Ungated(not corrected by Dropped Events) //Do not call SumW2 here,

    gROOT->cd("Calibration");
    gDirectory->mkdir("Tagger");
    gDirectory->cd("Tagger");
    hTaggerChTDC = new TH2D("hTaggerChTDC", "hTaggerChTDC", 200, -10000, 10000, NTaggerElements, 0, NTaggerElements); //Uncalibrated TDC
    hTaggerScalerAccum = new TH1D("TaggerScalerAccum", "TaggerScalerAccum", NTaggerElements, 0, NTaggerElements); //Tagger und Trigger Scaler //Do not call SumW2 here,
    hTaggerTime = new TH2D("hTaggerTime", "hTaggerTime", 2000, -1000, 1000, NTaggerElements, 0, NTaggerElements);//Calibrated Time
    hTaggerNMultiHits = new TH2D("hTaggerNMultiHits", "hTaggerNMultiHits", 10,0,10, NTaggerElements, 0, NTaggerElements); //Number of Multihits per Event | Without cuts, raw signal
    hTaggerNMultiHitsCuts = new TH2D("hTaggerNMultiHitsCuts", "hTaggerNMultiHitsCuts", 10,0,10, NTaggerElements, 0, NTaggerElements); //Number of Multihits per Event | After calibration and time cuts
    hTaggerNHits = new TH1D("TaggerNHits", "TaggerNHits", 100, 0, 100);//NHits spectra

    gROOT->cd("Calibration");
    gDirectory->mkdir("CB");
    gDirectory->cd("CB");
    hCBChTDC = new TH2D("hCBChTDC", "hCBChTDC", 200, -10000, 10000, NCBElements, 0, NCBElements); //Uncalibrated TDC
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

    gROOT->cd("Calibration");
    gDirectory->mkdir("PID");
    gDirectory->cd("PID");
    hPIDChTDC = new TH2D("hPIDChTDC", "hPIDChTDC", 200, -10000, 10000, NPIDElements, 0, NPIDElements); //Uncalibrated TDC
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
    hTimeMesonTagger = new TH1D("hTimeMesonTagger", "hTimeMesonTagger", 400, -200, 200);
    hNPhotons = new TH1D("NPhotons", "NPhotons", 20, 0, 20);
    hCBEnergySum = new TH1D("CBEnergySum", "CBEnergySum", 250, 0, 500); hCBEnergySum->Sumw2();
    hCBEnergySumTP = new TH1D("CBEnergySumTP", "CBEnergySumTP", 250, 0, 500); hCBEnergySumTP->Sumw2();
    hCBEnergySumTM = new TH1D("CBEnergySumTM", "CBEnergySumTM", 250, 0, 500); hCBEnergySumTM->Sumw2();
    hCBEnergySumFP = new TH1D("CBEnergySumFP", "CBEnergySumFP", 250, 0, 500); hCBEnergySumFP->Sumw2();
    hCBEnergySumFM = new TH1D("CBEnergySumFM", "CBEnergySumFM", 250, 0, 500); hCBEnergySumFM->Sumw2();
    hCBEnergySumAllEvents = new TH1D("CBEnergySumAllEvents", "CBEnergySumAllEvents", 250, 0, 500); hCBEnergySumAllEvents->Sumw2();
    hCBEnergySum_VS_EventID = new TH2D("hCBEnergySum_VS_EventID", "hCBEnergySum_VS_EventID", 2000, 0, 2000000, 250, 0, 500);
    hCB2GammaDeltaTime = new TH1D("hCB2GammaDeltaTime", "hCB2GammaDeltaTime", 200, -100, 100);

    hMesonPhi_VS_EventID = new TH2D("MesonPhi_VS_EventID", "MesonPhi_VS_EventID", (2000000./1000.), 0, 2000000, 45, -180, 180);
    hMesonThetaLab_VS_EventID = new TH2D("MesonThetaLab_VS_EventID", "MesonThetaLab_VS_EventID", (2000000./1000.), 0, 2000000, 45, 0, 180);


    //Physics analysis
    hMissingMassCombinedPrompt = new TH2D("MissingMassCombinedPrompt", "MissingMassCombinedPrompt", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPrompt->Sumw2();
    hMissingMassCombinedBg = new TH2D("MissingMassCombinedBg", "MissingMassCombinedBg", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBg->Sumw2();
    hMissingMassCombinedSignal = new TH2D("MissingMassCombinedSignal", "MissingMassCombinedSignal", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignal->Sumw2();

    hMissingMassCombinedPromptFP =   new TH2D("MissingMassCombinedPromptFP",  "MissingMassCombinedPromptFP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptFP->Sumw2();
    hMissingMassCombinedBgFP =       new TH2D("MissingMassCombinedBgFP",      "MissingMassCombinedBgFP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgFP->Sumw2();
    hMissingMassCombinedSignalFP =   new TH2D("MissingMassCombinedSignalFP",  "MissingMassCombinedSignalFP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalFP->Sumw2();

    hMissingMassCombinedPromptFM =   new TH2D("MissingMassCombinedPromptFM",  "MissingMassCombinedPromptFM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptFM->Sumw2();
    hMissingMassCombinedBgFM =       new TH2D("MissingMassCombinedBgFM",      "MissingMassCombinedBgFM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgFM->Sumw2();
    hMissingMassCombinedSignalFM =   new TH2D("MissingMassCombinedSignalFM",  "MissingMassCombinedSignalFM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalFM->Sumw2();

    hMissingMassCombinedPromptTP =   new TH2D("MissingMassCombinedPromptTP",  "MissingMassCombinedPromptTP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptTP->Sumw2();
    hMissingMassCombinedBgTP =       new TH2D("MissingMassCombinedBgTP",      "MissingMassCombinedBgTP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgTP->Sumw2();
    hMissingMassCombinedSignalTP =   new TH2D("MissingMassCombinedSignalTP",  "MissingMassCombinedSignalTP", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalTP->Sumw2();

    hMissingMassCombinedPromptTM =   new TH2D("MissingMassCombinedPromptTM",  "MissingMassCombinedPromptTM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedPromptTM->Sumw2();
    hMissingMassCombinedBgTM =       new TH2D("MissingMassCombinedBgTM",      "MissingMassCombinedBgTM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedBgTM->Sumw2();
    hMissingMassCombinedSignalTM =   new TH2D("MissingMassCombinedSignalTM",  "MissingMassCombinedSignalTM", NTaggerElements, 0, NTaggerElements, 18, 0, 180); hMissingMassCombinedSignalTM->Sumw2();

    hMissingMassVsMesonThetaPrompt = new TH2D("hMissingMassVsMesonThetaPrompt", "hMissingMassVsMesonThetaPrompt",   120, 880, 1000, 36, 0, 180); hMissingMassVsMesonThetaPrompt->Sumw2();
    hMissingMassVsMesonThetaBg =     new TH2D("hMissingMassVsMesonThetaBg",     "hMissingMassVsMesonThetaBg",       120, 880, 1000, 36, 0, 180); hMissingMassVsMesonThetaBg->Sumw2();
    hMissingMassVsMesonThetaSignal = new TH2D("hMissingMassVsMesonThetaSignal", "hMissingMassVsMesonThetaSignal",   120, 880, 1000, 36, 0, 180); hMissingMassVsMesonThetaSignal->Sumw2();

    hInvariant2GammaMassVsThetaPrompt = new TH2D("hInvariant2GammaMassVsThetaPrompt", "hInvariant2GammaMassVsThetaPrompt",   200, 0, 200, 36, 0, 180); hInvariant2GammaMassVsThetaPrompt->Sumw2();
    hInvariant2GammaMassVsThetaPromptKinFit = new TH2D("hInvariant2GammaMassVsThetaPromptKinFit", "hInvariant2GammaMassVsThetaPromptKinFit",       200, 0, 200, 36, 0, 180); hInvariant2GammaMassVsThetaPromptKinFit->Sumw2();

    hKinFitPulls = new TH1D("hKinFitPulls", "hKinFitPulls", 200, -50, 50);

    #ifdef DO_hMissingMassPrompt
        hMissingMassPrompt.resize(NTaggerElements);
        hMissingMassBg.resize(NTaggerElements);
        hMissingMassSignal.resize(NTaggerElements);
        for (int i=0;i<NTaggerElements;i++) {
            Char_t TempStr[256];
            sprintf(TempStr, "hMissingMassPrompt_%d", i);
            hMissingMassPrompt.at(i) = new TH2D(TempStr, TempStr, 120,880,1000,  18, 0, 180);
            hMissingMassPrompt.at(i)->Sumw2();
            sprintf(TempStr, "hMissingMassBg_%d", i);
            hMissingMassBg.at(i) = new TH2D(TempStr, TempStr, 120,880,1000,  18, 0, 180);
            hMissingMassBg.at(i)->Sumw2();
            sprintf(TempStr, "hMissingMassSignal_%d", i);
            hMissingMassSignal.at(i) = new TH2D(TempStr, TempStr, 120,880,1000, 18, 0, 180);
            hMissingMassSignal.at(i)->Sumw2();
        }
    #endif

    gROOT->cd();
    gROOT->mkdir("BeamTarget");
    gROOT->cd("BeamTarget");
    hBeamPol    = new TH2D("BeamPol", "BeamPol", NTaggerElements, 0, NTaggerElements, 1000, 0, 1); hBeamPol->Sumw2();
    hTargetPolF = new TH2D("TargetPolF", "TargetPolF", NTaggerElements, 0, NTaggerElements, 200, 0, 1); hTargetPolF->Sumw2();
    hTargetPolT = new TH2D("TargetPolT", "TargetPolT", NTaggerElements, 0, NTaggerElements, 200, 0, 1); hTargetPolT->Sumw2();
    hTaggEffAbsT = new TH2D("TaggEffAbsT", "TaggEffAbsT", NTaggerElements, 0, NTaggerElements, 1000, 0, 1); hTaggEffAbsT->Sumw2();
    hTaggEffAbsF = new TH2D("TaggEffAbsF", "TaggEffAbsF", NTaggerElements, 0, NTaggerElements, 1000, 0, 1); hTaggEffAbsF->Sumw2();
    hTaggEffAbsAll = new TH2D("TaggEffAbsAll", "TaggEffAbsAll", NTaggerElements, 0, NTaggerElements, 1000, 0, 1); hTaggEffAbsAll->Sumw2(); //with cut on EffHel


    //Histogram counter for later hadd
    gROOT->cd();
    hCountNumberOfHistos = new TH1D("CountNumberOfHistos", "CountNumberOfHistos", 1,1,2);
    hCountNumberOfHistos->Fill(1);

    Printf("INFO: All Histograms created.");
}




#endif
