#include <GeneralConstants.h>


//This union is used to support reading the file
union StoredInfo { //Used for the 32bit structure in the file, used for reading file data
  struct {unsigned short Ch, Value; } AsADC;
  //int AsInt;
  unsigned int AsInt;
};


//--------------------------------------------------------------------------------------------

typedef struct {
    double RiseTime;
    double Threshold;
    double Shift;
    double Power;
} TTimeWalkParameter;

typedef struct {
    int TDCCh;
    int ADCCh;
    int ScalerCh;
    int TDCMultiHit; //0 == false, if true, it's treated as several particles
    int ADCMultiHit; //0 == false, if true, it's treated as one hit with base, signal and tail information

    //TDC Calibration
    double TDCLowThresholdNS;
    double TDCHighThresholdNS;
    double TDCOffsetTicks;
    double TDCNSPerTick;
    TTimeWalkParameter TimeWalkParameter;

    //ADC calibration
    double ADCLowThresholdMEV;
    double ADCHighThresholdMEV;
    double ADCPedestalTicks;
    double ADCMEVPerTick;

    //For Tagger
    double CentralEnergyMEV;

    //Position
    TPosition Position; //+x pointing to MAMI C, +z gamma downstream
} TDetectorElement;



typedef struct {
    double TimeOffsetNS; //in ns
    std::vector<TDetectorElement> Elements; //Event ADC Daten werden hier herein gespeichert
} TDetector;

struct TRawADCData {
    int ExperimentLiveCounterScalerCh;
    int UngatedLiveCounterScalerCh;
    int TaggerLiveCounterScalerCh;
    TDetector Tagger;    //Detector ID = 0
    TDetector CB;        //Detector ID = 1
    TDetector PID;       //Detector ID = 2
    TDetector Trigger;   //Detector ID = 10
} RawADCData;


//-----------------------------------------------------------------------------

//Lookup table for ADC ch to detector element
typedef struct {
    int DetectorID; //-1 = undef., 0 = Tagger, 1 = CB, 2 = PID, 10 = Trigger
    int ElementID; //from 0 to ...
    int TypeID; //0 = ADC, 1= TDC, Pattern?
} TLookupTableADC;
std::vector<TLookupTableADC> LookupTableADC;

//Lookup table for Scaler ch to detector element
typedef struct {
    int DetectorID; //-1 = undef., 0 = Tagger, 1 = CB, 2 = PID, 10 = Trigger
    int ElementID; //from 0 to ...
} TLookupTableScaler;
std::vector<TLookupTableScaler> LookupTableScaler;

//-----------------------------------------------------------------------------


int InitializeLookupTables() {
    RawADCData.ExperimentLiveCounterScalerCh = -1;
    RawADCData.UngatedLiveCounterScalerCh = -1;
    RawADCData.TaggerLiveCounterScalerCh = -1;

    //Create Experiment Detectors
    LookupTableADC.resize(0xffff);
    for (int tempI = 0; tempI<LookupTableADC.size(); tempI++) {
        LookupTableADC.at(tempI).DetectorID = -1;
        LookupTableADC.at(tempI).ElementID = -1;
        LookupTableADC.at(tempI).TypeID = -1;
    }
    LookupTableScaler.resize(0xffff);
    for (int tempI = 0; tempI<LookupTableScaler.size(); tempI++) {
        LookupTableScaler.at(tempI).DetectorID = -1;
        LookupTableScaler.at(tempI).ElementID = -1;
    }

    /*    Printf("\nConfiguration Lookup of ADC ch");
        for (int tempI = 0; tempI<LookupTableScaler.size(); tempI++) {
            Printf("%d %d %d", tempI, LookupTableScaler.at(tempI).DetectorID, LookupTableScaler.at(tempI).ElementID);
        }
        Printf("End of output: Configuration Lookup of ADC ch\n");
      */


}
