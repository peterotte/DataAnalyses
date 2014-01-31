#include <GeneralConstants.h>
#include <TMath.h>

int Do_ConstructDetectorHits() {
    for (int i=0;i < EventBlock.Events.size(); i++) { //Loop through all events
        //Printf("\n== Event start: Nr %d =============================", EventBlock.Events.at(i).EventID);


        for (int k=0;k < EventBlock.Events.at(i).HitElements.size();k++) { //Goes through all hits in event
          //  Printf("Detector: %4d Element: %4d ADC: %5d TDC: %5d", EventBlock.Events.at(i).HitElements.at(k).DetectorID, EventBlock.Events.at(i).HitElements.at(k).ElementID,
          //         EventBlock.Events.at(i).HitElements.at(k).RawADC, (EventBlock.Events.at(i).HitElements.at(k).RawTDC-EventBlock.Events.at(i).ReferenceRawTDCTagger));

            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;

            //Tagger
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == DetectorIDTagger) {
                hTaggerNMultiHits->Fill(EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(), AktElementNr);

                //printf("Referenz: %d\n", EventBlock.Events.at(i).ReferenceRawTDCTagger);

                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) {
                    double TempResult = ( EventBlock.Events.at(i).HitElements.at(k).RawTDC.at(l)-EventBlock.Events.at(i).ReferenceRawTDCTagger );
                    if (TempResult < -50000) TempResult = TempResult + 62054; //Überlauf der CATCH TDCs
                    if (TempResult > 50000) TempResult = TempResult - 62054; //Überlauf der CATCH TDCs

                    //printf("TempResult: %f \n",TempResult);
                    hTaggerChTDC->Fill(TempResult, AktElementNr );

                    TempResult = (-1*RawADCData.Tagger.Elements.at(AktElementNr).TDCOffsetTicks + TempResult)*RawADCData.Tagger.Elements.at(AktElementNr).TDCNSPerTick +
                            RawADCData.Tagger.TimeOffsetNS;

                    //Do Time Cuts
                    int TempFill = -1;
                    if (CutHitsOutOfRange) {
                        if ( (TempResult < RawADCData.Tagger.Elements.at(AktElementNr).TDCLowThresholdNS) || (TempResult > RawADCData.Tagger.Elements.at(AktElementNr).TDCHighThresholdNS) ) {
                            TempFill = 0;
                        }
                    }
                    if (TempFill) {
                        EventBlock.Events.at(i).HitElements.at(k).Time.push_back(TempResult);
                        //hTaggerTime->Fill( TempResult, AktElementNr );
                    }
                }
            }

            //CB
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == DetectorIDCB) {
                hCBNMultiHits->Fill(EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(), AktElementNr);

                //Handle ADC information
                hCBChADC->Fill( EventBlock.Events.at(i).HitElements.at(k).RawADC, AktElementNr );
                double TempEnergy = (EventBlock.Events.at(i).HitElements.at(k).RawADC - RawADCData.CB.Elements.at(AktElementNr).ADCPedestalTicks) * RawADCData.CB.Elements.at(AktElementNr).ADCMEVPerTick;
                TempEnergy = TempEnergy * GlobalCBEnergyScale * RunsMetaInformation.at(IndexRunMetaInfomation).CBEnergyScale;

                //Do Energy Cuts
                int TempEFill = -1;
                if (CutHitsOutOfRange) {
                    if ( (TempEnergy < RawADCData.CB.Elements.at(AktElementNr).ADCLowThresholdMEV) || (TempEnergy > RawADCData.CB.Elements.at(AktElementNr).ADCHighThresholdMEV) ) {
                        TempEFill = 0;
                    }
                }
                if (TempEFill) {
                    EventBlock.Events.at(i).HitElements.at(k).Energy = TempEnergy;
                    //hCBChEnergy->Fill( TempEnergy, AktElementNr );
                    hCBADCHits_VS_EventID->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                }

                //Handle TDC information
                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) {
                    double TempResult = ( EventBlock.Events.at(i).HitElements.at(k).RawTDC.at(l)-EventBlock.Events.at(i).ReferenceRawTDCCB );
                    if (TempResult < -50000) TempResult = TempResult + 62054; //Überlauf der CATCH TDCs
                    if (TempResult > 50000) TempResult = TempResult - 62054; //Überlauf der CATCH TDCs

                    hCBChTDC->Fill( TempResult, AktElementNr );

                    TempResult = (-1*RawADCData.CB.Elements.at(AktElementNr).TDCOffsetTicks + TempResult)*RawADCData.CB.Elements.at(AktElementNr).TDCNSPerTick +
                            RawADCData.CB.TimeOffsetNS;;

                    //Now Timewalk correction
                    //formula: t_corr(t,E) = t- RiseTime + (Threshold / (E+Shift)^Power)
                    double TempTDCResultCorrected = TempResult - (  RawADCData.CB.Elements.at(AktElementNr).TimeWalkParameter.RiseTime +
                                                                    RawADCData.CB.Elements.at(AktElementNr).TimeWalkParameter.Threshold/
                                                                    TMath::Power(TempEnergy + RawADCData.CB.Elements.at(AktElementNr).TimeWalkParameter.Shift,
                                                                                 RawADCData.CB.Elements.at(AktElementNr).TimeWalkParameter.Power) );

                    //Do Time Cuts
                    int TempFill = -1;
                    if (CutHitsOutOfRange) {
                        if ( (TempTDCResultCorrected < RawADCData.CB.Elements.at(AktElementNr).TDCLowThresholdNS) || (TempTDCResultCorrected > RawADCData.CB.Elements.at(AktElementNr).TDCHighThresholdNS) ) {
                            TempFill = 0;
                        }
                    }
                    if (TempFill) {
                        EventBlock.Events.at(i).HitElements.at(k).Time.push_back(TempTDCResultCorrected);
                        hCBTDCHits_VS_EventID->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                        //hCBTime->Fill( TempTDCResultCorrected, AktElementNr );
                    }

                }
            }

            //PID
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == DetectorIDPID) {
                hPIDNMultiHits->Fill(EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(), AktElementNr);

                //Handle ADC information
                hPIDChADC->Fill( EventBlock.Events.at(i).HitElements.at(k).RawADC, AktElementNr ); //Also entries with no ADC information (NoValuePresent) are filled

                if (EventBlock.Events.at(i).HitElements.at(k).RawTDC.size() > 0) {
                    hPIDChADCCutTDCRequired->Fill( EventBlock.Events.at(i).HitElements.at(k).RawADC, AktElementNr );
                }
                double TempEnergy = (EventBlock.Events.at(i).HitElements.at(k).RawADC - RawADCData.PID.Elements.at(AktElementNr).ADCPedestalTicks) * RawADCData.PID.Elements.at(AktElementNr).ADCMEVPerTick;
                hPIDChEnergyUncut->Fill( TempEnergy, AktElementNr );

                //Do Energy Cuts
                int TempEFill = -1;
                if (CutHitsOutOfRange) {
                    if ( (TempEnergy < RawADCData.PID.Elements.at(AktElementNr).ADCLowThresholdMEV) || (TempEnergy > RawADCData.PID.Elements.at(AktElementNr).ADCHighThresholdMEV) ) {
                        TempEFill = 0;
                    }
                }
                if (TempEFill) {
                    EventBlock.Events.at(i).HitElements.at(k).Energy = TempEnergy;
                    //hPIDChEnergy->Fill( TempEnergy, AktElementNr );
                }


                //Handle TDC information
                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) {
                    double TempResult = ( EventBlock.Events.at(i).HitElements.at(k).RawTDC.at(l)-EventBlock.Events.at(i).ReferenceRawTDCCB );
                    if (TempResult < -50000) TempResult = TempResult + 62054; //Überlauf der CATCH TDCs
                    if (TempResult > 50000) TempResult = TempResult - 62054; //Überlauf der CATCH TDCs

                    hPIDChTDC->Fill(TempResult, AktElementNr );

                    TempResult = (-1*RawADCData.PID.Elements.at(AktElementNr).TDCOffsetTicks + TempResult)*RawADCData.PID.Elements.at(AktElementNr).TDCNSPerTick;

                    //Do Time Cuts
                    int TempFill = -1;
                    if (CutHitsOutOfRange) {
                        if ( (TempResult < RawADCData.PID.Elements.at(AktElementNr).TDCLowThresholdNS) || (TempResult > RawADCData.PID.Elements.at(AktElementNr).TDCHighThresholdNS) ) {
                            TempFill = 0;
                        }
                    }
                    if (TempFill) {
                        EventBlock.Events.at(i).HitElements.at(k).Time.push_back(TempResult);
                        //hPIDTime->Fill( TempResult, AktElementNr );
                    }

                }
            }

        }
    }

    std::vector<int> CBSectorHits(4,0);

    //Now delete events, which have no Time or Energy information left after cuts
    int i=0;
    while (i < EventBlock.Events.size()) { //Loop through all events
        //Printf("\n== Event start: Nr %d =============================", EventBlock.Events.at(i).EventID);

        int k=0;
        while (k < EventBlock.Events.at(i).HitElements.size()) { //Goes through all hits in event
          //  Printf("Detector: %4d Element: %4d ADC: %5d TDC: %5d", EventBlock.Events.at(i).HitElements.at(k).DetectorID, EventBlock.Events.at(i).HitElements.at(k).ElementID,
          //         EventBlock.Events.at(i).HitElements.at(k).RawADC, (EventBlock.Events.at(i).HitElements.at(k).RawTDC-EventBlock.Events.at(i).ReferenceRawTDCTagger));

            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
            int AktDetectorNr = EventBlock.Events.at(i).HitElements.at(k).DetectorID;

            int DeleteHit = 0; //Default: Do not delete the hit
            //Only if Time and Energy cuts are enabled
            if (CutHitsOutOfRange) {
                if ( (AktDetectorNr == 1) || (AktDetectorNr == 2) ) { //require ADC only for CB and PID
                    if (EventBlock.Events.at(i).HitElements.at(k).Energy == NoValuePresent ) {
                        DeleteHit = -1;
                        //hCBDeletedHits_VS_EventID_DueEnergy->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                    }
                }
                if ( (AktDetectorNr == 0) || (AktDetectorNr == 1) ) { //require TDC only for Tagger and CB
                    if (EventBlock.Events.at(i).HitElements.at(k).Time.size() == 0 ) { //Require an TDC hit
                        DeleteHit = -1;
                        //hCBDeletedHits_VS_EventID_DueTime->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                    }
                }
            }

            if (DeleteHit) {
                //Printf("Hit %d from Detector %d has no Energy or Time information. Delete it.", AktElementNr, AktDetectorNr);
                EventBlock.Events.at(i).HitElements.erase(EventBlock.Events.at(i).HitElements.begin() + k);
            } else {
                switch (AktDetectorNr) {
                case 0: //Tagger
                    for (int l=0;l<EventBlock.Events.at(i).HitElements.at(k).Time.size();l++) {
                        hTaggerTime->Fill( EventBlock.Events.at(i).HitElements.at(k).Time.at(l), AktElementNr );
                    }
                    break;
                case 1: //CB
                    if (EventBlock.Events.at(i).HitElements.at(k).Energy != NoValuePresent) {
                        hCBChEnergy->Fill( EventBlock.Events.at(i).HitElements.at(k).Energy, AktElementNr );
                    }
                    for (int l=0;l<EventBlock.Events.at(i).HitElements.at(k).Time.size();l++) {
                        hCBHits_VS_EventID->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                        hCBTime->Fill( EventBlock.Events.at(i).HitElements.at(k).Time.at(l), AktElementNr );

                        CBSectorHits.at((int)floor(AktElementNr/(720./4.)))++;
                    }
                    break;
                case 2: //PID
                    if (EventBlock.Events.at(i).HitElements.at(k).Energy != NoValuePresent) {
                        hPIDChEnergy->Fill( EventBlock.Events.at(i).HitElements.at(k).Energy, AktElementNr );
                    }
                    for (int l=0;l<EventBlock.Events.at(i).HitElements.at(k).Time.size();l++) {
                        hPIDTime->Fill( EventBlock.Events.at(i).HitElements.at(k).Time.at(l), AktElementNr );
                    }
                    break;
                default: printf("ERROR: unexpected DetectorID\n");
                }

                k++;
            }
        }

        i++;
    }


//    printf("CBSectorHits: %8d %8d %8d %8d\n",CBSectorHits.at(0), CBSectorHits.at(1), CBSectorHits.at(2), CBSectorHits.at(3));
    for (i=0;i<4;i++) {
        if (CBSectorHits.at(i) < RequireMinHitsCBBlock) {
            DiscardActualEventBlockDueToCBHits = -1;
            printf("CBSectorHits: %8d %8d %8d %8d\n",CBSectorHits.at(0), CBSectorHits.at(1), CBSectorHits.at(2), CBSectorHits.at(3));
            printf("WARNING: This EventBlock will be discarded tue to low hits in CB. Section %d has only %d hits.\n", i, CBSectorHits.at(i));
        }
    }

    return 0;
}

