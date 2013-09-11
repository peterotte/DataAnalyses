#include <Worker.h>
#include <TMath.h>


int Do_MarkChargedClusters() {
    int i = 0;
    while (i<EventBlock.Events.size()) { //Loop through all events in EventBlock

        int k=0;
        while (k<EventBlock.Events.at(i).CBClusters.size()) { //Loop through all Clusters in CB

            for (int m=0;m < EventBlock.Events.at(i).HitElements.size();m++) { //Goes through all hits in event
                int AktElementNr2 = EventBlock.Events.at(i).HitElements.at(m).ElementID;
                int AktDetectorNr2 = EventBlock.Events.at(i).HitElements.at(m).DetectorID;

                if ( (AktDetectorNr2 == 2) ) {
                    PIDCorrelation->Fill(
                                EventBlock.Events.at(i).CBClusters.at(k).Position.phi * 180/3.141,
                                RawADCData.PID.Elements.at(AktElementNr2).Position.phi * 180/3.141
                                );

                    double TempDifference = (EventBlock.Events.at(i).CBClusters.at(k).Position.phi-RawADCData.PID.Elements.at(AktElementNr2).Position.phi) * 180/TMath::Pi();
                    if (TempDifference > 180) TempDifference = TempDifference - 360;
                    if (TempDifference < -180) TempDifference = TempDifference + 360;

                    hCB_DeltaPhi->Fill( TempDifference, AktElementNr2 );

                    //Mark Clusters as charged
                    if ( TMath::Abs(TempDifference) <= PIDCBPhiDelta ) { //if the difference between the two phi angles is small, then mark the cluster it as charged
                        EventBlock.Events.at(i).CBClusters.at(k).IsCharged = -1;
//                        hCB_DeltaPhi->Fill( TempDifference );
                    }

                }
            }

            k++;
        }

        i++;
    }
}
