#include "TProcessor.h"
#include "TRint.h"
#include "TThread.h"
#include "TRandom3.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


#define CORES 16

TProcessor* Proc[CORES];
TThread*    Work[CORES];

//-----------------------------------------------------------------------------

void StartThread(Int_t Number)
{
	Proc[Number]->Run();
}

//-----------------------------------------------------------------------------

int SigIntCalled = 0; //To catch CTRL-C
void sigfunc(int sig) {
    if (sig!=SIGINT) {
        return;
    } else {
        printf("Called CTRL-C.\n");
        SigIntCalled--;
    }
}

//-----------------------------------------------------------------------------

int main(int argc, char **argv) {
	//Register Signal handler function
	signal(SIGINT, sigfunc);

	Bool_t Busy;
	Char_t Buffer[256];
	Char_t Line[1024];
	Int_t StartNumber = 0;
	Int_t ActNumber  = -1;
	Int_t EndNumber = 1600;
	
	system("mkdir logs");
	
	//Handle command line options
	Int_t CountNumbers = 0;
	Char_t TempStr[256];
	Int_t TempNumber = 0;
	for (int i=1; i< argc; i++) {
		if (strncmp("--", argv[i], 1)) {
			strcpy(TempStr, argv[i]);
			sscanf(TempStr, "%d", &TempNumber);
			switch (i) {
				case 1:	StartNumber = TempNumber; break;
				case 2: EndNumber = TempNumber; break;
			}
			TempNumber++;
		} else {
			//if (!strcmp("-b", argv[i])) ProgramBatchMode = -1;
		}
	}

	printf("Analyse file from %d to %d.\n", StartNumber, EndNumber);

	//Create worker threads for all cores
	for(Int_t i=0; i<CORES; i++) {
		sprintf(Buffer, "Processor%d", i);
		Proc[i] = new TProcessor(Buffer);
		Proc[i]->SetNumber(i);
		Proc[i]->SetRunning(false);
		Work[i] = new TThread("ProcessorThread", (void(*)(void*))&(StartThread), (void*)(i), TThread::kNormalPriority);
		//Delete old log files
		sprintf(Buffer, "logs/Thread%dLog.txt", i);
		unlink(Buffer);
	}

	ActNumber = StartNumber;
	while (ActNumber <= EndNumber) {
		for(Int_t i=0; i<CORES; i++) {
			if (!(Proc[i]->GetRunning())) {
				//worker i is free
				//if ActNumber <= EndNumber then start working on a new file
				if (ActNumber <= EndNumber) {
					Proc[i]->SetRunning(true);
					Proc[i]->SetNumber(ActNumber);

					Work[i]->Run(); //Start Analysis process
					printf("Processing file number %d on thread %2d\n", ActNumber, i);

					sleep(10); //Some delay to prevent starting all processes at the same time)
					ActNumber++;
				}
			}
		}		
		sleep(1); //Go to sleep before checking again for free threads
		if (SigIntCalled < 0) ActNumber = EndNumber+1;
	}

	//When all files have been distributed, wait for running threads to be finished
	printf("Waiting for all jobs to become complete.\n");
	Busy = true;
	while(Busy) {
		Busy = false;
		for(Int_t i=0; i<CORES; i++) {
			Busy = (Busy || Proc[i]->GetRunning());
		}
		sleep(1);
	}

	printf("Finished\n");
	return 0;
}
