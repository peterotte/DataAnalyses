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

typedef struct TTaggEffRunsMetaInformation {
    int CorrespodingRun;
    int RunBackground1;
    int RunMeasurement;
    int RunBackground2;
    int BeamTimeBlock;
    double MeanTaggEff;
} TTaggEffRunsMetaInformation;
std::vector<TTaggEffRunsMetaInformation> TaggEffRunsMetaInformation;
int NTaggEffRunsMetaInformation = 0;




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
    TaggEffRunsMetaInformation.clear();
    NTaggEffRunsMetaInformation = 0;
    int LastTaggEffCorrespondingRun = -1;
    int LastRunBackground1 = -1;
    int LastRunMeasurement = -1;
    int LastRunBackground2 = -1;
    int LastBeamTimeBlock = -1;


    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        buffer[0] = '\0';

        //
        if (sscanf(Line, "%s %d %s %d %d %f %d %d %d %d %f %f %f %f %f %s", buffer, &RunNumber, FileName, &RunType, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, BufferDataPath ) == 16) {
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
                if (fDebugInfo) Printf("Run %i info added (16 info): %d %d %f %f %s", AktElementNr, RunNumber, RunType, LaddP2Ratio, AbsTaggEff, TempRunInfo.DataPath.c_str() );
                AktElementNr++;
            }
        }

        if (sscanf(Line, "%s %d %s %d %d %d %f %d %d %d %d %f %f %f %f %f %s", buffer, &RunNumber, FileName, &TaggEffCorrespondingRun, &TaggEffPartNo, &BeamTimeBlock,
                   &RunDateTime, &TargetNMRSignal, &TargetOrient, &HelBitPresent, &AcquCrash, &LaddP2Ratio, &TargetPolDegree, &ErrorTargetPolDegree, &BeamPolDegree, &AbsTaggEff, BufferDataPath ) == 17) {
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

                if ( LastTaggEffCorrespondingRun != TaggEffCorrespondingRun ) {
                    //******** These lines are needed a few lines later
                    if ( (LastTaggEffCorrespondingRun != -1) && (LastRunMeasurement != -1) ) {
                        if (fBeamtime == LastBeamTimeBlock) {
                            //Fixes
                            if ( (LastRunBackground1 == -1) && (LastRunBackground2 != -1) ) {LastRunBackground1 = LastRunBackground2; Printf("INFO: Used 2nd BG run also as 1st."); };
                            if ( (LastRunBackground2 == -1) && (LastRunBackground1 != -1) ) {LastRunBackground2 = LastRunBackground1; Printf("INFO: Used 1st BG run also as 2nd."); };
                            if ( (LastRunBackground2 == -1) && (LastRunBackground1 == -1) ) {Printf("ERROR: No BG run defined."); };

                            TTaggEffRunsMetaInformation TempTaggEffRunInfo;
                            TempTaggEffRunInfo.CorrespodingRun = LastTaggEffCorrespondingRun;
                            TempTaggEffRunInfo.RunBackground1 = LastRunBackground1;
                            TempTaggEffRunInfo.RunMeasurement = LastRunMeasurement;
                            TempTaggEffRunInfo.RunBackground2 = LastRunBackground2;
                            TempTaggEffRunInfo.BeamTimeBlock = LastBeamTimeBlock;
                            TempTaggEffRunInfo.MeanTaggEff = 0;
                            TaggEffRunsMetaInformation.push_back(TempTaggEffRunInfo);
                            NTaggEffRunsMetaInformation++;
                            if (fDebugInfo) printf("TaggEffRuns (number %i): %i (%i, %i, %i)  Beamtime: %i\n", NTaggEffRunsMetaInformation, LastTaggEffCorrespondingRun, LastRunBackground1, LastRunMeasurement, LastRunBackground2, LastBeamTimeBlock);
                        } else {
                            if (fDebugInfo) printf("INFO: Rejected, wrong beamtime: TaggEffRuns (number %i): %i (%i, %i, %i)  Beamtime: %i\n", NTaggEffRunsMetaInformation, LastTaggEffCorrespondingRun, LastRunBackground1, LastRunMeasurement, LastRunBackground2, LastBeamTimeBlock);
                        }
                    }
                    //*************

                    LastTaggEffCorrespondingRun = TaggEffCorrespondingRun;
                    LastRunBackground1 = -1;
                    LastRunMeasurement = -1;
                    LastRunBackground2 = -1;
                    LastBeamTimeBlock = -1;
                }

                if (TaggEffPartNo == 1) { LastRunBackground1 = AktElementNr; }
                if (TaggEffPartNo == 2) { LastRunMeasurement = AktElementNr; LastBeamTimeBlock = BeamTimeBlock; }
                if (TaggEffPartNo == 3) { LastRunBackground2 = AktElementNr; }

                if (fDebugInfo) Printf("TaggEffRun %i info added (17 info): %d %d %d %f %f %s", AktElementNr, RunNumber, TaggEffCorrespondingRun, TaggEffPartNo, LaddP2Ratio, AbsTaggEff, TempRunInfo.DataPath.c_str() );
                AktElementNr++;
            }
        }
    }
    NRunsMetaInformation = AktElementNr;

    //***********
    if ( (LastTaggEffCorrespondingRun != -1) && (LastRunMeasurement != -1) ) {
        if (fBeamtime == LastBeamTimeBlock) {
            //Fixes
            if ( (LastRunBackground1 == -1) && (LastRunBackground2 != -1) ) {LastRunBackground1 = LastRunBackground2; Printf("INFO: Used 2nd BG run also as 1st."); };
            if ( (LastRunBackground2 == -1) && (LastRunBackground1 != -1) ) {LastRunBackground2 = LastRunBackground1; Printf("INFO: Used 1st BG run also as 2nd."); };
            if ( (LastRunBackground2 == -1) && (LastRunBackground1 == -1) ) {Printf("ERROR: No BG run defined."); };

            TTaggEffRunsMetaInformation TempTaggEffRunInfo;
            TempTaggEffRunInfo.CorrespodingRun = LastTaggEffCorrespondingRun;
            TempTaggEffRunInfo.RunBackground1 = LastRunBackground1;
            TempTaggEffRunInfo.RunMeasurement = LastRunMeasurement;
            TempTaggEffRunInfo.RunBackground2 = LastRunBackground2;
            TempTaggEffRunInfo.BeamTimeBlock = LastBeamTimeBlock;
            TempTaggEffRunInfo.MeanTaggEff = 0;
            TaggEffRunsMetaInformation.push_back(TempTaggEffRunInfo);
            NTaggEffRunsMetaInformation++;
            if (fDebugInfo) printf("TaggEffRuns (number %i): %i (%i, %i, %i)  Beamtime: %i\n", NTaggEffRunsMetaInformation, LastTaggEffCorrespondingRun, LastRunBackground1, LastRunMeasurement, LastRunBackground2, LastBeamTimeBlock);
        } else {
            if (fDebugInfo) printf("INFO: Rejected, wrong beamtime: TaggEffRuns (number %i): %i (%i, %i, %i)  Beamtime: %i\n", NTaggEffRunsMetaInformation, LastTaggEffCorrespondingRun, LastRunBackground1, LastRunMeasurement, LastRunBackground2, LastBeamTimeBlock);

        }
    }
    //**************

    Printf("INFO: RunInfo: Read %d elements.", AktElementNr);
    Printf("INFO: TaggEffRunInfo: Read %d Taggeff measurements.", NTaggEffRunsMetaInformation);

    int status=fclose(readFile);
    if(status!=0)//Konnte die datei nicht schließen
    {
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    }




    return 0;
}



