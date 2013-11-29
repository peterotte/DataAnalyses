#include <ConvertionRaw2Units.cc>
#include <ExportTreeFile.cc>
#include <CBClusterFinder.cc>
#include <FillNHitsHistograms.cc>
#include <CBPID_MarkChargedClusters.cc>
#include <PhysicsAnalysis.cc>
#include <NTECBitPatternTest.cc>


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
                     printf("Fehler: Startmarkierung des ersten Event Buffers gefunden?: %x\n",ptrN[i].AsInt);
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

                                         if (LookupTableScaler.at(ptrN[i+bi+ei+si].AsInt).DetectorID != -1) { //Only of defined in config files
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
                                 case 29192: TempEvent.ReferenceRawTDCPbWO = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 410: TempEvent.TAPSCrateHitRegister.at(0) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 420: TempEvent.TAPSCrateHitRegister.at(1) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 430: TempEvent.TAPSCrateHitRegister.at(2) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 440: TempEvent.TAPSCrateHitRegister.at(3) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 450: TempEvent.TAPSCrateHitRegister.at(4) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 460: TempEvent.TAPSCrateHitRegister.at(5) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 470: TempEvent.TAPSCrateHitRegister.at(6) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 480: TempEvent.TAPSCrateHitRegister.at(7) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 490: TempEvent.TAPSCrateHitRegister.at(8) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 411: TempEvent.TAPSCrateHitPattern.at(0) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 421: TempEvent.TAPSCrateHitPattern.at(1) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 431: TempEvent.TAPSCrateHitPattern.at(2) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 441: TempEvent.TAPSCrateHitPattern.at(3) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 451: TempEvent.TAPSCrateHitPattern.at(4) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 461: TempEvent.TAPSCrateHitPattern.at(5) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 471: TempEvent.TAPSCrateHitPattern.at(6) = ptrN[(i+bi+ei)].AsADC.Value; break;
                                 case 481: TempEvent.TAPSCrateHitPattern.at(7) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 491: TempEvent.TAPSCrateHitPattern.at(8) = ptrN[(i+bi+ei)].AsADC.Value; break;

                             case 10010: TempEvent.VUPROMHitPattern.at(0) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10020: TempEvent.VUPROMHitPattern.at(1) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10030: TempEvent.VUPROMHitPattern.at(2) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10040: TempEvent.VUPROMHitPattern.at(3) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10050: TempEvent.VUPROMHitPattern.at(4) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10060: TempEvent.VUPROMHitPattern.at(5) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10070: TempEvent.VUPROMHitPattern.at(6) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10080: TempEvent.VUPROMHitPattern.at(7) = ptrN[(i+bi+ei)].AsADC.Value; break;
                             case 10090: TempEvent.VUPROMHitPattern.at(8) = ptrN[(i+bi+ei)].AsADC.Value; break;




                             case 20020: TempEvent.TAPSCFDHitPatternBit.at(0) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(1) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20021: TempEvent.TAPSCFDHitPatternBit.at(2) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(3) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20052: TempEvent.TAPSCFDHitPatternBit.at(4) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(5) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20053: TempEvent.TAPSCFDHitPatternBit.at(6) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(7) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20084: TempEvent.TAPSCFDHitPatternBit.at(8) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(9) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20085: TempEvent.TAPSCFDHitPatternBit.at(10) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(11) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20116: TempEvent.TAPSCFDHitPatternBit.at(12) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(13) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20117: TempEvent.TAPSCFDHitPatternBit.at(14) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(15) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20148: TempEvent.TAPSCFDHitPatternBit.at(16) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(17) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20149: TempEvent.TAPSCFDHitPatternBit.at(18) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(19) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20180: TempEvent.TAPSCFDHitPatternBit.at(20) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(21) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20181: TempEvent.TAPSCFDHitPatternBit.at(22) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(23) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20212: TempEvent.TAPSCFDHitPatternBit.at(24) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(25) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20213: TempEvent.TAPSCFDHitPatternBit.at(26) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(27) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20244: TempEvent.TAPSCFDHitPatternBit.at(28) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(29) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20245: TempEvent.TAPSCFDHitPatternBit.at(30) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(31) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20276: TempEvent.TAPSCFDHitPatternBit.at(32) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(33) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20277: TempEvent.TAPSCFDHitPatternBit.at(34) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(35) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20308: TempEvent.TAPSCFDHitPatternBit.at(36) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(37) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20309: TempEvent.TAPSCFDHitPatternBit.at(38) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(39) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20340: TempEvent.TAPSCFDHitPatternBit.at(40) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(41) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20341: TempEvent.TAPSCFDHitPatternBit.at(42) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(43) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20372: TempEvent.TAPSCFDHitPatternBit.at(44) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(45) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20373: TempEvent.TAPSCFDHitPatternBit.at(46) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(47) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20404: TempEvent.TAPSCFDHitPatternBit.at(48) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(49) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20405: TempEvent.TAPSCFDHitPatternBit.at(50) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(51) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20436: TempEvent.TAPSCFDHitPatternBit.at(52) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(53) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20437: TempEvent.TAPSCFDHitPatternBit.at(54) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(55) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20468: TempEvent.TAPSCFDHitPatternBit.at(56) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(57) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20469: TempEvent.TAPSCFDHitPatternBit.at(58) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(59) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20500: TempEvent.TAPSCFDHitPatternBit.at(60) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(61) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 20501: TempEvent.TAPSCFDHitPatternBit.at(62) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(63) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;



                             case 21020: TempEvent.TAPSCFDHitPatternBit.at(64) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(65) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21021: TempEvent.TAPSCFDHitPatternBit.at(66) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(67) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21052: TempEvent.TAPSCFDHitPatternBit.at(68) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(69) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21053: TempEvent.TAPSCFDHitPatternBit.at(70) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(71) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21084: TempEvent.TAPSCFDHitPatternBit.at(72) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(73) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21085: TempEvent.TAPSCFDHitPatternBit.at(74) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(75) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21116: TempEvent.TAPSCFDHitPatternBit.at(76) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(77) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21117: TempEvent.TAPSCFDHitPatternBit.at(78) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(79) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21148: TempEvent.TAPSCFDHitPatternBit.at(80) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(81) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21149: TempEvent.TAPSCFDHitPatternBit.at(82) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(83) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21180: TempEvent.TAPSCFDHitPatternBit.at(84) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(85) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21181: TempEvent.TAPSCFDHitPatternBit.at(86) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(87) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21212: TempEvent.TAPSCFDHitPatternBit.at(88) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(89) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21213: TempEvent.TAPSCFDHitPatternBit.at(90) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(91) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21244: TempEvent.TAPSCFDHitPatternBit.at(92) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(93) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21245: TempEvent.TAPSCFDHitPatternBit.at(94) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(95) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21276: TempEvent.TAPSCFDHitPatternBit.at(96) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(97) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21277: TempEvent.TAPSCFDHitPatternBit.at(98) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(99) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21308: TempEvent.TAPSCFDHitPatternBit.at(100) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(101) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21309: TempEvent.TAPSCFDHitPatternBit.at(102) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(103) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21340: TempEvent.TAPSCFDHitPatternBit.at(104) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(105) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21341: TempEvent.TAPSCFDHitPatternBit.at(106) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(107) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21372: TempEvent.TAPSCFDHitPatternBit.at(108) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(109) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21373: TempEvent.TAPSCFDHitPatternBit.at(110) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(111) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21404: TempEvent.TAPSCFDHitPatternBit.at(112) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(113) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21405: TempEvent.TAPSCFDHitPatternBit.at(114) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(115) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21436: TempEvent.TAPSCFDHitPatternBit.at(116) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(117) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21437: TempEvent.TAPSCFDHitPatternBit.at(118) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(119) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21468: TempEvent.TAPSCFDHitPatternBit.at(120) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(121) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21469: TempEvent.TAPSCFDHitPatternBit.at(122) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(123) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21500: TempEvent.TAPSCFDHitPatternBit.at(124) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(125) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 21501: TempEvent.TAPSCFDHitPatternBit.at(126) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(127) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;


                             case 22020: TempEvent.TAPSCFDHitPatternBit.at(128) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(129) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22021: TempEvent.TAPSCFDHitPatternBit.at(130) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(131) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22052: TempEvent.TAPSCFDHitPatternBit.at(132) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(133) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22053: TempEvent.TAPSCFDHitPatternBit.at(134) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(135) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22084: TempEvent.TAPSCFDHitPatternBit.at(136) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(137) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22085: TempEvent.TAPSCFDHitPatternBit.at(138) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(139) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22116: TempEvent.TAPSCFDHitPatternBit.at(140) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(141) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22117: TempEvent.TAPSCFDHitPatternBit.at(142) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(143) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22148: TempEvent.TAPSCFDHitPatternBit.at(144) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(145) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22149: TempEvent.TAPSCFDHitPatternBit.at(146) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(147) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22180: TempEvent.TAPSCFDHitPatternBit.at(148) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(149) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22181: TempEvent.TAPSCFDHitPatternBit.at(150) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(151) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22212: TempEvent.TAPSCFDHitPatternBit.at(152) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(153) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22213: TempEvent.TAPSCFDHitPatternBit.at(154) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(155) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22244: TempEvent.TAPSCFDHitPatternBit.at(156) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(157) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22245: TempEvent.TAPSCFDHitPatternBit.at(158) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(159) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22276: TempEvent.TAPSCFDHitPatternBit.at(160) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(161) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22277: TempEvent.TAPSCFDHitPatternBit.at(162) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(163) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22308: TempEvent.TAPSCFDHitPatternBit.at(164) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(165) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22309: TempEvent.TAPSCFDHitPatternBit.at(166) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(167) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22340: TempEvent.TAPSCFDHitPatternBit.at(168) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(169) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22341: TempEvent.TAPSCFDHitPatternBit.at(170) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(171) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22372: TempEvent.TAPSCFDHitPatternBit.at(172) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(173) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22373: TempEvent.TAPSCFDHitPatternBit.at(174) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(175) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22404: TempEvent.TAPSCFDHitPatternBit.at(176) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(177) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22405: TempEvent.TAPSCFDHitPatternBit.at(178) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(179) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22436: TempEvent.TAPSCFDHitPatternBit.at(180) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(181) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22437: TempEvent.TAPSCFDHitPatternBit.at(182) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(183) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22468: TempEvent.TAPSCFDHitPatternBit.at(184) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(185) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22469: TempEvent.TAPSCFDHitPatternBit.at(186) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(187) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22500: TempEvent.TAPSCFDHitPatternBit.at(188) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(189) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 22501: TempEvent.TAPSCFDHitPatternBit.at(190) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(191) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;




                             case 23020: TempEvent.TAPSCFDHitPatternBit.at(192) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(193) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23021: TempEvent.TAPSCFDHitPatternBit.at(194) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(195) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23052: TempEvent.TAPSCFDHitPatternBit.at(196) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(197) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23053: TempEvent.TAPSCFDHitPatternBit.at(198) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(199) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23084: TempEvent.TAPSCFDHitPatternBit.at(200) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(201) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23085: TempEvent.TAPSCFDHitPatternBit.at(202) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(203) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23116: TempEvent.TAPSCFDHitPatternBit.at(204) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(205) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23117: TempEvent.TAPSCFDHitPatternBit.at(206) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(207) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23148: TempEvent.TAPSCFDHitPatternBit.at(208) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(209) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23149: TempEvent.TAPSCFDHitPatternBit.at(210) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(211) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23180: TempEvent.TAPSCFDHitPatternBit.at(212) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(213) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23181: TempEvent.TAPSCFDHitPatternBit.at(214) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(215) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23212: TempEvent.TAPSCFDHitPatternBit.at(216) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(217) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23213: TempEvent.TAPSCFDHitPatternBit.at(218) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(219) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23244: TempEvent.TAPSCFDHitPatternBit.at(220) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(221) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23245: TempEvent.TAPSCFDHitPatternBit.at(222) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(223) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23276: TempEvent.TAPSCFDHitPatternBit.at(224) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(225) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23277: TempEvent.TAPSCFDHitPatternBit.at(226) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(227) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23308: TempEvent.TAPSCFDHitPatternBit.at(228) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(229) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23309: TempEvent.TAPSCFDHitPatternBit.at(230) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(231) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23340: TempEvent.TAPSCFDHitPatternBit.at(232) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(233) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23341: TempEvent.TAPSCFDHitPatternBit.at(234) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(235) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23372: TempEvent.TAPSCFDHitPatternBit.at(236) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(237) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23373: TempEvent.TAPSCFDHitPatternBit.at(238) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(239) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23404: TempEvent.TAPSCFDHitPatternBit.at(240) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(241) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23405: TempEvent.TAPSCFDHitPatternBit.at(242) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(243) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23436: TempEvent.TAPSCFDHitPatternBit.at(244) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(245) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23437: TempEvent.TAPSCFDHitPatternBit.at(246) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(247) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23468: TempEvent.TAPSCFDHitPatternBit.at(248) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(249) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23469: TempEvent.TAPSCFDHitPatternBit.at(250) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(251) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23500: TempEvent.TAPSCFDHitPatternBit.at(252) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(253) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 23501: TempEvent.TAPSCFDHitPatternBit.at(254) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(255) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;


                             case 24020: TempEvent.TAPSCFDHitPatternBit.at(256) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(257) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24021: TempEvent.TAPSCFDHitPatternBit.at(258) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(259) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24052: TempEvent.TAPSCFDHitPatternBit.at(260) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(261) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24053: TempEvent.TAPSCFDHitPatternBit.at(262) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(263) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24084: TempEvent.TAPSCFDHitPatternBit.at(264) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(265) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24085: TempEvent.TAPSCFDHitPatternBit.at(266) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(267) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24116: TempEvent.TAPSCFDHitPatternBit.at(268) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(269) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24117: TempEvent.TAPSCFDHitPatternBit.at(270) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(271) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24148: TempEvent.TAPSCFDHitPatternBit.at(272) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(273) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24149: TempEvent.TAPSCFDHitPatternBit.at(274) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(275) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24180: TempEvent.TAPSCFDHitPatternBit.at(276) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(277) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24181: TempEvent.TAPSCFDHitPatternBit.at(278) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(279) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24212: TempEvent.TAPSCFDHitPatternBit.at(280) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(281) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24213: TempEvent.TAPSCFDHitPatternBit.at(282) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(283) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24244: TempEvent.TAPSCFDHitPatternBit.at(284) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(285) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24245: TempEvent.TAPSCFDHitPatternBit.at(286) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(287) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24276: TempEvent.TAPSCFDHitPatternBit.at(288) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(289) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24277: TempEvent.TAPSCFDHitPatternBit.at(290) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(291) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24308: TempEvent.TAPSCFDHitPatternBit.at(292) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(293) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24309: TempEvent.TAPSCFDHitPatternBit.at(294) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(295) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24340: TempEvent.TAPSCFDHitPatternBit.at(296) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(297) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24341: TempEvent.TAPSCFDHitPatternBit.at(298) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(299) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24372: TempEvent.TAPSCFDHitPatternBit.at(300) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(301) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24373: TempEvent.TAPSCFDHitPatternBit.at(302) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(303) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24404: TempEvent.TAPSCFDHitPatternBit.at(304) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(305) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24405: TempEvent.TAPSCFDHitPatternBit.at(306) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(307) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24436: TempEvent.TAPSCFDHitPatternBit.at(308) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(309) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24437: TempEvent.TAPSCFDHitPatternBit.at(310) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(311) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24468: TempEvent.TAPSCFDHitPatternBit.at(312) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(313) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24469: TempEvent.TAPSCFDHitPatternBit.at(314) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(315) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24500: TempEvent.TAPSCFDHitPatternBit.at(316) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(317) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 24501: TempEvent.TAPSCFDHitPatternBit.at(318) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(319) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;


                             case 25020: TempEvent.TAPSCFDHitPatternBit.at(320) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(321) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25021: TempEvent.TAPSCFDHitPatternBit.at(322) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(323) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25052: TempEvent.TAPSCFDHitPatternBit.at(324) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(325) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25053: TempEvent.TAPSCFDHitPatternBit.at(326) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(327) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25084: TempEvent.TAPSCFDHitPatternBit.at(328) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(329) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25085: TempEvent.TAPSCFDHitPatternBit.at(330) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(331) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25116: TempEvent.TAPSCFDHitPatternBit.at(332) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(333) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25117: TempEvent.TAPSCFDHitPatternBit.at(334) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(335) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25148: TempEvent.TAPSCFDHitPatternBit.at(336) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(337) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25149: TempEvent.TAPSCFDHitPatternBit.at(338) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(339) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25180: TempEvent.TAPSCFDHitPatternBit.at(340) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(341) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25181: TempEvent.TAPSCFDHitPatternBit.at(342) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(343) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25212: TempEvent.TAPSCFDHitPatternBit.at(344) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(345) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25213: TempEvent.TAPSCFDHitPatternBit.at(346) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(347) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25244: TempEvent.TAPSCFDHitPatternBit.at(348) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(349) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25245: TempEvent.TAPSCFDHitPatternBit.at(350) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(351) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25276: TempEvent.TAPSCFDHitPatternBit.at(352) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(353) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25277: TempEvent.TAPSCFDHitPatternBit.at(354) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(355) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25308: TempEvent.TAPSCFDHitPatternBit.at(356) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(357) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25309: TempEvent.TAPSCFDHitPatternBit.at(358) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(359) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25340: TempEvent.TAPSCFDHitPatternBit.at(360) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(361) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25341: TempEvent.TAPSCFDHitPatternBit.at(362) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(363) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25372: TempEvent.TAPSCFDHitPatternBit.at(364) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(365) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25373: TempEvent.TAPSCFDHitPatternBit.at(366) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(367) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25404: TempEvent.TAPSCFDHitPatternBit.at(368) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(369) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25405: TempEvent.TAPSCFDHitPatternBit.at(370) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(371) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25436: TempEvent.TAPSCFDHitPatternBit.at(372) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(373) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25437: TempEvent.TAPSCFDHitPatternBit.at(374) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(375) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25468: TempEvent.TAPSCFDHitPatternBit.at(376) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(377) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25469: TempEvent.TAPSCFDHitPatternBit.at(378) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(379) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25500: TempEvent.TAPSCFDHitPatternBit.at(380) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(381) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;
                             case 25501: TempEvent.TAPSCFDHitPatternBit.at(382) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSCFDHitPatternBit.at(383) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); break;



                            //TAPS Veto

                             case 26016: TempEvent.TAPSVetoHitPatternBit.at(0) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(1) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(2) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(3) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(4) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(5) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(6) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(7) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26048: TempEvent.TAPSVetoHitPatternBit.at(8) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(9) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(10) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(11) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(12) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(13) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(14) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(15) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26080: TempEvent.TAPSVetoHitPatternBit.at(16) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(17) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(18) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(19) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(20) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(21) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(22) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(23) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26112: TempEvent.TAPSVetoHitPatternBit.at(24) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(25) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(26) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(27) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(28) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(29) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(30) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(31) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26144: TempEvent.TAPSVetoHitPatternBit.at(32) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(33) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(34) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(35) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(36) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(37) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(38) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(39) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26176: TempEvent.TAPSVetoHitPatternBit.at(40) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(41) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(42) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(43) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(44) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(45) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(46) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(47) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26208: TempEvent.TAPSVetoHitPatternBit.at(48) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(49) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(50) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(51) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(52) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(53) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(54) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(55) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26240: TempEvent.TAPSVetoHitPatternBit.at(56) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(57) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(58) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(59) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(60) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(61) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(62) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(63) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26272: TempEvent.TAPSVetoHitPatternBit.at(64) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(65) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(66) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(67) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(68) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(69) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(70) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(71) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26304: TempEvent.TAPSVetoHitPatternBit.at(72) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(73) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(74) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(75) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(76) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(77) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(78) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(79) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26336: TempEvent.TAPSVetoHitPatternBit.at(80) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(81) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(82) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(83) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(84) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(85) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(86) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(87) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26368: TempEvent.TAPSVetoHitPatternBit.at(88) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(89) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(90) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(91) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(92) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(93) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(94) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(95) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26400: TempEvent.TAPSVetoHitPatternBit.at(96) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(97) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(98) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(99) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(100) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(101) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(102) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(103) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26432: TempEvent.TAPSVetoHitPatternBit.at(104) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(105) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(106) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(107) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(108) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(109) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(110) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(111) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26464: TempEvent.TAPSVetoHitPatternBit.at(112) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(113) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(114) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(115) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(116) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(117) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(118) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(119) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 26496: TempEvent.TAPSVetoHitPatternBit.at(120) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(121) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(122) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(123) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(124) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(125) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(126) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(127) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;



                             case 27016: TempEvent.TAPSVetoHitPatternBit.at(128) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(129) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(130) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(131) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(132) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(133) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(134) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(135) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27048: TempEvent.TAPSVetoHitPatternBit.at(136) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(137) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(138) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(139) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(140) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(141) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(142) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(143) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27080: TempEvent.TAPSVetoHitPatternBit.at(144) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(145) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(146) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(147) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(148) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(149) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(150) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(151) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27112: TempEvent.TAPSVetoHitPatternBit.at(152) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(153) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(154) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(155) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(156) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(157) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(158) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(159) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27144: TempEvent.TAPSVetoHitPatternBit.at(160) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(161) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(162) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(163) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(164) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(165) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(166) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(167) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27176: TempEvent.TAPSVetoHitPatternBit.at(168) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(169) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(170) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(171) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(172) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(173) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(174) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(175) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27208: TempEvent.TAPSVetoHitPatternBit.at(176) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(177) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(178) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(179) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(180) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(181) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(182) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(183) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27240: TempEvent.TAPSVetoHitPatternBit.at(184) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(185) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(186) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(187) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(188) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(189) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(190) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(191) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27272: TempEvent.TAPSVetoHitPatternBit.at(192) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(193) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(194) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(195) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(196) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(197) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(198) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(199) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27304: TempEvent.TAPSVetoHitPatternBit.at(200) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(201) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(202) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(203) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(204) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(205) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(206) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(207) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27336: TempEvent.TAPSVetoHitPatternBit.at(208) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(209) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(210) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(211) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(212) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(213) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(214) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(215) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27368: TempEvent.TAPSVetoHitPatternBit.at(216) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(217) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(218) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(219) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(220) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(221) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(222) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(223) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27400: TempEvent.TAPSVetoHitPatternBit.at(224) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(225) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(226) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(227) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(228) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(229) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(230) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(231) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27432: TempEvent.TAPSVetoHitPatternBit.at(232) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(233) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(234) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(235) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(236) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(237) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(238) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(239) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27464: TempEvent.TAPSVetoHitPatternBit.at(240) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(241) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(242) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(243) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(244) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(245) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(246) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(247) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 27496: TempEvent.TAPSVetoHitPatternBit.at(248) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(249) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(250) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(251) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(252) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(253) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(254) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(255) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;

                             case 28016: TempEvent.TAPSVetoHitPatternBit.at(256) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(257) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(258) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(259) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(260) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(261) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(262) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(263) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28048: TempEvent.TAPSVetoHitPatternBit.at(264) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(265) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(266) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(267) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(268) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(269) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(270) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(271) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28080: TempEvent.TAPSVetoHitPatternBit.at(272) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(273) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(274) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(275) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(276) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(277) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(278) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(279) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28112: TempEvent.TAPSVetoHitPatternBit.at(280) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(281) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(282) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(283) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(284) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(285) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(286) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(287) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28144: TempEvent.TAPSVetoHitPatternBit.at(288) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(289) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(290) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(291) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(292) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(293) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(294) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(295) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28176: TempEvent.TAPSVetoHitPatternBit.at(296) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(297) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(298) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(299) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(300) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(301) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(302) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(303) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28208: TempEvent.TAPSVetoHitPatternBit.at(304) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(305) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(306) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(307) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(308) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(309) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(310) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(311) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28240: TempEvent.TAPSVetoHitPatternBit.at(312) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(313) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(314) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(315) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(316) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(317) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(318) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(319) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28272: TempEvent.TAPSVetoHitPatternBit.at(320) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(321) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(322) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(323) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(324) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(325) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(326) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(327) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28304: TempEvent.TAPSVetoHitPatternBit.at(328) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(329) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(330) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(331) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(332) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(333) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(334) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(335) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28336: TempEvent.TAPSVetoHitPatternBit.at(336) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(337) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(338) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(339) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(340) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(341) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(342) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(343) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28368: TempEvent.TAPSVetoHitPatternBit.at(344) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(345) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(346) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(347) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(348) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(349) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(350) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(351) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28400: TempEvent.TAPSVetoHitPatternBit.at(352) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(353) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(354) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(355) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(356) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(357) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(358) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(359) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28432: TempEvent.TAPSVetoHitPatternBit.at(360) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(361) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(362) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(363) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(364) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(365) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(366) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(367) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28464: TempEvent.TAPSVetoHitPatternBit.at(368) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(369) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(370) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(371) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(372) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(373) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(374) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(375) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;
                             case 28496: TempEvent.TAPSVetoHitPatternBit.at(376) = ((ptrN[(i+bi+ei)].AsADC.Value) & 0x1);  TempEvent.TAPSVetoHitPatternBit.at(377) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x2)>>1); TempEvent.TAPSVetoHitPatternBit.at(378) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x4)>>2); TempEvent.TAPSVetoHitPatternBit.at(379) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x8)>>3); TempEvent.TAPSVetoHitPatternBit.at(380) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x10)>>4); TempEvent.TAPSVetoHitPatternBit.at(381) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x20)>>5); TempEvent.TAPSVetoHitPatternBit.at(382) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x40)>>6); TempEvent.TAPSVetoHitPatternBit.at(383) = (((ptrN[(i+bi+ei)].AsADC.Value) & 0x80)>>7); break;









                             }

                             hADCOverview->Fill(ptrN[(i+bi+ei)].AsADC.Ch); //This fills the debug information histo, which ADC is how often used

                             int AktADCDetectorID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).DetectorID;
                             int AktADCElementID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).ElementID;
                             int AktADCTypeID = LookupTableADC.at(ptrN[(i+bi+ei)].AsADC.Ch).TypeID; //0 = ADC, 1 = TDC
                             int AktADCValue = ptrN[(i+bi+ei)].AsADC.Value;
                             int AktADCMultiValue = ptrN[(i+bi+ei+1)].AsADC.Value - ptrN[(i+bi+ei)].AsADC.Value; //Signal - Pedestal

                             //CB ADC: Debug single ADC Sums
                             if ( (AktADCDetectorID == 1) /*CB*/ && (AktADCTypeID == 0) /*ADC information*/ ) {
                                 hCBChADCPart1->Fill(ptrN[(i+bi+ei)].AsADC.Value, AktADCElementID);
                                 hCBChADCPart2->Fill(ptrN[(i+bi+ei+1)].AsADC.Value, AktADCElementID);
                                 hCBChADCPart3->Fill(ptrN[(i+bi+ei+2)].AsADC.Value, AktADCElementID);
                             }

                             //MWPC ADC: Debug single ADC Sums
                             if ( ( (ptrN[(i+bi+ei)].AsADC.Ch >= 5000) && (ptrN[(i+bi+ei)].AsADC.Ch < 5600) ) /*WC ADC*/ ) {
                                 hMWPCChADCPart1->Fill(ptrN[(i+bi+ei)].AsADC.Value, ptrN[(i+bi+ei)].AsADC.Ch-5000);
                                 hMWPCChADCPart2->Fill(ptrN[(i+bi+ei+1)].AsADC.Value, ptrN[(i+bi+ei+1)].AsADC.Ch-5000);
                                 hMWPCChADCPart3->Fill(ptrN[(i+bi+ei+2)].AsADC.Value, ptrN[(i+bi+ei+2)].AsADC.Ch-5000);
                                 ei = ei+2;
                             }

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
                                                 ei = ei+2; //Skip the next two entries because the are already included
                                             } else {
                                                 TempEvent.HitElements.at(HitPresent).RawADC = AktADCValue;
                                             }
                                         } else {
                                             Printf("ERROR: ADC value occured where it is not expected. EventID: %d",AktEventNr );
                                             Printf("       Details: %d %d %d %d", AktADCDetectorID, AktADCElementID, AktADCTypeID, AktADCValue);
                                             printf("       ADC Value already assigned: %d\n",TempEvent.HitElements.at(HitPresent).RawADC);
                                             Printf("       32-bit Raw: %8x  FilePointer: %8x", ptrN[(i+bi+ei)].AsInt, (i+bi+ei)*sizeof(int));
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
                                     case 0: //if ADC info comes
                                         if (AktADCDetectorID == 1) { //Activate Multihit ADC for CB
                                             TempHit.RawADC = AktADCMultiValue;
                                             ei = ei+2; //Skip the next to entries because the are already included
                                         } else {
                                             TempHit.RawADC = AktADCValue;
                                         }
                                         break;
                                     case 1: //if TDC info comes
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
                                 Do_CheckNTECBitPatternTest(); //For Detector Tests: Check the BitPattern from Trigger matches the data in NTEC module
                                 Do_ConstructDetectorHits(); //Use Raw data in EventBlock and apply the calibration to it
                                 Do_CBClusterFinding(); //Use calibrated data from EventBlock and fill EventBlock.Events.at(i).CBClusters
                                 Do_MarkChargedClusters(); //Diesmal mit Clustern und PID Elementen
                                 Do_FillNHitsHistograms();
                                 Do_PhysicsAnalysis();
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

                         //Clear the event block data afer analysis
                         Clear_EventBlock();

                         //Do Speed and Progress measurements
                         float TimeElapsed = ((float)(clock()-Time_LastTime))/CLOCKS_PER_SEC;
                         double TempStatus = (AbsFilePosInt+i)*1./NIntsInRawFile*100.;

                         printf("Progress: %3.1f %%  Speed: Number of events %d in %.1f sec = %.0f events/sec. \r", TempStatus,
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
