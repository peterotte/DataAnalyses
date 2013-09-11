//Buffersize richtig gross machen (hier nicht so gross...)
//#define BUFFER_SIZE 4500000000 //Buffer size in bytes
#define AcquDAQ_BUFFER_SIZE 0x8000 //Defined by John

unsigned char *buffer; //For reading the Raw Data file
int BUFFER_SIZE = AcquDAQ_BUFFER_SIZE * 2000; //Buffer size to read in bytes
//long nRawFile = 0;
int BLOCKSIZE4 = AcquDAQ_BUFFER_SIZE/sizeof(int);
int BufferSize4 = BUFFER_SIZE/sizeof(int);
long long int NBytesInRawFile = 0;
long int NIntsInRawFile = 0;
FILE *readFile = NULL;


int PrepareReadRawData() {
    Printf("--------------------------------------");

    Char_t RawDataFileName[1024] = ""; //With Path

    //Prepare FileName to read
    strcpy(RawDataFileName, InputPathRawData);
    strcat(RawDataFileName, "/");
    strcat(RawDataFileName, RunsMetaInformation.at(IndexRunMetaInfomation).FileName.c_str());
    Printf("INFO: Filename Raw Data: %s", RawDataFileName);



    readFile=fopen(RawDataFileName, "rb");// 2GB File, Binäre Datei ("b") zum Schreiben "r" aufmachen.

    if(readFile == NULL)//Öffnen nicht geklappt, steig aus:
    {
        Printf("ERROR: Could not open Raw data file.");
        exit(1);
    } else {
        Printf("INFO: Raw Data file opened.");
    }

//    unsigned char buffer[BUFFER_SIZE];//unsigned char = 1 byte  in C
    buffer = (unsigned char*) malloc (BUFFER_SIZE+1);
    if (buffer==NULL) {
        fprintf(stderr,"ERROR: Could not allocate buffer via malloc.\n");
        exit (1);
    }

    //Get File Size directly:
    fseek(readFile, 0, SEEK_END); // seek to end of file
    NBytesInRawFile = ftell(readFile); // get current file pointer
    NIntsInRawFile = NBytesInRawFile * sizeof(unsigned char) / sizeof(int);
    printf("INFO: File size in bytes: %u (%d ints)\n",NBytesInRawFile, NIntsInRawFile);
    fseek(readFile, 0, SEEK_SET); // seek back to beginning of file

    return 0;
}

long int ReadRawDataPacket() {
    Printf("INFO: Starting to read from Raw Data file.");
    long readCnt=fread(buffer, sizeof(unsigned char), BUFFER_SIZE, readFile); //lese aus der Datei in die Datei

    if(readCnt!=BUFFER_SIZE) {//Endweder Fehler oder EOF erreicht
        Printf("INFO: Number of characters read not equal to buffer size.");
        if(ferror(readFile)!=0) {//dann ist es Fehler!
            Printf("ERROR: Could not read raw file.");
            exit(1);
        }
    }

    long int NIntsLastRead = 0;
    NIntsLastRead=readCnt*sizeof(unsigned char)/sizeof(int);
    printf("INFO: Number of bytes read: %u, equivalent to %d ints\n", readCnt, NIntsLastRead);

    return NIntsLastRead;
}

int CloseRawFile() {
    int status=fclose(readFile);
    if(status!=0) {//Konnte die datei nicht schließen
        fprintf(stderr,"Konnte die Daten nach dem Lesen nicht schließen!\n");
        exit(1);
    } else {
        Printf("INFO: File closed.");
    }

    free (buffer);

    return 0;
}