int ReadTaggEffConfigurationFile(int fRunNumber) {
    Printf("--------------------------------------");
    Char_t Line[1024];
    Char_t Buffer[1024];
    Char_t Buffer2[1024];
    float SingleFloatToRead;

    FILE *readFile = NULL;
    Char_t TempStr[1024];
    sprintf(TempStr, "%s/%s/TaggEff.dat", InputPathDataDir, RunsMetaInformation.at(fRunNumber).DataPath.c_str() );
    readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

    Printf("INFO: Try to open TagEff Analysis file (for RunFile %i): %s", fRunNumber, TempStr);
    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        fprintf(stderr,"Could not open TaggEff analysis config file!\n");
        exit(1);
    } else {
        Printf("INFO: TagEff Analysis file (for RunFile %i): %s", fRunNumber, TempStr);
    }

    while (!feof(readFile)) {
        if (!fgets(Line, sizeof(Line), readFile)) break;
        Buffer[0] = '\0';

        if (sscanf(Line, "%s %s ", Buffer, Buffer2 ) == 2) {
            if (!strcmp(Buffer, "OutputPath:")) {
                strcpy(TaggEffOutputPath, Buffer2);
                Printf("INFO: OutputPath: %s",TaggEffOutputPath);
            }
            if (!strcmp(Buffer, "InputPath:")) {
                strcpy(TaggEffInputPath, Buffer2);
                Printf("INFO: InputPath: %s",TaggEffInputPath);
            }
        }
        if (sscanf(Line, "%s %f ", Buffer, &SingleFloatToRead ) == 2) {
            if (!strcmp(Buffer, "TaggerClockSpeed:")) {
                TaggEffTaggerClockSpeed = SingleFloatToRead;
                Printf("INFO: TaggerClockSpeed: %f",TaggEffTaggerClockSpeed);
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



//************************************************************************
// Write Configuration files
//************************************************************************


int WriteRunsInformationConfigurationFile (int WriteAbsTaggEffNew = 0) {
    printf("INFO: Starting writing new ConfigFile.\n");

    FILE *fOutputFile;
    fOutputFile = fopen ("RunsMetaInformation.new.dat","w");
    if (fOutputFile == NULL) {
        printf("ERROR: Can not open file for writing.\n");
        return 1;
    }

    fprintf(fOutputFile, "# RunInfo:\tRunNumber\tFileName\tRunType\tBeamTimeBlock\tRunDateTime\tTargetNMRSignal\tTargetOrient\tHelBitPresent\tAcquCrash\tLaddP2Ratio\tTargetPolDegree\tErrorTargetPolDegree\tBeamPolDegree\tAbsTaggEff\tBufferDataPath\n");

    if (WriteAbsTaggEffNew) {
        printf("INFO: The new ConfigFile will be filled with 1/LaddP2Ratio in column AbsTaggEff.\n");
    }

    for (int i=0; i<NRunsMetaInformation; i++) {
        if (RunsMetaInformation.at(i).RunType == 1) {
            fprintf(fOutputFile, "TaggRunInfo:\t%d\t%s\t%d\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%s\n", RunsMetaInformation.at(i).RunNumber, RunsMetaInformation.at(i).FileName.c_str(),
                RunsMetaInformation.at(i).TaggEffCorrespondingRun, RunsMetaInformation.at(i).TaggEffPartNo, RunsMetaInformation.at(i).BeamTimeBlock, RunsMetaInformation.at(i).RunDateTime,
                RunsMetaInformation.at(i).TargetNMRSignal, RunsMetaInformation.at(i).TargetOrient, RunsMetaInformation.at(i).HelBitPresent,
                RunsMetaInformation.at(i).AcquCrash, RunsMetaInformation.at(i).LaddP2Ratio, RunsMetaInformation.at(i).TargetPolDegree,
                RunsMetaInformation.at(i).ErrorTargetPolDegree, RunsMetaInformation.at(i).BeamPolDegree, RunsMetaInformation.at(i).AbsTaggEff,
                   RunsMetaInformation.at(i).DataPath.c_str() );

        } else {
            Double_t TempAbsTaggEff = RunsMetaInformation.at(i).AbsTaggEff;
            if (WriteAbsTaggEffNew) TempAbsTaggEff = 1/RunsMetaInformation.at(i).LaddP2Ratio;
            fprintf(fOutputFile, "RunInfo:\t%d\t%s\t%d\t%d\t%f\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%s\n", RunsMetaInformation.at(i).RunNumber, RunsMetaInformation.at(i).FileName.c_str(),
                RunsMetaInformation.at(i).RunType, RunsMetaInformation.at(i).BeamTimeBlock, RunsMetaInformation.at(i).RunDateTime,
                RunsMetaInformation.at(i).TargetNMRSignal, RunsMetaInformation.at(i).TargetOrient, RunsMetaInformation.at(i).HelBitPresent,
                RunsMetaInformation.at(i).AcquCrash, RunsMetaInformation.at(i).LaddP2Ratio, RunsMetaInformation.at(i).TargetPolDegree,
                RunsMetaInformation.at(i).ErrorTargetPolDegree, RunsMetaInformation.at(i).BeamPolDegree, TempAbsTaggEff,
                   RunsMetaInformation.at(i).DataPath.c_str() );
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

    printf("INFO: Replacing RunsMetaFil completed.\n");
}




int SaveTaggEffNormRootFile() {
    printf("\n\nINFO: Start saving TaggEff Norm Root file.\n");
    TH1D *h1 = (TH1D*)gROOT->FindObject("TaggEffMeanNorm");

    if (h1) {
        char buffer[1024] = "";
        sprintf(buffer, "%s/%s/TaggEffNorm.root", InputPathDataDir, RunsMetaInformation.at(TaggEffRunsMetaInformation.at(0).CorrespodingRun).DataPath.c_str()); //Path and Filename to File with RunInfo: for all files to analyse
        printf("INFO: Write file to %s\n", buffer);
        TFile* foutput = new TFile(buffer, "RECREATE");
        if( !foutput ){ printf("ERROR: File %s not found\n", buffer); return 0; }
        h1->Write();
        foutput->Close();
    } else {
        printf("ERROR: TaggEffMean histogram not found.\n");
    }
    printf("INFO: Saving TaggEff Norm Root file completed.\n");

    return 0;
}
