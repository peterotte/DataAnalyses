#include <GeneralConstants.h>
#include <TMath.h>
#include <CBNeighbours.h>
#include <TVector3.h>



int Do_CBClusterFinding() {
    int i=0;
    while (i < EventBlock.Events.size()) { //Loop through all events
        //Printf("\n== Event start: Nr %d =============================", EventBlock.Events.at(i).EventID);

        std::vector<double> CBHits(NCBElements,0); //Stores the Energy of each crystal
        std::vector<int> CBElementID2HitElementID(NCBElements,-1); //Stores the Hit ID
        int k=0;
        while (k < EventBlock.Events.at(i).HitElements.size()) { //Goes through all hits in event
            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
            int AktDetectorNr = EventBlock.Events.at(i).HitElements.at(k).DetectorID;

            if (AktDetectorNr == 1) { //For CB only
                //printf("%d: %3.1f\t", AktElementNr, EventBlock.Events.at(i).HitElements.at(k).Energy);
                CBHits.at(AktElementNr) = EventBlock.Events.at(i).HitElements.at(k).Energy;
                CBElementID2HitElementID.at(AktElementNr) = k;
            }
            k++;
        }
        //printf("\n");


        int AktClusterID = 0;
        int AktGefElementID = -1;

        do {
            //Search for the element with highest energy
            AktGefElementID = -1;
            double AktGefMaxE = -1;
            k=0;
            while (k < EventBlock.Events.at(i).HitElements.size()) { //Goes through all hits in event
                int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;
                int AktDetectorNr = EventBlock.Events.at(i).HitElements.at(k).DetectorID;

                if (AktDetectorNr == 1) { //For CB only
                    if ( EventBlock.Events.at(i).HitElements.at(k).ParticipatingClusterID == -1 ) { //Not yet used in Cluster
                        if ( EventBlock.Events.at(i).HitElements.at(k).Energy >= 5 ) { //Mindestenergie
                            if (EventBlock.Events.at(i).HitElements.at(k).Energy > AktGefMaxE) {
                                AktGefElementID = AktElementNr;
                                AktGefMaxE = EventBlock.Events.at(i).HitElements.at(k).Energy;
                            }
                        }
                    }
                }
                k++;
            }

            //printf("CLUSTER: Höchste E in Elem ID: %d mit E: %f\n",AktGefElementID, AktGefMaxE);

            double AktClusterESum = AktGefMaxE;
            if (AktGefElementID >= 0) { //Something found
                TVector3 vSum;
                int AktNumberOfCrystals = 0;
                EventBlock.Events.at(i).HitElements.at( CBElementID2HitElementID.at(AktGefElementID) ).ParticipatingClusterID = AktClusterID;
                ///printf("CLUSTER: highest E in Elem ID: %d with E: %f\n",AktGefElementID, AktGefMaxE);

                vSum.SetX(1);
                vSum.SetPhi(RawADCData.CB.Elements.at(AktGefElementID).Position.phi);
                vSum.SetTheta(RawADCData.CB.Elements.at(AktGefElementID).Position.theta);
                vSum.SetMag(RawADCData.CB.Elements.at(AktGefElementID).Position.r);
                vSum = vSum * AktClusterESum;

                for (int l=0;l<12;l++) {
                    ///Printf("CLUSTER: %d mit %f",CBClustering_N1[AktGefElementID][l], CBHits.at( CBClustering_N1[AktGefElementID][l]) );
                    //Printf("Info: %d %d -> %d", AktGefElementID, CBClustering_N1[AktGefElementID][l], CBElementID2HitElementID.at(CBClustering_N1[AktGefElementID][l]) );
                    if (CBElementID2HitElementID.at(CBClustering_N1[AktGefElementID][l]) != -1) { //If Element is in Hit included
                        if (EventBlock.Events.at(i).HitElements.at( CBElementID2HitElementID.at(CBClustering_N1[AktGefElementID][l]) ).ParticipatingClusterID == -1) { //Check, that Element is not included in Sum or somewhere else yet
                            TVector3 vTemp;
                            vTemp.SetX(1);
                            vTemp.SetPhi(RawADCData.CB.Elements.at(CBClustering_N1[AktGefElementID][l]).Position.phi);
                            vTemp.SetTheta(RawADCData.CB.Elements.at(CBClustering_N1[AktGefElementID][l]).Position.theta);
                            vTemp.SetMag(RawADCData.CB.Elements.at(CBClustering_N1[AktGefElementID][l]).Position.r);
                            vTemp = vTemp * CBHits.at( CBClustering_N1[AktGefElementID][l]);
                            vSum = vSum + vTemp ;

                            AktClusterESum = AktClusterESum + CBHits.at( CBClustering_N1[AktGefElementID][l] );
                            EventBlock.Events.at(i).HitElements.at( CBElementID2HitElementID.at(CBClustering_N1[AktGefElementID][l]) ).ParticipatingClusterID = AktClusterID;
                            AktNumberOfCrystals++;
                        }
                    }
                }
                vSum = vSum * (1./AktClusterESum);
                //vSum.SetMag(45.4);

                if (AktClusterESum >= ClusterMinEnergy) { //Check for Cluster Threshold
                    for (int n=0; n<EventBlock.Events.at(i).HitElements.at( CBElementID2HitElementID.at(AktGefElementID) ).Time.size();n++ ) {
                        TCBCluster TempCluster;
                        TempCluster.Time = EventBlock.Events.at(i).HitElements.at( CBElementID2HitElementID.at(AktGefElementID) ).Time.at(n);
                        TempCluster.Energy = AktClusterESum;
                        TempCluster.CentralElementID = AktGefElementID;
                        TempCluster.NumberOfCrystals = AktNumberOfCrystals;
                        //TempCluster.Position = RawADCData.CB.Elements.at(AktGefElementID).Position; //Use this line, if the Cluster position should be the position of the element with highest Eeeeeee
                        TempCluster.IsCharged = 0; //uncharged

                        //Set the momentum direction to the weighted mean
                        TempCluster.Position.phi = vSum.Phi();
                        TempCluster.Position.theta  = vSum.Theta();
                        TempCluster.Position.r = AktClusterESum;


                       // printf("Centre: %d phi: %f th: %f r: %f \t phi: %f th: %f r: %f \n", AktGefElementID, RawADCData.CB.Elements.at(AktGefElementID).Position.phi,
                         //   RawADCData.CB.Elements.at(AktGefElementID).Position.theta, RawADCData.CB.Elements.at(AktGefElementID).Position.r,
                           //    vSum.Phi(), vSum.Theta(), vSum.Mag());

                        EventBlock.Events.at(i).CBClusters.push_back(TempCluster);
                        ///printf("CLUSTER: ClusterID: %d E: %.1f\n", AktClusterID, AktClusterESum);
                    }
                    AktClusterID++;

                    CBClusterEnergy_VS_CBNElemCluster->Fill(AktClusterESum, AktNumberOfCrystals);
                }
            }

        } while ( AktGefElementID >= 0 ); //loop terminates, if condition is fulfilled
        CBNCluster->Fill(AktClusterID); //Fill the Number of Clusters


        i++;
    }


    return 0;

}

