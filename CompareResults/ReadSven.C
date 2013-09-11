int ReadSven() {
	FILE *readFile = NULL;
	Char_t Line[1024];
	Char_t TempStr[1024];
	Char_t Buffer[1024];
	strcpy(TempStr, "XSecDataSven.dat");
	readFile=fopen(TempStr, "r");// text Datei zum Schreiben "r" aufmachen.

	if(readFile == NULL) {
		printf("Could not open analysis config file!\n");
		exit(1);
	} else {
		printf("INFO: file: %s\n", TempStr);
	}

	int AktElementNr = 0;
	while (!feof(readFile)) {
		if (!fgets(Line, sizeof(Line), readFile)) break;
		Buffer[0] = '\0';
		/*
		if (sscanf(Line, "%s %s ", Buffer, Buffer2 ) == 2) {
		if (!strcmp(Buffer, "OutputPath:")) {
		strcpy(OutputDirectory, Buffer2);
		Printf("INFO: OutputPath: %s",OutputDirectory);
		}
		if (!strcmp(Buffer, "InputPathRawData:")) {
		strcpy(InputPathRawData, Buffer2);
		Printf("INFO: InputPathRawData: %s",InputPathRawData);
		}
		} */
	}

	int status=fclose(readFile);
	if(status!=0) {
		printf("ERROR: Could not close file!\n");
		exit(1);
	}

	return 0;
}