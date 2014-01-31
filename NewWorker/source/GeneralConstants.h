#ifndef __GeneralConstants_H_INCLUDED__
#define __GeneralConstants_H_INCLUDED__

#include <TLorentzVector.h>

const int ADCTypeIDADC = 0;
const int ADCTypeIDTDC = 1;
const int ADCTypeIDPattern = 2; //Not yet used
const int ADCTypeIDMoellerHisto = 3; //Not yet used

const int DetectorIDTagger   = 0;      //Detector ID = 0
const int DetectorIDCB       = 1;      //Detector ID = 1
const int DetectorIDPID      = 2;      //Detector ID = 2
const int DetectorIDTrigger  = 10;     //Detector ID = 10


const int NTaggerElements = 352;
const int NCBElements = 720;
const int NPIDElements = 24;
int NTriggerElements = -1; //Needs to be defined in Trigger Config file


const double MassPion0 = 134.9766;
const double MassProton = 938.272046;

Char_t OutputDirectory[256] = ""; //With / at the end
Char_t InputPathRawData[256] = ""; //With / at the end
//Char_t FilePathRunInformation[1024] = "/home/otte/NewAnalysis/data/RunsMetaInformation.dat"; //Path and Filename to File with RunInfo: for all files to analyse
Char_t InputPathDataDir[256] = "/home/otte/NewAnalysis/data/"; //With / at the end
int ScalerBlockLength1 = -1; //For Mk1 DataFormat
int ScalerBlockLength2 = -1; //For Mk1 DataFormat
int RawDataFormat = 0; //1 for Mk1 and 2 for Mk2 Format
Char_t TaggEffNormFile[1024] = ""; //Path plus Filename where to find the TaggEffNorm.root file

//-----------------------------------------------------------------------------

//Setting the CutHitsOutOfRange to 0 needs some more modifications to the code and is at the moment not yet tested.
int CutHitsOutOfRange = -1; //If set to 0, no hits will be deleted. if set to -1, hits which do not mach the ranges, defined by config file, will be deleted.

//Correct for Missing Hits in CB
//if ADC does not match TDC information then skip the complete Block
int RequireMinHitsCBBlock = 0; //default will not drop any events

//For analysis of F
int RequireBeamHelicityPresent = -1; //default: requires BeamHelicity information present


//PID Constants
double PIDOffset = 0; //Phi angle to add to PID phi after loading
double PIDCBPhiDelta = 0.; //Correlation between CB Cluster and PID Element in Phi, sigma of Delta_CB_PID plot is 7.2 degree

//----------------------------------------------------------------------------
// CB Constants
double GlobalCBEnergyScale = 1.;
//CB Cluster Algo Constants
double ClusterMinEnergy = 0.; //in MeV

double ElectronBeamEnergy = 0.; //in MeV


int ProgramBatchMode = 0; //if 0, it runs interactively, else, it runs in batch mode
int ProgramOnlyFileCheckMode = 0; //if != 0 it only read and cheks the file
int ProgramSaveSpectraMode = 0; //if != 0 spectra will be save to root file at the end of analysis
int ProgramNoTBrowserMode = 0; //

int RunNumberFromDataFile = -1; //Is read from file header at byte 0x2a8 (4 bytes)

//-----------------------------------------------------------------------------

TLorentzVector LVTarget(0., 0., 0., MassProton); //Position and Energy


//-----------------------------------------------------------------------------

//If a TDC or ADC value is not present, than this value is assigned.
//Should be assigned to each TDC/ADC variable after initialisation
const int NoValuePresent = -1E9;

//To check, whether the number of scalers is constant with time
int LastScalerReadNCh = -1;

//if set to -1, then the actual EventBlock will not be analysed after Do_ConstructDetectorHits()
int DiscardActualEventBlockDueToCBHits = 0;

//------------------------------------------------------------------------------

typedef struct {
    double r; //in cm
    double theta, phi; //in rad
} TPosition;





#endif //__GeneralConstants_H_INCLUDED__
