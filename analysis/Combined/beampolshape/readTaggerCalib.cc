int ReadTaggerConfigurationFile() {
    printf("--------------------------------------\n");
    Char_t Line[1024];
    Char_t Buffer[1024];

    //For Elements
    int ADCChNr = -1;
    float ADCLowThresholdMEV = -1;
    float ADCHighThresholdMEV = -1;
    float ADCPedestalTicks = -1;
    float ADCMEVPerTick = -1;
    Char_t TDCChNr_Str[20];
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

    FILE *readFile = NULL;
    Char_t TempStr[1024];
	Char_t	InputPathDataDir[1024] = "/home/peter/NewWorker/data/2010_09";
    strcpy(TempStr, InputPathDataDir);
    strcat(TempStr, "/Tagger_Ladder.dat");
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        printf("Konnte die Datei nicht  zum Lesen oeffnen!\n");
        exit(1);
    } else {
        printf("INFO: Tagger file: %s\n", TempStr);
    }

    int AktElementNr = 0;
    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';

        //
        if (sscanf(Line, "%s %d %f %f %f %f %s %f %f %f %f %f %f %f %f %f %d", Buffer, &ADCChNr, &ADCLowThresholdMEV, &ADCHighThresholdMEV, &ADCPedestalTicks, &ADCMEVPerTick, TDCChNr_Str, &TDCLowThresholdNS, &TDCHighThresholdNS, &TDCOffsetTicks,
            &TDCNSPerTick, &PositionX, &PositionY, &PositionZ, &CentralEnergyMEV, &EnergyOverlapMEV, &ScalerChNr ) == 17) {
            if (!strcmp(Buffer, "Element:")) {
                TaggerCentralEnergy.push_back(CentralEnergyMEV);

                AktElementNr++;
            } else {
                //printf("Error reading file: Line does not start with Element:");
            }
        } else {
            //printf(". ");
        }
    }

    if (AktElementNr != NTaggerElements) {
        printf("ERROR: Tagger: Read only %d elements, %d elements expected.\n", AktElementNr, NTaggerElements);
        exit(1);
    } else {
        printf("INFO: Tagger: Read %d elements.\n", AktElementNr);
    }



    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        printf("Konnte die Daten nach dem Lesen nicht schliessen!\n");
        exit(1);
    }



    return 0;
}
