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


//************************************************************************
// Read Configuration files
//************************************************************************

int ReadRunsInformationConfigurationFile(int fBeamtime, int fDebugInfo=0) {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t buffer[1024];
    char FileName[1024]; //was before using it in Root with ACLiC Char_t
    char BufferDataPath[1024]; //was before using it in Root with ACLiC Char_t
    char FilePathRunInformation[1024] = "";

    int RunNumber = -1;
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
    float CBEnergyScale = 1.0;


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
        if (sscanf(Line, "%s %d %s %d %d %f %d %d %d %d %f %f %f %f %f %s %f", buffer, &RunNumber, FileName, &RunType, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, BufferDataPath, &CBEnergyScale ) == 17) {
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
                TempRunInfo.CBEnergyScale = CBEnergyScale;

                RunsMetaInformation.push_back(TempRunInfo);
                if (fDebugInfo) Printf("Run %i info added (16 info): %d %d %f %f %f %s", AktElementNr, RunNumber, RunType, LaddP2Ratio, AbsTaggEff, CBEnergyScale, TempRunInfo.DataPath.c_str() );
                AktElementNr++;
            }
        }

        if (sscanf(Line, "%s %d %s %d %d %d %f %d %d %d %d %f %f %f %f %f %s %f", buffer, &RunNumber, FileName, &TaggEffCorrespondingRun, &TaggEffPartNo, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, BufferDataPath, &CBEnergyScale ) == 18) {
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
                TempRunInfo.CBEnergyScale = CBEnergyScale;

                RunsMetaInformation.push_back(TempRunInfo);

                if (fDebugInfo) Printf("TaggEffRun %i info added (17 info): %d %d %d %f %f %s", AktElementNr, RunNumber, TaggEffCorrespondingRun, TaggEffPartNo, LaddP2Ratio, AbsTaggEff, TempRunInfo.DataPath.c_str() );
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


//************************************************************************
// Write Configuration files
//************************************************************************


int WriteRunsInformationConfigurationFile (int fWriteCBEnergyScaleNew) {
    printf("INFO: Starting writing new ConfigFile.\n");

    FILE *fOutputFile;
    fOutputFile = fopen ("RunsMetaInformation.new.dat","w");
    if (fOutputFile == NULL) {
        printf("ERROR: Can not open file for writing.\n");
        return 1;
    }

    fprintf(fOutputFile, "# RunInfo:\tRunNumber\tFileName\tRunType\tBeamTimeBlock\tRunDateTime\tTargetNMRSignal\tTargetOrient\tHelBitPresent\tAcquCrash\tLaddP2Ratio\tTargetPolDegree\tErrorTargetPolDegree\tBeamPolDegree\tAbsTaggEff\tBufferDataPath\tCBEnergyScale\n");

    for (int i=0; i<NRunsMetaInformation; i++) {
        if (fWriteCBEnergyScaleNew) RunsMetaInformation.at(i).CBEnergyScale = 1.0;
        if (RunsMetaInformation.at(i).RunType == 1) {
            fprintf(fOutputFile, "TaggRunInfo:\t%d\t%s\t%d\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%s\t%f\n", RunsMetaInformation.at(i).RunNumber, RunsMetaInformation.at(i).FileName.c_str(),
                RunsMetaInformation.at(i).TaggEffCorrespondingRun, RunsMetaInformation.at(i).TaggEffPartNo, RunsMetaInformation.at(i).BeamTimeBlock, RunsMetaInformation.at(i).RunDateTime,
                RunsMetaInformation.at(i).TargetNMRSignal, RunsMetaInformation.at(i).TargetOrient, RunsMetaInformation.at(i).HelBitPresent,
                RunsMetaInformation.at(i).AcquCrash, RunsMetaInformation.at(i).LaddP2Ratio, RunsMetaInformation.at(i).TargetPolDegree,
                RunsMetaInformation.at(i).ErrorTargetPolDegree, RunsMetaInformation.at(i).BeamPolDegree, RunsMetaInformation.at(i).AbsTaggEff,
                   RunsMetaInformation.at(i).DataPath.c_str(), RunsMetaInformation.at(i).CBEnergyScale );

        } else {
            fprintf(fOutputFile, "RunInfo:\t%d\t%s\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%s\t%f\n", RunsMetaInformation.at(i).RunNumber, RunsMetaInformation.at(i).FileName.c_str(),
                RunsMetaInformation.at(i).RunType, RunsMetaInformation.at(i).BeamTimeBlock, RunsMetaInformation.at(i).RunDateTime,
                RunsMetaInformation.at(i).TargetNMRSignal, RunsMetaInformation.at(i).TargetOrient, RunsMetaInformation.at(i).HelBitPresent,
                RunsMetaInformation.at(i).AcquCrash, RunsMetaInformation.at(i).LaddP2Ratio, RunsMetaInformation.at(i).TargetPolDegree,
                RunsMetaInformation.at(i).ErrorTargetPolDegree, RunsMetaInformation.at(i).BeamPolDegree, RunsMetaInformation.at(i).AbsTaggEff,
                   RunsMetaInformation.at(i).DataPath.c_str(), RunsMetaInformation.at(i).CBEnergyScale );
        }
    }

    fclose (fOutputFile);
    printf("INFO: New ConfigFile successfully written.\n");

    return 0;
}



int ReplaceAndBackupRunsMetaInformation() {
    printf("\n\nINFO: Start replacing RunsMetaFile.\n");

    char FilePathRunInformation[1024] = "";
    sprintf(FilePathRunInformation, "%s/RunsMetaInformation.dat", InputPathDataDir); //Path and Filename to File with RunInfo: for all files to analyse

    char buffer[1024] = "";
    sprintf(buffer, "mv %s %s_backup", FilePathRunInformation, FilePathRunInformation);
    system(buffer);

    sprintf(buffer, "mv RunsMetaInformation.new.dat %s", FilePathRunInformation);
    system(buffer);

    printf("INFO: Replacing RunsMetaFile completed.\n");
}




