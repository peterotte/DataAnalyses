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
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == 0) {
                hTaggerNMultiHits->Fill(EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(), AktElementNr);

                for (int l=0; l<EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(); l++ ) {
                    double TempResult = ( EventBlock.Events.at(i).HitElements.at(k).RawTDC.at(l)-EventBlock.Events.at(i).ReferenceRawTDCTagger );
                    if (TempResult < -50000) TempResult = TempResult + 62054; //Überlauf der CATCH TDCs
                    if (TempResult > 50000) TempResult = TempResult - 62054; //Überlauf der CATCH TDCs

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
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == 1) {
                hCBNMultiHits->Fill(EventBlock.Events.at(i).HitElements.at(k).RawTDC.size(), AktElementNr);

                //Handle ADC information
                hCBChADC->Fill( EventBlock.Events.at(i).HitElements.at(k).RawADC, AktElementNr );
                double TempEnergy = (EventBlock.Events.at(i).HitElements.at(k).RawADC - RawADCData.CB.Elements.at(AktElementNr).ADCPedestalTicks) * RawADCData.CB.Elements.at(AktElementNr).ADCMEVPerTick;
                TempEnergy = TempEnergy * CBEnergyScale;

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
            if (EventBlock.Events.at(i).HitElements.at(k).DetectorID == 2) {
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




    //Do some checks for these missing Blocks in CB start
/*    int i=0;
    std::vector<int> ADCHitVectorMinus4(NCBElements,0);
    std::vector<int> ADCHitVectorMinus3(NCBElements,0);
    std::vector<int> ADCHitVectorMinus2(NCBElements,0);
    std::vector<int> ADCHitVectorMinus1(NCBElements,0);
    std::vector<int> TDCHitVectorMinus4(NCBElements,0);
    std::vector<int> TDCHitVectorMinus3(NCBElements,0);
    std::vector<int> TDCHitVectorMinus2(NCBElements,0);
    std::vector<int> TDCHitVectorMinus1(NCBElements,0);
    while (i < EventBlock.Events.size()) { //Loop through all events
        std::vector<int> TDCHitVector(NCBElements,0);
        std::vector<int> ADCHitVector(NCBElements,0);

        int TDCHitVectorCount = 0;
        int k=0;
        while (k < EventBlock.Events.at(i).HitElements.size()) { //Goes through all hits in event
            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
            int AktDetectorNr = EventBlock.Events.at(i).HitElements.at(k).DetectorID;

            if (AktDetectorNr == 1) { //For CB
                //if ( (AktElementNr > 360) && (AktElementNr < 544) ) {
                {
                    if (EventBlock.Events.at(i).HitElements.at(k).Energy != NoValuePresent) {
                        ADCHitVector.at(k) = -1;
                    }
                    if (EventBlock.Events.at(i).HitElements.at(k).Time.size() > 0) {
                        TDCHitVector.at(k) = -1;
                        TDCHitVectorCount++;
                    }
                }
            }
            k++;
        }

        k=0;
        double CommonCounter0 = 0;
        while(k<NCBElements) {
            if ( (TDCHitVector.at(k)) && (ADCHitVector.at(k)) ) {
                CommonCounter0++;
            }
            k++;
        }
        double CommonCounter1 = 0;
        k=0;
        while(k<NCBElements) {
            if ( (TDCHitVector.at(k)) && (ADCHitVectorMinus3.at(k)) ) {
                CommonCounter1++;
            }
            k++;
        }
        double CommonCounter2 = 0;
        k=0;
        while(k<NCBElements) {
            if ( (TDCHitVector.at(k)) && (ADCHitVectorMinus4.at(k)) ) {
                CommonCounter2++;
            }
            k++;
        }


        double CommonCounterP1 = 0;
        k=0;
        while(k<NCBElements) {
            if ( (TDCHitVectorMinus3.at(k)) && (ADCHitVector.at(k)) ) CommonCounterP1++;
            k++;
        }
        double CommonCounterP2 = 0;
        k=0;
        while(k<NCBElements) {
            if ( (TDCHitVectorMinus4.at(k)) && (ADCHitVector.at(k)) ) CommonCounterP2++;
            k++;
        }

        CommonCounter0 = CommonCounter0 / TDCHitVectorCount;
        CommonCounter1 = CommonCounter1 / TDCHitVectorCount;
        CommonCounter2 = CommonCounter2 / TDCHitVectorCount;
        CommonCounterP1 = CommonCounterP1 / TDCHitVectorCount;
        CommonCounterP2 = CommonCounterP2 / TDCHitVectorCount;
        double meanCommonCounter =
                (0.*CommonCounter0 + 1.*CommonCounter1 + 2.*CommonCounter2 + (-1.)*CommonCounterP1 + (-2.)*CommonCounterP2) /
                (CommonCounter0 + CommonCounter1 + CommonCounter2 + CommonCounterP1 + CommonCounterP2);
        hCommonCounter_VS_EventID->Fill(EventBlock.Events.at(i).EventID, meanCommonCounter);

        ADCHitVectorMinus4 = ADCHitVectorMinus3;
        ADCHitVectorMinus3 = ADCHitVectorMinus2;
        ADCHitVectorMinus2 = ADCHitVectorMinus1;
        ADCHitVectorMinus1 = ADCHitVector;
        TDCHitVectorMinus4 = TDCHitVectorMinus3;
        TDCHitVectorMinus3 = TDCHitVectorMinus2;
        TDCHitVectorMinus2 = TDCHitVectorMinus1;
        TDCHitVectorMinus1 = TDCHitVector;

        i++;
    }

*/
    //End CB Missing Blocks stop







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
                    hCBChEnergy->Fill( EventBlock.Events.at(i).HitElements.at(k).Energy, AktElementNr );
                    for (int l=0;l<EventBlock.Events.at(i).HitElements.at(k).Time.size();l++) {
                        hCBHits_VS_EventID->Fill(EventBlock.Events.at(i).EventID, AktElementNr);
                        hCBTime->Fill( EventBlock.Events.at(i).HitElements.at(k).Time.at(l), AktElementNr );
                    }
                    break;
                case 2: //PID
                    hPIDChEnergy->Fill( EventBlock.Events.at(i).HitElements.at(k).Energy, AktElementNr );
                    for (int l=0;l<EventBlock.Events.at(i).HitElements.at(k).Time.size();l++) {
                        hPIDTime->Fill( EventBlock.Events.at(i).HitElements.at(k).Time.at(l), AktElementNr );
                    }
                    break;
                default: printf("ERROR: unexpected DetectorID\n");
                }



  /*              // PID Alignment check between CB and PID
                  // For raw hits. In Do_MarkChargedClusters there is the same routine, just
                  // with clusters in CB
                if (AktDetectorNr == 1) {
                    for (int m=0;m < EventBlock.Events.at(i).HitElements.size();m++) { //Goes through all hits in event
                        int AktElementNr2 = EventBlock.Events.at(i).HitElements.at(m).ElementID;
                        int AktDetectorNr2 = EventBlock.Events.at(i).HitElements.at(m).DetectorID;

                        if ( (AktDetectorNr2 == 2) ) {
                            PIDCorrelation->Fill( RawADCData.CB.Elements.at(AktElementNr).Position.phi * 180/3.141, RawADCData.PID.Elements.at(AktElementNr2).Position.phi * 180/3.141 );
                        }
                    }

                }
*/

                k++;
            }
        }

        i++;
    }




    return 0;
}



