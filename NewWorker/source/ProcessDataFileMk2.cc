#include <ConvertionRaw2Units.cc>
#include <ExportTreeFile.cc>
#include <CBClusterFinder.cc>
#include <FillNHitsHistograms.cc>
#include <CBPID_MarkChargedClusters.cc>
#include <PhysicsAnalysis.cc>


int ProcessDataFileMk2() {
    StoredInfo *ptrN=(StoredInfo*)buffer;//caste Buffer zum String
    // unsigned short int *ptrs=(unsigned short int*)buffer;//caste Buffer zum short int

     int AnzahlUngeschlossenerBuffer = 0;
     int AktEventNr = -1;
     int AktEventNrBefore = -1; //The EventNr of the Event before
     int AktEventLength = -1;
     int AnzahlScalerBuffer = 0;
     int AnzahlDateiFehler = 0;
     int AnzahlDateiHeaderFehler = 1;
     int AnzahlDateiFooterFehler = 1;
     int AnzahlBuffer = 0;
     int EventAnzahlImBuffer = 0;
     int AnzahlFehlerBlocks = 0;
     int AnzahlScalerFehlerBlocks = 0;
     int ZustandFehlerImAktEvent = 0; //0 == nein, sonst ja
     int AnzahlEventsMitFehlern = 0;
     int AnzahlEvents = 0;
     int AnzahlEvents_LastTime = 0; //For Speed measurements
     clock_t Time_LastTime = clock(); //For Speed measurements

     EventBlock.Events.reserve(25000); //For Sep 2010 we had 20k per scaler read, now 1k
     EventBlock.Scalers.reserve(1000); //Max number of scaler information per scaler read
     Clear_EventBlock();
     int DiscardActualEventBlock = 0; //if set to -1, then the act EventBlock will not be analysed
     DiscardActualEventBlockDueToCBHits = 0;


     int CancelProcessing = 0;
     long int NBytesLastRead = 0;
     long int AbsFilePosInt = 0;
     do {
         NBytesLastRead = ReadRawDataPacket();
         int i=0;
             int ErrorCounter = 0;
             while ( (i < NBytesLastRead) && (!CancelProcessing) ) {
         //	printf("%d ", ptr[i]);
             if (ptrN[i].AsInt == 0x10101010) { //Header buffer start
                 ErrorCounter = 0;
                 AnzahlDateiHeaderFehler--;
                 printf("\nINFO: Found header buffer.\n");
                 RunNumberFromDataFile = ptrN[(0x2a8/4)].AsInt;
                 Printf("INFO: RunNumberFromDataFile: %d",RunNumberFromDataFile);
                 i=i+BLOCKSIZE4;
                 //if (i<nRawFile) {
                 //	printf("Startmarkierung des ersten Event Buffers erwartet: %x\n",ptr[i]);
                 //}
             }
             else if (ptrN[i].AsInt == 0x30303030) { //End header buffer start
                 ErrorCounter = 0;
                 AnzahlDateiFooterFehler--;
                 printf("Found End Header buffer. \n\n");
                 i=i+BLOCKSIZE4;
                     if (i<NBytesLastRead) {
                     printf("Fehler: start marker of first event buffer found?: %x\n",ptrN[i].AsInt);
                 } else {
                         printf("INFO: Correct: file ended.\n");
                 }
             }
             else if (ptrN[i].AsInt == 0x70707070) { //event buffer start, dann Event Nr, dann Event Länge in bytes
                 AnzahlBuffer++;
                 ErrorCounter = 0;
                 EventAnzahlImBuffer = 0; //Eventzähler per Buffer
                 //printf("Start neuer Buffer mit Nr: %d (%d)\n",AnzahlBuffer,i);

                     /*if ( (i+BLOCKSIZE4) >= NBytesLastRead) {
                         printf("\nERROR: File too short, we are expecting a footer where none is!");
                         break;
                     }*/

                 int bi=1; //Zähler für den Block, startet bei 1 wegen des start markers für den Block
                 while (bi<BLOCKSIZE4) { //Den Buffer durchlaufen
                     //Neues Event beginnt
                     ZustandFehlerImAktEvent = 0;
                     EventAnzahlImBuffer++;
                     AnzahlEvents++;
                     AktEventNr = ptrN[i+bi].AsInt;

                     Clear_TempEvent();
                     TempEvent.EventID = AktEventNr;

                     AktEventLength = (ptrN[i+bi+1].AsInt)/4;
                     //printf("Event: %d   with Length: %d  @ %x\n",AktEventNr,AktEventLength,(i+bi)*4);

                     if (ptrN[i+bi+AktEventLength+1].AsInt != 0xffffffff) { //This should give the End Marker
                         DiscardActualEventBlock = -1;
                         printf("ERROR: Event End Marker wrong! THIS Eventblock will be discarded.\n");
                     }

                     if (AktEventNr-AktEventNrBefore != 1) {
                         DiscardActualEventBlock = -1;
                         printf("WARNING: Event Nr not consecutive! (AnzahlEvents: %d, act: %d, before: %d) THIS EventBlock will be discarded.\n", AnzahlEvents, AktEventNr, AktEventNrBefore);
                     }
                     AktEventNrBefore = AktEventNr;


                     int ei = 2; //Zähler für das Event, ei==0 ist die Event Nr, ei==1 ist die Event Länge
                     while (ei <= AktEventLength) { //Durch das gesamte Event durchgehen
                         if (ptrN[i+bi+ei].AsInt == 0xfefefefe) { //Scaler Block start / stop
                             AnzahlScalerBuffer++;
                             int AktScalerBlockLaenge = 0;
                             AktScalerBlockLaenge = (ptrN[i+bi+ei+1].AsInt)/sizeof(int); //original length is given in bytes
                             if (i+bi+ei+AktScalerBlockLaenge >= NBytesLastRead) {
                                 Printf("ERROR: wrong scaler block length.");
                                 AnzahlDateiFehler++;
                                 ei = ei+AktEventLength; //skip this event
                             } else if ((ptrN[i+bi+ei+AktScalerBlockLaenge+1].AsInt) != 0xfefefefe) { //Check for end marker of scaler block
                                 Printf("ERROR: scaler block end not found.");
                                 AnzahlDateiFehler++;
                                 ei = ei+AktEventLength; //skip this event
                             } else {
                                 int si=2; //Starts with 2, because ptr[i+bi+ei+1] is the length
                                 while (si < AktScalerBlockLaenge) {
                                     if ( (ptrN[i+bi+ei+si].AsInt) == 0xefefefef) {
                                         EventBlock.ScalerErrors++;
                                         AnzahlScalerFehlerBlocks++;
                                         Printf("Error: Error block in scaler read.");
                                         si = si +5;
                                     } else {
                                         hAllScalerAccum->Fill(ptrN[i+bi+ei+si].AsInt, ptrN[i+bi+ei+si+1].AsInt); //This fills the debug information histo, which scaler is how often used
                                         int EventBlockAnzahlScaler = ptrN[i+bi+ei+si].AsInt;
                                         if (RawADCData.ExperimentLiveCounterScalerCh == EventBlockAnzahlScaler) EventBlock.ExperimentLiveCounter = ptrN[i+bi+ei+si+1].AsInt;
                                         if (RawADCData.UngatedLiveCounterScalerCh == EventBlockAnzahlScaler) EventBlock.UngatedLiveCounter = ptrN[i+bi+ei+si+1].AsInt;
                                         if (RawADCData.TaggerLiveCounterScalerCh == EventBlockAnzahlScaler) EventBlock.TaggerLiveCounter = ptrN[i+bi+ei+si+1].AsInt;

                                         if (LookupTableScaler.at(ptrN[i+bi+ei+si].AsInt).DetectorID != -1) { //Only if defined in config files
                                             TScalerRead TempScalerRead;
                                             TempScalerRead.DetectorID = LookupTableScaler.at(ptrN[i+bi+ei+si].AsInt).DetectorID;
                                             TempScalerRead.ElementID = LookupTableScaler.at(ptrN[i+bi+ei+si].AsInt).ElementID;
                                             TempScalerRead.Value = ptrN[i+bi+ei+si+1].AsInt;
                                             EventBlock.Scalers.push_back(TempScalerRead);
                                         }

                                         si = si +2;
                                     }
                                 }

                                 ei = ei + AktScalerBlockLaenge+1; //set the actual pointer to the end of the scaler block
                                            //Check this +1 again
                             }

                         } //Scaler block end
                         else if (ptrN[i+bi+ei].AsInt == 0xfdfdfdfd) { //EPICS Block start
                             int EPICSRecordLength = ptrN[i+bi+ei+12].AsADC.Ch; //This should give the length in bytes of the complete EPICS block (including header and info blocks)
                             //printf("INFO: Length of EPICS block: %d\n",EPICSRecordLength);
                             ei += EPICSRecordLength/sizeof(int)+1;
                             //Basicly: Skip EPICS Block
                         }  //EPICS Block end
                         else if (ptrN[i+bi+ei].AsInt == 0xefefefef) { //Error block start
                             AnzahlFehlerBlocks++;
                             EventBlock.NumberErrorBlocks++;
                             ZustandFehlerImAktEvent = -1;
                             hErrorBlocks->Fill(AnzahlEvents, (ptrN[i+bi+ei+2].AsInt*100+ptrN[i+bi+ei+3].AsInt) );

                           //  Printf("Error block (buffer#: %5u  BufferEvent#: %3u  Event#: %6u): %3x %3x %3x",AnzahlBuffer,EventAnzahlImBuffer,AnzahlEvents, ptr[i+bi+ei+1],ptr[i+bi+ei+2],ptr[i+bi+ei+3]);

                             ei = ei + 4;
                             if (ptrN[i+bi+ei].AsInt != 0xefefefef) { //Fehlerblock nicht terminiert
                                 AnzahlDateiFehler++;
                                 Printf("Fehler: error block not terminated @0x%x.",(i+bi+ei)*4);
                             }

                         } //Error block end

                         else { //Normale Daten
                             switch (ptrN[(i+bi+ei)].AsADC.Ch) {
                                 case 1400: TempEvent.ReferenceRawTDCTagger = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 2000: TempEvent.ReferenceRawTDCCB = ptrN[(i+bi+ei)].AsADC.Value; break;
                             }

                             //hADCOverview->Fill(ptrN[(i+bi+ei)].AsADC.Ch); //This fills the debug information histo, which ADC is how often used

                             int AktADCDetectorID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).DetectorID;
                             int AktADCElementID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).ElementID;
                             int AktADCTypeID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).TypeID; //0 = ADC, 1 = TDC
                             int AktADCValue = ptrN[(i+bi+ei)].AsADC.Value;
                             int AktADCMultiValue = ptrN[(i+bi+ei+1)].AsADC.Value - ptrN[(i+bi+ei)].AsADC.Value; //Signal - Pedestal

                             //CB ADC: Debug single ADC Sums
//                             if ( (AktADCDetectorID == 1) /*CB*/ && (AktADCTypeID == 0) /*ADC information*/ ) {
//                                 hCBChADCPart1->Fill(ptrN[(i+bi+ei)].AsADC.Value, AktADCElementID);
//                                 hCBChADCPart2->Fill(ptrN[(i+bi+ei+1)].AsADC.Value, AktADCElementID);
//                                 hCBChADCPart3->Fill(ptrN[(i+bi+ei+2)].AsADC.Value, AktADCElementID);
//                             }


                             if (AktADCDetectorID != -1) { //Only, if the ADC value is defined by configuration files.
                                 //Now check, whether the element is already represented with some data (ADC/TDC) in this Event(TempEvent)
                                 int HitPresent = -1; //If elem. already has an hit, then hit number gets saved
                                 for (int TempE = 0; ( (TempE < TempEvent.HitElements.size()) && (HitPresent == -1) );TempE++) {
                                     if ( (TempEvent.HitElements.at(TempE).ElementID == AktADCElementID) && (TempEvent.HitElements.at(TempE).DetectorID == AktADCDetectorID) ) {
                                         HitPresent = TempE; //Some information is already present, save hit number
                                     }
                                 }

                                 if (HitPresent >= 0) { //Information about this elem does already exist
                                     switch (AktADCTypeID) {
                                     case ADCTypeIDADC: //ADC
                                         if (TempEvent.HitElements.at(HitPresent).RawADC == NoValuePresent) {
                                             //Add ADC information to existing TDC information
                                             if (TempEvent.HitElements.at(HitPresent).DetectorID == DetectorIDCB) { //Activate Multihit ADC for CB
                                                 TempEvent.HitElements.at(HitPresent).RawADC = AktADCMultiValue;

                                                 //Check for correct Dataformat
                                                 if ( (ptrN[(i+bi+ei)].AsADC.Ch != ptrN[(i+bi+ei+1)].AsADC.Ch) ||
                                                      (ptrN[(i+bi+ei)].AsADC.Ch != ptrN[(i+bi+ei+2)].AsADC.Ch) ) {
                                                     printf("ERROR: MultiHit ADC Index wrong: %d %d %d\n",ptrN[(i+bi+ei)].AsADC.Ch, ptrN[(i+bi+ei+1)].AsADC.Ch, ptrN[(i+bi+ei+2)].AsADC.Ch);
                                                 }
                                                 ei = ei+2; //Skip the next two entries because the are already included
                                             } else {
                                                 TempEvent.HitElements.at(HitPresent).RawADC = AktADCValue;
                                             }
                                         } else {
                                             Printf("ERROR: ADC value occured where it is not expected. EventID: %d",AktEventNr );
                                             Printf("       Details: %d %d %d %d", AktADCDetectorID, AktADCElementID, AktADCTypeID, AktADCValue);
                                             printf("       ADC Value already assigned: %d\n",TempEvent.HitElements.at(HitPresent).RawADC);
                                             Printf("       32-bit Raw: %8x  FilePointer: %8x", ptrN[(i+bi+ei)].AsInt, (AbsFilePosInt+i+bi+ei)*sizeof(int));
                                             AnzahlDateiFehler++;
                                         }
                                         break;
                                     case ADCTypeIDTDC: //TDC
                                         TempEvent.HitElements.at(HitPresent).RawTDC.push_back(AktADCValue); //Add TDC information to existing ADC value
                                         break;
                                     default: Printf("Error during analysis. Unknown ADCType.");
                                     }
                                 }


                                 //If no information found, then do...
                                 if (HitPresent == -1) {
                                     THitElement TempHit;
                                     TempHit.DetectorID = AktADCDetectorID;
                                     TempHit.ElementID = AktADCElementID;
                                     TempHit.RawTDC.clear();
                                     TempHit.RawADC = NoValuePresent;
                                     TempHit.Time.clear();
                                     TempHit.Energy = NoValuePresent;
                                     TempHit.ParticipatingClusterID = -1;
                                     switch (AktADCTypeID) {
                                     case ADCTypeIDADC: //if ADC info comes
                                         if (AktADCDetectorID == 1) { //Activate Multihit ADC for CB
                                             //Check for correct Dataformat
                                             if ( (ptrN[(i+bi+ei)].AsADC.Ch != ptrN[(i+bi+ei+1)].AsADC.Ch) ||
                                                  (ptrN[(i+bi+ei)].AsADC.Ch != ptrN[(i+bi+ei+2)].AsADC.Ch) ) {
                                                 printf("ERROR: MultiHit ADC Index wrong: %d %d %d\n",ptrN[(i+bi+ei)].AsADC.Ch, ptrN[(i+bi+ei+1)].AsADC.Ch, ptrN[(i+bi+ei+2)].AsADC.Ch);
                                             }

                                             TempHit.RawADC = AktADCMultiValue;
                                             ei = ei+2; //Skip the next to entries because the are already included
                                         } else {
                                             TempHit.RawADC = AktADCValue;
                                         }
                                         break;
                                     case ADCTypeIDTDC: //if TDC info comes
                                         TempHit.RawTDC.push_back(AktADCValue);
                                         break;
                                     default: Printf("Error during analysis.");
                                         AnzahlDateiFehler++;
                                     }

                                     TempEvent.HitElements.push_back(TempHit);
                                 }
                             }


                         }
                         ei++;
                     }

                     //Here is the end of the event, it is completely read

                     EventBlock.Events.push_back(TempEvent);
                     Clear_TempEvent();


                     //if a scaler block is now included in the event data block, then analyse the data now
                     if (EventBlock.Scalers.size() > 0) {

                         if (DiscardActualEventBlock == 0) {
                             if (!ProgramOnlyFileCheckMode) {
                                 //TThread::Lock();
                                 Do_ConstructDetectorHits(); //Use Raw data in EventBlock and apply the calibration to it
                                 if (!DiscardActualEventBlockDueToCBHits) { //If Hits in CB are okay
                                     Do_CBClusterFinding(); //Use calibrated data from EventBlock and fill EventBlock.Events.at(i).CBClusters
                                     Do_MarkChargedClusters(); //Diesmal mit Clustern und PID Elementen
                                     Do_FillNHitsHistograms();
                                     Do_PhysicsAnalysis();
                                 }
                                 //TThread::UnLock();
                             }
                             //if (AnzahlScalerBuffer > 30) {
                             if (SigIntCalled) {
                                printf("WARNING: Analysis cancelled by user. Will end now.\n");
                                //i = i+NBytesLastRead; //Quickly Abort analysis
                                CancelProcessing = -1;
                             }
                             //gSystem->ProcessEvents();
                         }
                         DiscardActualEventBlock = 0;
                         DiscardActualEventBlockDueToCBHits = 0;

                         //Clear the event block data afer analysis
                         Clear_EventBlock();

                         //Do Speed and Progress measurements
                         float TimeElapsed = ((float)(clock()-Time_LastTime))/CLOCKS_PER_SEC;
                         double TempStatus = (AbsFilePosInt+i)*1./NIntsInRawFile*100.;

                         printf("Progress: %3.1f %%  Speed: Number of events %d in %.1f sec = %.0f events/sec. \n", TempStatus,
                                (AnzahlEvents-AnzahlEvents_LastTime), TimeElapsed, (AnzahlEvents-AnzahlEvents_LastTime)/TimeElapsed);
                         fflush(stdout);

                         AnzahlEvents_LastTime = AnzahlEvents; //For Speed measurements
                         Time_LastTime = clock();
                     }

                     bi=bi+AktEventLength+2;
                     if ( ptrN[i+bi].AsInt == 0xffffffff )  { //End of Buffer
                         bi = bi+BLOCKSIZE4;
                         //printf("info: End of Buffer\n");
                     }

                     //In diesem Event aufgetretene Fehler aufsummieren
                     if (ZustandFehlerImAktEvent != 0) 	AnzahlEventsMitFehlern++;

                 }
                 i=i+BLOCKSIZE4;

             } else {
                 if (ErrorCounter<5) {
                    printf("ERROR: Error in file, scanning for next buffer!\n");
                 }
                 i++;
                 ErrorCounter++;
                 AnzahlDateiFehler++;
             }
         }

        AbsFilePosInt += i;
        printf("\nINFO: End of Sequence, read new part of raw file.\n");
     } while ( (NBytesLastRead > 0) && (!CancelProcessing) );

     printf("\n\n");
     printf("----------------------------------------------\nFile Summary:\n");
     printf("INFO: Number File Errors: %d\n", AnzahlDateiFehler);
     printf("INFO: Number FileHeader Errors: %d\n", AnzahlDateiHeaderFehler);
     printf("INFO: Number FileFooter Errors: %d\n",AnzahlDateiFooterFehler);
     printf("INFO: Number Buffers: %d\n",AnzahlBuffer);
     printf("INFO: Number Scaler Buffer Markers: %d\n",AnzahlScalerBuffer);
     printf("INFO: Number Events: %d\n", AnzahlEvents);
     printf("INFO: Number Unclosed Buffer: %d\n",AnzahlUngeschlossenerBuffer);
     printf("INFO: Number ErrorBlocks: %d\n",AnzahlFehlerBlocks);
     Printf("INFO: Number ScalerErrorBlocks: %d", AnzahlScalerFehlerBlocks);
     printf("INFO: Number Events with Errors: %d (%.2f\%)\n",AnzahlEventsMitFehlern, AnzahlEventsMitFehlern*1./AktEventNr*100.);


     printf("\nFinished\n");

}
