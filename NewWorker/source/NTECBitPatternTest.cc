// Do some very basic test, to check whether the BitPattern is send correclty
// The BitPattern tells the CPU, which cards have some data in their buffer

std::vector<int> NTECModulesDataInBuffer;

int Do_CheckNTECBitPatternTest() {
    const int NumberChPerCard = 4;

    //Go through all events in block
    for (int i=0;i < EventBlock.Events.size(); i++) { //Loop through all events
        //For the i-th event
        //Clear the variable NTECModulesDataInBuffer
        NTECModulesDataInBuffer.clear();
        for (int k=0;k<(NBaFElements/NumberChPerCard);k++) { //division by 4 because a NTEC module has 4 channels
            NTECModulesDataInBuffer.push_back(0);
        }

        //Fill the Variable NTECModulesDataInBuffer
        for (int k=0;k < EventBlock.Events.at(i).HitElements.size();k++) { //Goes through all hits in event
            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == DetectorIDBaF) {
                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) { //Look for TDC Hits
                    NTECModulesDataInBuffer.at( floor(AktElementNr / NumberChPerCard) )++;
                }
            }
        }

        if (i < 0) {
            //Check, that NTECModulesDataInBuffer matches with information in EventBlock.Events.TAPSCrateHitPattern
            for (int m=0; m<6; m++) { //loop through all NTEC BaF Modules 0..5
                int TempBufferAccordingToData = 0;
                for (int k=0; k<16; k++) {
                    if (NTECModulesDataInBuffer.at(k+m*16) > 0) {
                        TempBufferAccordingToData += pow(2, k);
                    }
                    //printf("TAPSCrateHitPattern: %d\n", EventBlock.Events.at(i).TAPSCrateHitPattern.at(1) );
                }

                //Check whether the crates BitPattern machtes the data
                int TempTriggerData = EventBlock.Events.at(i).TAPSCrateHitPattern.at(m+1);
                if (TempTriggerData != NoValuePresent) {
                    printf("NTEC BP, EventID: %d\tEvent in Block: %d\tCrate: %d\tTrigger: x%4x\tData: x%4x\tResult: ", EventBlock.Events.at(i).EventID, i, m+1, TempTriggerData, TempBufferAccordingToData);
                } else {
                    printf("NTEC BP, EventID: %d\tEvent in Block: %d\tCrate: %d\tTrigger: no data\tData: x%4x\tResult: ", EventBlock.Events.at(i).EventID, i, m+1, TempBufferAccordingToData);
                }
                if ( TempTriggerData == TempBufferAccordingToData ) {
                    printf("YES\n");
                } else {
                    printf("No\n");
                }
            }

        }
    }
}
