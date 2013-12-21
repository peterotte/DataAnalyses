#include <RawADCDataVars.h>
#include <TMath.h>
#include <cmath>

int ReadAnalysisConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t Buffer2[1024];
    float SingleFloatToRead = -1;
    int SingleIntToRead1 = -1;
    int SingleIntToRead2 = -1;

    ///RawADCData.PID.Elements.resize(NPIDElements); // Commented out on 15.8.2013, error from copying?

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/Analysis.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Could not open analysis config file!\n");
        exit(1);
    } else {
        Printf("INFO: Analysis file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';

        if (sscanf(Line, "%s %s ", Buffer, Buffer2 ) == 2) {
            if (!strcmp(Buffer, "OutputPath:")) {
                strcpy(OutputDirectory, Buffer2);
                Printf("INFO: OutputPath: %s",OutputDirectory);
            }
            if (!strcmp(Buffer, "InputPathRawData:")) {
                strcpy(InputPathRawData, Buffer2);
                Printf("INFO: InputPathRawData: %s",InputPathRawData);
            }
        }

        if (sscanf(Line, "%s %d %d ", Buffer, &SingleIntToRead1, &SingleIntToRead2 ) == 3) {
            if (!strcmp(Buffer, "SplitScaler:")) {
                ScalerBlockLength1 = SingleIntToRead1;
                ScalerBlockLength2 = SingleIntToRead2;
                Printf("Info: SplitScaler: %d %d", ScalerBlockLength1, ScalerBlockLength2);
            }
        }
        if (sscanf(Line, "%s %f ", Buffer, &SingleFloatToRead ) == 2) {
            if (!strcmp(Buffer, "ElectronBeamEnergy:")) {
                ElectronBeamEnergy = SingleFloatToRead;
                Printf("Info: ElectronBeamEnergy: %f", ElectronBeamEnergy );
            }
            if (!strcmp(Buffer, "RawDataFormat:")) {
                RawDataFormat = round(SingleFloatToRead);
                Printf("Info: RawDataFormat: %d", RawDataFormat );
            }
            if (!strcmp(Buffer, "CutHitsOutOfRange:")) {
                CutHitsOutOfRange = round(SingleFloatToRead);
                Printf("Info: CutHitsOutOfRange: %d", CutHitsOutOfRange );
            }


        }

    }


    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }




    return 0;
}


// ****************************************************************************************

int ReadTriggerConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];

    //For Elements
    int ScalerChNr = -1;


    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/Trigger.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Konnte die Trigger file nicht zum Lesen oeffnen!\n");
        exit(1);
    } else {
        Printf("INFO: Trigger file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';
        //
        if (sscanf(Line, "%s %d ", Buffer, &ScalerChNr ) == 2) {
            if (!strcmp(Buffer, "ExperimentLiveCounter:")) { RawADCData.ExperimentLiveCounterScalerCh = ScalerChNr; }
            if (!strcmp(Buffer, "UngatedLiveCounter:")) { RawADCData.UngatedLiveCounterScalerCh = ScalerChNr; }
            if (!strcmp(Buffer, "TaggerLiveCounter:")) { RawADCData.TaggerLiveCounterScalerCh = ScalerChNr; }
            if (!strcmp(Buffer, "NumberOfElements:")) {
                NTriggerElements = ScalerChNr;
                Printf("INFO: NTriggerElements: %d", NTriggerElements);
                if (RawADCData.Trigger.Elements.size() == 0) {
                    RawADCData.Trigger.Elements.resize(NTriggerElements);
                } else {
                    Printf("ERROR: NTriggerElements line needs to be first.");
                    exit(1);
                }
            }
            //
            if (!strcmp(Buffer, "ScalerElement:")) {
                //Printf("Trigger Element added: %d %d", AktElementNr, ScalerChNr);
                RawADCData.Trigger.Elements.at(AktElementNr).ADCCh = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).TDCCh = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).ScalerCh = ScalerChNr;

                //TDC calibration
                RawADCData.Trigger.Elements.at(AktElementNr).TDCLowThresholdNS = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).TDCHighThresholdNS = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).TDCOffsetTicks = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).TDCNSPerTick = -1;

                //ADC calibration
                RawADCData.Trigger.Elements.at(AktElementNr).ADCLowThresholdMEV = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).ADCHighThresholdMEV = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).ADCPedestalTicks = -1;
                RawADCData.Trigger.Elements.at(AktElementNr).ADCMEVPerTick = -1;

                RawADCData.Trigger.Elements.at(AktElementNr).CentralEnergyMEV = -1;

                //Position
                RawADCData.Trigger.Elements.at(AktElementNr).Position.r = 0;
                RawADCData.Trigger.Elements.at(AktElementNr).Position.theta = 0;
                RawADCData.Trigger.Elements.at(AktElementNr).Position.phi = 0;

                if (LookupTableScaler.at(ScalerChNr).DetectorID != -1) { Printf("ERROR: Scalerch %d already configured.", ScalerChNr); } else {
                    LookupTableScaler.at(ScalerChNr).DetectorID = 10;
                    LookupTableScaler.at(ScalerChNr).ElementID = AktElementNr;
                }
                AktElementNr++;
            }
        } else {
            //printf(". ");
        }
    }

    Printf("INFO: Trigger: Selected Scaler Channels %d %d %d ", RawADCData.ExperimentLiveCounterScalerCh, RawADCData.UngatedLiveCounterScalerCh, RawADCData.TaggerLiveCounterScalerCh);
    if (AktElementNr != NTriggerElements) {
        Printf("ERROR: Trigger: Read only %d elements, %d elements expected.", AktElementNr, NTriggerElements);
        exit(1);
    } else {
        Printf("INFO: Trigger: Read %d elements.", AktElementNr);
    }



    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }



    return 0;
}






// ****************************************************************************************

int ReadTaggerConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t Buffer2[1024];
    //For TimeOffset
    float SingleFloatToRead = -1;

    //For Elements
    int ADCChNr = -1;
    float ADCLowThresholdMEV = -1;
    float ADCHighThresholdMEV = -1;
    float ADCPedestalTicks = -1;
    float ADCMEVPerTick = -1;
    Char_t TDCChNr_Str[20];
    int TDCChNr = -1;
    float TDCLowThresholdNS = -1;
    float TDCHighThresholdNS = -1;
    float TDCOffsetTicks = -1;
    float TDCNSPerTick = -1;
    float PositionX = -1;
    float PositionY = -1;
    float PositionZ = -1;
    float CentralEnergyMEV = -1;
    float EnergyOverlapMEV = -1;
    int ScalerChNr = -1;

    RawADCData.Tagger.Elements.resize(NTaggerElements);

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/Tagger_Ladder.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Konnte die Datei nicht  zum Lesen oeffnen!\n");
        exit(1);
    } else {
        Printf("INFO: Tagger file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';
        TDCChNr_Str[0] = '\0';
        if (sscanf(Line, "%s %f", Buffer, &SingleFloatToRead ) == 2) {
            if (!strcmp(Buffer, "TimeOffsetNS:")) {
                RawADCData.Tagger.TimeOffsetNS = SingleFloatToRead;
                Printf("Info: Tagger TimeOffsetNS: %f", RawADCData.Tagger.TimeOffsetNS);
            }
        }
        if (sscanf(Line, "%s %s", Buffer, Buffer2 ) == 2) {
            if (!strcmp(Buffer, "TaggEffFile:")) {
                sprintf(Buffer, "%s/%s", InputPathDataDir, Buffer2);
                strcpy(TaggEffNormFile, Buffer);
                Printf("Info: TaggEffFile (with normalised TaggEff per Tagger channel): %s", TaggEffNormFile);
            }
        }


        //
        if (sscanf(Line, "%s %d %f %f %f %f %s %f %f %f %f %f %f %f %f %f %d", Buffer, &ADCChNr, &ADCLowThresholdMEV, &ADCHighThresholdMEV, &ADCPedestalTicks, &ADCMEVPerTick, TDCChNr_Str, &TDCLowThresholdNS, &TDCHighThresholdNS, &TDCOffsetTicks,
            &TDCNSPerTick, &PositionX, &PositionY, &PositionZ, &CentralEnergyMEV, &EnergyOverlapMEV, &ScalerChNr ) == 17) {
            if (!strcmp(Buffer, "Element:")) {
                if (strlen(TDCChNr_Str)>2) {TDCChNr_Str[strlen(TDCChNr_Str)-2] = '\0'; } //Cut away the "M0" part
                if (sscanf(TDCChNr_Str, "%d", &TDCChNr) != 1) {Printf("Error reading TDC Ch.");}
                //Printf("Tagger Element added: %d %f %f %f %f %d %f %f %f %f %f %f %f %f %f %d", ADCChNr, ADCLowThresholdMEV, ADCHighThresholdMEV, ADCPedestalTicks, ADCMEVPerTick, TDCChNr,
                  //     TDCLowThresholdNS, TDCHighThresholdNS, TDCOffsetTicks, TDCNSPerTick, PositionX, PositionY, PositionZ, CentralEnergyMEV, EnergyOverlapMEV, ScalerChNr);
                RawADCData.Tagger.Elements.at(AktElementNr).ADCCh = ADCChNr;
                RawADCData.Tagger.Elements.at(AktElementNr).TDCCh = TDCChNr;
                RawADCData.Tagger.Elements.at(AktElementNr).ScalerCh = ScalerChNr;

                //TDC calibration
                RawADCData.Tagger.Elements.at(AktElementNr).TDCLowThresholdNS = TDCLowThresholdNS;
                RawADCData.Tagger.Elements.at(AktElementNr).TDCHighThresholdNS = TDCHighThresholdNS;
                RawADCData.Tagger.Elements.at(AktElementNr).TDCOffsetTicks = TDCOffsetTicks;
                RawADCData.Tagger.Elements.at(AktElementNr).TDCNSPerTick = TDCNSPerTick;

                //ADC calibration
                RawADCData.Tagger.Elements.at(AktElementNr).ADCLowThresholdMEV = ADCLowThresholdMEV;
                RawADCData.Tagger.Elements.at(AktElementNr).ADCHighThresholdMEV = ADCHighThresholdMEV;
                RawADCData.Tagger.Elements.at(AktElementNr).ADCPedestalTicks = ADCPedestalTicks;
                RawADCData.Tagger.Elements.at(AktElementNr).ADCMEVPerTick = ADCMEVPerTick;

                RawADCData.Tagger.Elements.at(AktElementNr).CentralEnergyMEV = CentralEnergyMEV;

                //Position
                RawADCData.Tagger.Elements.at(AktElementNr).Position.r = 0;
                RawADCData.Tagger.Elements.at(AktElementNr).Position.theta = 0;
                RawADCData.Tagger.Elements.at(AktElementNr).Position.phi = 0;

                if (LookupTableADC.at(ADCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", ADCChNr); } else {
                    LookupTableADC.at(ADCChNr).DetectorID = 0;
                    LookupTableADC.at(ADCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(ADCChNr).TypeID = 0;
                }
                if (LookupTableADC.at(TDCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", TDCChNr); } else {
                    LookupTableADC.at(TDCChNr).DetectorID = 0;
                    LookupTableADC.at(TDCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(TDCChNr).TypeID = 1;
                }
                if (LookupTableScaler.at(ScalerChNr).DetectorID != -1) { Printf("ERROR: Scalerch %d already configured.\n", ScalerChNr); } else {
                    LookupTableScaler.at(ScalerChNr).DetectorID = 0;
                    LookupTableScaler.at(ScalerChNr).ElementID = AktElementNr;
                }
                AktElementNr++;
            } else {
                //Printf("Error reading file: Line does not start with Element:");
            }
        } else {
            //printf(". ");
        }
    }

    if (AktElementNr != NTaggerElements) {
        Printf("ERROR: Tagger: Read only %d elements, %d elements expected.", AktElementNr, NTaggerElements);
        exit(1);
    } else {
        Printf("INFO: Tagger: Read %d elements.", AktElementNr);
    }



    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }



    return 0;
}





//****************************************************************************************


int ReadCBConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t ADCChNr_Str[20];
    int ADCChNr = -1;
    float ADCLowThresholdMEV = -1;
    float ADCHighThresholdMEV = -1;
    float ADCPedestalTicks = -1;
    float ADCMEVPerTick = -1;
    Char_t TDCChNr_Str[20];
    int TDCChNr = -1;
    float TDCLowThresholdNS = -1;
    float TDCHighThresholdNS = -1;
    float TDCOffsetTicks = -1;
    float TDCNSPerTick = -1;
    int TDCTimeWalkCh = -1;
    float TDCTimeWalkP0 = -1;
    float TDCTimeWalkP1 = -1;
    float TDCTimeWalkP2 = -1;
    float TDCTimeWalkP3 = -1;
    float Position0 = -1;
    float Position1 = -1;
    float Position2 = -1;
    float SingleFloatToRead = -1;

    RawADCData.CB.Elements.resize(NCBElements);

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/CB_NaI.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Konnte die CB Datei nicht zum Lesen oeffnen!\n");
        exit(1);
    } else {
        Printf("INFO: CB file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';
        ADCChNr_Str[0] = '\0';
        TDCChNr_Str[0] = '\0';

        //read Energy-Scale Parameter
        if (sscanf(Line, "%s %f ", Buffer, &SingleFloatToRead ) == 2) {
            if (!strcmp(Buffer, "TimeOffsetNS:")) {
                RawADCData.CB.TimeOffsetNS = SingleFloatToRead;
                Printf("Info: CB TimeOffsetNS: %f", RawADCData.CB.TimeOffsetNS);
            }
            if (!strcmp(Buffer, "Energy-Scale:")) {
                CBEnergyScale = SingleFloatToRead;
                Printf("Info: CB Energy-Scale: %f", CBEnergyScale );
            }
            if (!strcmp(Buffer, "ClusterMinEnergy:")) {
                ClusterMinEnergy = SingleFloatToRead;
                Printf("Info: CB ClusterMinEnergy: %f", ClusterMinEnergy );
            }
        }


        //read TimeWalk Parameters
        if (sscanf(Line, "%s %d %f %f %f %f ", Buffer, &TDCTimeWalkCh, &TDCTimeWalkP0, &TDCTimeWalkP1, &TDCTimeWalkP2, &TDCTimeWalkP3 ) == 6) {
            if (!strcmp(Buffer, "TimeWalk:")) {
                //Printf("Info: CB TimeWalk Read, ch %d, paras: %f %f %f %f", TDCTimeWalkCh, TDCTimeWalkP0, TDCTimeWalkP1, TDCTimeWalkP2, TDCTimeWalkP3);
                RawADCData.CB.Elements.at(TDCTimeWalkCh).TimeWalkParameter.RiseTime = TDCTimeWalkP0;
                RawADCData.CB.Elements.at(TDCTimeWalkCh).TimeWalkParameter.Threshold = TDCTimeWalkP1;
                RawADCData.CB.Elements.at(TDCTimeWalkCh).TimeWalkParameter.Shift = TDCTimeWalkP2;
                RawADCData.CB.Elements.at(TDCTimeWalkCh).TimeWalkParameter.Power = TDCTimeWalkP3;
            }
        }

        //read Element parameters
        if (sscanf(Line, "%s %s %f %f %f %f %s %f %f %f %f %f %f %f", Buffer, &ADCChNr_Str, &ADCLowThresholdMEV, &ADCHighThresholdMEV, &ADCPedestalTicks, &ADCMEVPerTick, TDCChNr_Str, &TDCLowThresholdNS, &TDCHighThresholdNS, &TDCOffsetTicks,
            &TDCNSPerTick, &Position0, &Position1, &Position2 ) == 14) {
            if (!strcmp(Buffer, "Element:")) {
                if (strlen(ADCChNr_Str)>2) {ADCChNr_Str[strlen(ADCChNr_Str)-2] = '\0'; } //Cut away the "M1" part
                if (sscanf(ADCChNr_Str, "%d", &ADCChNr) != 1) {Printf("Error reading ADC Ch.");}
                if (strlen(TDCChNr_Str)>2) {TDCChNr_Str[strlen(TDCChNr_Str)-2] = '\0'; } //Cut away the "M0" part
                if (sscanf(TDCChNr_Str, "%d", &TDCChNr) != 1) {Printf("Error reading TDC Ch.");}
                //Printf("CB Element added: %d %f %f %f %f %d %f %f %f %f %f %f %f", ADCChNr, ADCLowThresholdMEV, ADCHighThresholdMEV, ADCPedestalTicks, ADCMEVPerTick, TDCChNr,
                  //     TDCLowThresholdNS, TDCHighThresholdNS, TDCOffsetTicks, TDCNSPerTick, Position0, Position1, Position2);
                RawADCData.CB.Elements.at(AktElementNr).ADCCh = ADCChNr;
                RawADCData.CB.Elements.at(AktElementNr).TDCCh = TDCChNr;
                RawADCData.CB.Elements.at(AktElementNr).ScalerCh = -1;

                RawADCData.CB.Elements.at(AktElementNr).TDCLowThresholdNS = TDCLowThresholdNS;
                RawADCData.CB.Elements.at(AktElementNr).TDCHighThresholdNS = TDCHighThresholdNS;
                RawADCData.CB.Elements.at(AktElementNr).TDCOffsetTicks = TDCOffsetTicks;
                RawADCData.CB.Elements.at(AktElementNr).TDCNSPerTick = TDCNSPerTick;

                //ADC calibration
                RawADCData.CB.Elements.at(AktElementNr).ADCLowThresholdMEV = ADCLowThresholdMEV;
                RawADCData.CB.Elements.at(AktElementNr).ADCHighThresholdMEV = ADCHighThresholdMEV;
                RawADCData.CB.Elements.at(AktElementNr).ADCPedestalTicks = ADCPedestalTicks;
                RawADCData.CB.Elements.at(AktElementNr).ADCMEVPerTick = ADCMEVPerTick;

                RawADCData.CB.Elements.at(AktElementNr).CentralEnergyMEV = -1;

                //Position
                RawADCData.CB.Elements.at(AktElementNr).Position.r = TMath::Sqrt( Position0*Position0 + Position1*Position1 + Position2*Position2 );
                RawADCData.CB.Elements.at(AktElementNr).Position.theta = acos( Position2 / RawADCData.CB.Elements.at(AktElementNr).Position.r );
                RawADCData.CB.Elements.at(AktElementNr).Position.phi = atan2( Position1, Position0 );


                if (LookupTableADC.at(ADCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", ADCChNr); } else {
                    LookupTableADC.at(ADCChNr).DetectorID = 1;
                    LookupTableADC.at(ADCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(ADCChNr).TypeID = 0;
                }
                if (LookupTableADC.at(TDCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", TDCChNr); } else {
                    LookupTableADC.at(TDCChNr).DetectorID = 1;
                    LookupTableADC.at(TDCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(TDCChNr).TypeID = 1;
                }
                AktElementNr++;
            } else {
                //Printf("Error reading file: Line does not start with Element:");
            }
        } else {
            //printf(". ");
        }
    }

    if (AktElementNr != NCBElements) {
        Printf("ERROR: CB: Read only %d elements, %d elements expected.", AktElementNr, NCBElements);
        exit(1);
    } else {
        Printf("INFO: CB: Read %d elements.", AktElementNr);
    }


    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }


 /*       for (int tempI = 0; tempI<RawADCData.CB.Elements.size(); tempI++) {
            Printf("CB %d: %f %f %f", tempI,
                   RawADCData.CB.Elements.at(tempI).Position.r,
                   RawADCData.CB.Elements.at(tempI).Position.theta*180./TMath::Pi(),
                   RawADCData.CB.Elements.at(tempI).Position.phi*180./TMath::Pi() );
        }
        return 0;
*/



    return 0;
}





//****************************************************************************************


int ReadPIDConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];
    int ADCChNr = -1;
    float ADCLowThresholdMEV = -1;
    float ADCHighThresholdMEV = -1;
    float ADCPedestalTicks = -1;
    float ADCMEVPerTick = -1;
    Char_t TDCChNr_Str[20];
    int TDCChNr = -1;
    float TDCLowThresholdNS = -1;
    float TDCHighThresholdNS = -1;
    float TDCOffsetTicks = -1;
    float TDCNSPerTick = -1;
    float Position0 = -1;
    float Position1 = -1;
    float Position2 = -1;
    float SingleFloatToRead = -1;

    RawADCData.PID.Elements.resize(NPIDElements);

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/CB_PID.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Konnte die PID Datei nicht zum Lesen oeffnen!\n");
        exit(1);
    } else {
        Printf("INFO: PID file: %s", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';
        TDCChNr_Str[0] = '\0';

        //read Energy-Scale Parameter
        if (sscanf(Line, "%s %f ", Buffer, &SingleFloatToRead ) == 2) {
            if (!strcmp(Buffer, "TimeOffsetNS:")) {
                RawADCData.PID.TimeOffsetNS = SingleFloatToRead;
                Printf("Info: PID TimeOffsetNS: %f", RawADCData.PID.TimeOffsetNS);
            }
            if (!strcmp(Buffer, "PIDOffset:")) {
                PIDOffset = SingleFloatToRead;
                Printf("Info: PID PIDOffset: %f", PIDOffset );
            }
            if (!strcmp(Buffer, "PIDCBPhiDelta:")) {
                PIDCBPhiDelta = SingleFloatToRead;
                Printf("Info: PID PIDCBPhiDelta: %f", PIDCBPhiDelta );
            }
        }


        //
        if (sscanf(Line, "%s %d %f %f %f %f %s %f %f %f %f %f %f %f", Buffer, &ADCChNr, &ADCLowThresholdMEV, &ADCHighThresholdMEV, &ADCPedestalTicks, &ADCMEVPerTick, TDCChNr_Str, &TDCLowThresholdNS, &TDCHighThresholdNS, &TDCOffsetTicks,
            &TDCNSPerTick, &Position0, &Position1, &Position2 ) == 14) {
            if (!strcmp(Buffer, "Element:")) {
                if (strlen(TDCChNr_Str)>2) {TDCChNr_Str[strlen(TDCChNr_Str)-2] = '\0'; } //Cut away the "M0" part
                if (sscanf(TDCChNr_Str, "%d", &TDCChNr) != 1) {Printf("Error reading TDC Ch.");}
                //Printf("PID Element added: %d %f %f %f %f %d %f %f %f %f %f %f %f", ADCChNr, ADCLowThresholdMEV, ADCHighThresholdMEV, ADCPedestalTicks, ADCMEVPerTick, TDCChNr,
                  //     TDCLowThresholdNS, TDCHighThresholdNS, TDCOffsetTicks, TDCNSPerTick, PositionX, PositionY, PositionZ);
                RawADCData.PID.Elements.at(AktElementNr).ADCCh = ADCChNr;
                RawADCData.PID.Elements.at(AktElementNr).TDCCh = TDCChNr;
                RawADCData.PID.Elements.at(AktElementNr).ScalerCh = -1;

                RawADCData.PID.Elements.at(AktElementNr).TDCLowThresholdNS = TDCLowThresholdNS;
                RawADCData.PID.Elements.at(AktElementNr).TDCHighThresholdNS = TDCHighThresholdNS;
                RawADCData.PID.Elements.at(AktElementNr).TDCOffsetTicks = TDCOffsetTicks;
                RawADCData.PID.Elements.at(AktElementNr).TDCNSPerTick = TDCNSPerTick;

                //ADC calibration
                RawADCData.PID.Elements.at(AktElementNr).ADCLowThresholdMEV = ADCLowThresholdMEV;
                RawADCData.PID.Elements.at(AktElementNr).ADCHighThresholdMEV = ADCHighThresholdMEV;
                RawADCData.PID.Elements.at(AktElementNr).ADCPedestalTicks = ADCPedestalTicks;
                RawADCData.PID.Elements.at(AktElementNr).ADCMEVPerTick = ADCMEVPerTick;

                RawADCData.PID.Elements.at(AktElementNr).CentralEnergyMEV = -1;

                //Position
                RawADCData.PID.Elements.at(AktElementNr).Position.r = Position0;
                RawADCData.PID.Elements.at(AktElementNr).Position.theta = Position1 / 180. * TMath::Pi();
                double TempVar = Position2+PIDOffset;
                if (TempVar > 180) TempVar = TempVar-360;
                if (TempVar < -180) TempVar = TempVar+360;
                RawADCData.PID.Elements.at(AktElementNr).Position.phi = TempVar / 180. * TMath::Pi();

                if (LookupTableADC.at(ADCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", ADCChNr); } else {
                    LookupTableADC.at(ADCChNr).DetectorID = 2;
                    LookupTableADC.at(ADCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(ADCChNr).TypeID = 0;
                }
                if (LookupTableADC.at(TDCChNr).DetectorID != -1) { Printf("ERROR: ADCch %d already configured.\n", TDCChNr); } else {
                    LookupTableADC.at(TDCChNr).DetectorID = 2;
                    LookupTableADC.at(TDCChNr).ElementID = AktElementNr;
                    LookupTableADC.at(TDCChNr).TypeID = 1;
                }
                AktElementNr++;
            } else {
                //Printf("Error reading file: Line does not start with Element:");
            }
        } else {
            //printf(". ");
        }
    }

    if (AktElementNr != NPIDElements) {
        Printf("ERROR: PID: Read only %d elements, %d elements expected.", AktElementNr, NPIDElements);
        exit(1);
    } else {
        Printf("INFO: PID: Read %d elements.", AktElementNr);
    }


    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }




    return 0;
}

//****************************************************************************************



//****************************************************************************************


int ReadRunsInformationConfigurationFile() {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t buffer[1024];
    Char_t FileName[1024];
    Char_t BufferDataPath[1024];
    char FilePathRunInformation[1024] = "";

    int RunNumber = -1;
//    int FileFormat = 1; //1 = Mk1, 2 = Mk2
    int RunType = 0; //0 = normal, good, Production Run
    int TaggEffCorrespondingRun = -1; //Only for TaggEff Runs: which real measurement run it belongs to
    int TaggEffPartNo = 0; // 1=Background1, 2=TaggEff measurement, 3=Background2
    int BeamTimeBlock = 0;
    float RunDateTime;
    int TargetNMRSignal = 0;
    int TargetOrient = 0;
    int HelBitPresent = 0;
    int AcquCrash = 0;
    float LaddP2Ratio = -1.;
    float TargetPolDegree = 0;
    float ErrorTargetPolDegree = 0;
    float BeamPolDegree = 0;
    float AbsTaggEff = -1.;


    sprintf(FilePathRunInformation, "%s/RunsMetaInformation.dat", InputPathDataDir); //Path and Filename to File with RunInfo: for all files to analyse
    FILE *readFile = NULL;
    readFile=fopen(FilePathRunInformation, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"ERROR: Konnte die RunsMetaInformation Datei nicht zum Lesen oeffnen!\n");
        exit(1);
    } else {
        Printf("INFO: RunsMetaInformation file: %s",FilePathRunInformation);
    }

    int AktElementNr = 0;
    RunsMetaInformation.clear();
    NRunsMetaInformation = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        buffer[0] = '\0';

        //
        if (sscanf(Line, "%s %d %s %d %d %f %d %d %d %d %f %f %f %f %f %s", buffer, &RunNumber, &FileName, &RunType, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, &BufferDataPath ) == 16) {
            if (!strcmp(buffer, "RunInfo:")) {
                TRunsMetaInformation TempRunInfo;
                TempRunInfo.RunNumber = RunNumber;
                TempRunInfo.FileName = FileName;
                TempRunInfo.RunType = RunType;
                TempRunInfo.TaggEffCorrespondingRun = -1;
                TempRunInfo.TaggEffPartNo = -1;
                TempRunInfo.BeamTimeBlock = BeamTimeBlock;
                TempRunInfo.RunDateTime = RunDateTime;
                TempRunInfo.TargetNMRSignal = TargetNMRSignal;
                TempRunInfo.TargetOrient = TargetOrient;
                TempRunInfo.HelBitPresent = HelBitPresent;
                TempRunInfo.AcquCrash = AcquCrash;
                TempRunInfo.LaddP2Ratio = LaddP2Ratio;
                TempRunInfo.TargetPolDegree = TargetPolDegree;
                TempRunInfo.ErrorTargetPolDegree = ErrorTargetPolDegree;
                TempRunInfo.BeamPolDegree = BeamPolDegree;
                TempRunInfo.AbsTaggEff = AbsTaggEff;
                TempRunInfo.DataPath = BufferDataPath;

                RunsMetaInformation.push_back(TempRunInfo);
                // Printf("Run %i info added (16 info): %d %d %f %f %s", AktElementNr, RunNumber, RunType, LaddP2Ratio, AbsTaggEff, TempRunInfo.DataPath.c_str() );
                AktElementNr++;
            }
        }

        if (sscanf(Line, "%s %d %s %d %d %d %f %d %d %d %d %f %f %f %f %f %s", buffer, &RunNumber, &FileName, &TaggEffCorrespondingRun, &TaggEffPartNo, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, &BufferDataPath ) == 17) {
            if (!strcmp(buffer, "TaggRunInfo:")) {
                TRunsMetaInformation TempRunInfo;
                TempRunInfo.RunNumber = RunNumber;
                TempRunInfo.FileName = FileName;
                TempRunInfo.RunType = 1; //TaggEff Run
                TempRunInfo.TaggEffCorrespondingRun = TaggEffCorrespondingRun;
                TempRunInfo.TaggEffPartNo = TaggEffPartNo;
                TempRunInfo.BeamTimeBlock = BeamTimeBlock;
                TempRunInfo.RunDateTime = RunDateTime;
                TempRunInfo.TargetNMRSignal = TargetNMRSignal;
                TempRunInfo.TargetOrient = TargetOrient;
                TempRunInfo.HelBitPresent = HelBitPresent;
                TempRunInfo.AcquCrash = AcquCrash;
                TempRunInfo.LaddP2Ratio = LaddP2Ratio;
                TempRunInfo.TargetPolDegree = TargetPolDegree;
                TempRunInfo.ErrorTargetPolDegree = ErrorTargetPolDegree;
                TempRunInfo.BeamPolDegree = BeamPolDegree;
                TempRunInfo.AbsTaggEff = AbsTaggEff;
                TempRunInfo.DataPath = BufferDataPath;
                RunsMetaInformation.push_back(TempRunInfo);

                //Printf("TaggEffRun %i info added (17 info): %d %d %d %f %f %s", AktElementNr, RunNumber, TaggEffCorrespondingRun, TaggEffPartNo, LaddP2Ratio, AbsTaggEff, TempRunInfo.DataPath.c_str() );
                AktElementNr++;
            }
        }
    }
    NRunsMetaInformation = AktElementNr;

    Printf("INFO: RunInfo: Read %d elements.", AktElementNr);


    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }




    return 0;
}


//****************************************************************************************

int ReadConfigurationFiles() {
    ReadAnalysisConfigurationFile();
    ReadTriggerConfigurationFile();
    ReadTaggerConfigurationFile();
    ReadCBConfigurationFile();
    ReadPIDConfigurationFile();

}
