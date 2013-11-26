// Do some very basic test, to check whether the BitPattern is send correclty
// The BitPattern tells the CPU, which cards have some data in their buffer


int Do_CheckNTECBitPatternTest() {
    const int NumberChPerCardBaF = 4;
    const int NumberChPerCardTAPSVeto = 8;
    std::vector<int> NTECModulesDataInBuffer; //Number of Hits per NTEC Module

    //Go through all events in block
    for (int i=0;i < EventBlock.Events.size(); i++) { //Loop through all events
        //For the i-th event
        //Clear the variable NTECModulesDataInBuffer
        NTECModulesDataInBuffer.clear();
        for (int k=0;k<(NBaFElements/NumberChPerCardBaF + NTAPSVetoElements/NumberChPerCardTAPSVeto);k++) { //division by 4 because a NTEC module has 4 channels
            NTECModulesDataInBuffer.push_back(0);
        }

        //Fill the Variable NTECModulesDataInBuffer
        for (int k=0;k < EventBlock.Events.at(i).HitElements.size();k++) { //Goes through all hits in event
            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
            switch (EventBlock.Events.at(i).HitElements.at(k).DetectorID) {
            case DetectorIDBaF:
                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) { //Look for TDC Hits
                    NTECModulesDataInBuffer.at( floor(AktElementNr / NumberChPerCardBaF) )++;
                }
                break;
            case DetectorIDTAPSVeto:
                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) { //Look for TDC Hits
                    NTECModulesDataInBuffer.at( floor(AktElementNr / NumberChPerCardTAPSVeto) + NBaFElements/NumberChPerCardBaF )++;
                }
                break;

            }
        }

        //Check, that NTECModulesDataInBuffer matches with information in EventBlock.Events.TAPSCrateHitPattern
        for (int m=0; m<9; m++) { //loop through all NTEC BaF Modules 0..5 and TAPS Veto 7..9
            //Check for existence of BP Register Status
            int TempTriggerReg = EventBlock.Events.at(i).TAPSCrateHitRegister.at(m);
            int BPReceived = 0;

            //Fill Bits 0 and 1
            if (TempTriggerReg != NoValuePresent) { //Some BP has been received, whether in error or not is not yet clear
                BPReceived = -1;
                hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, 0+17*5*m); //Bit0: Some BitPattern received
            } else {
                hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, 1+17*5*m); //Bit1: No BitPattern received
            }

            //Fill Bits 2 and 3
            TempTriggerReg = (TempTriggerReg & 0x1f00); //Check only the Register for the Bitpattern from Trigger
            int BPCorrectlyReceived = 0;
            if (BPReceived) {
                if (TempTriggerReg == 0x1a00) { //Test whether received without error
                    BPCorrectlyReceived = -1;
                    hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, 2+17*5*m); //Bit2: Received: OK
                } else {
                    hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, 3+17*5*m); //Bit3: Received: Error
                }
            }


            //Check whether the crates BitPattern matches the data
            int TempTriggerData = EventBlock.Events.at(i).TAPSCrateHitPattern.at(m);
            if (TempTriggerData == NoValuePresent) {
                hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, 4+17*5*m); //Bit4: No Data in file about this trigger decision
                TempTriggerData = 0xffff; //Data not in data stream, set to some default. Whether this is correct or not...
            }
            for (int k=0; k<16; k++) { //Loop through all NTEC Modules
                int ResultData = 0;
                int ResultTrigger = 0;
                int ResultSum = 0;
                if (NTECModulesDataInBuffer.at(k+m*16) > 0) { //Test whether number of hits per NTEC Module is >0
                    ResultData = -1;
                    ResultSum += 1;
                }
                if ((TempTriggerData>>k)&0x1) { //Test whether Trigger said, there is data in the Module
                    ResultTrigger = -1;
                    ResultSum += 2;
                }

                //ResultSum = 0 : No data in module and no from trigger
                //            1 :    Data in module and no from trigger <-- this may never happen!!
                //            2 : No data in module and    from trigger <-- should be reduced
                //            3 :    Data in module and    from trigger
                hNTECModulesBitPatternTest->Fill(EventBlock.Events.at(i).EventID, ResultSum+5+5*k+17*5*m);

                if (ResultSum == 1) {

                    //Fill the Variable NTECModulesDataInBuffer
                    for (int q=0;q < EventBlock.Events.at(i).HitElements.size();q++) { //Goes through all hits in event
                        int AktElementNr = EventBlock.Events.at(i).HitElements.at(q).ElementID;
                        switch (EventBlock.Events.at(i).HitElements.at(q).DetectorID) {
                        case DetectorIDBaF:
                            if ( (floor(AktElementNr/4)==(k+m*16)) ) {
                                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(q).RawTDC.size(); l++ ) { //Look for TDC Hits
                                    hBaFChTDCError->Fill(EventBlock.Events.at(i).HitElements.at(q).RawTDC.at(l), AktElementNr);
                                }
                            }
                            //printf("BaF Error, ElementNr: %d\n", AktElementNr);
                            break;
                        case DetectorIDTAPSVeto:
                            if ( (floor(AktElementNr/8)==(k+(m-6)*16)) ) {
                                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(q).RawTDC.size(); l++ ) { //Look for TDC Hits
                                    hTAPSVetoChTDCError->Fill(EventBlock.Events.at(i).HitElements.at(q).RawTDC.at(l), AktElementNr);
                                }
                            }
                            break;

                        }
                    }
                }
            }

        }

    }
}
