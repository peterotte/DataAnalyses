#include <TLorentzVector.h>
#include <GeneralConstants.h>
#include "TA2KFParticle.h"
#include "TA2CBKinematicFitter.h"

int SetParticleMass(TLorentzVector *lv, double fMass) {
    double E, P;
    E = lv->E() - lv->M() + fMass;
    P = TMath::Sqrt( E*E - fMass*fMass );

    lv->SetE(E);
    lv->SetVect(lv->Vect().Unit() * P);
}

//For actuel EventBlock
unsigned int GetExperimentLiveCounter() {
    return EventBlock.ExperimentLiveCounter;
}
unsigned int GetUngatedLiveCounter() {
    return EventBlock.UngatedLiveCounter;
}
unsigned int GetTaggerLiveCounter() {
    return EventBlock.TaggerLiveCounter;
}

double GetExpLiveTimeRatio() {
    return (1.*GetExperimentLiveCounter() / (1.*GetUngatedLiveCounter()));
}

//For complete Run, accumulated
double GetExperimentLiveCounterAcc() {
    return hLiveTimeAccum->GetBinContent(2);
}
double GetUngatedLiveCounterAcc() { //Skipp Events with Dropped Events
    return hLiveTimeAccum->GetBinContent(1);
}
double GetUngatedLiveCounterAllEventsAcc() { //Also Events with Dropped Events
    return hLiveTimeAccum->GetBinContent(4);
}
double GetTaggerLiveCounterAcc() {
    return hLiveTimeAccum->GetBinContent(3);
}

double GetExpLiveTimeAccRatio() {
    return ((1.*GetExperimentLiveCounterAcc()) / (1.*GetUngatedLiveCounterAcc()));
}
double GetTaggerLiveTimeAccRatio() {
    return ((1.*GetTaggerLiveCounterAcc()) / (1.*GetUngatedLiveCounterAcc()));
}
double GetNotDroppedEventsRatio() {
    return ((1.*GetUngatedLiveCounterAcc()) / (1.*GetUngatedLiveCounterAllEventsAcc()));
}
//-------------------------------------------------------------------------------


int Handle_ScalerCounts(int fNRejectedEvents, int fNTotalEvents) {
    if (LastScalerReadNCh != EventBlock.Scalers.size()) {
        //Show this warning only after the first EventBlock
        if (LastScalerReadNCh >= 0) Printf("ERROR: Number of Scalers differs to last EventBlock. %d %d", LastScalerReadNCh, EventBlock.Scalers.size());
    }
    LastScalerReadNCh = EventBlock.Scalers.size();

    int i = 0;
    int NScalerBlocksUsed = EventBlock.BlockID;
    double TaggerLT = ((double)GetTaggerLiveCounter() / (double)GetUngatedLiveCounter() );
    double ExpLT = ((double)GetExperimentLiveCounter() / (double)GetUngatedLiveCounter() );
    while (i<EventBlock.Scalers.size()) {
        if (EventBlock.Scalers.at(i).DetectorID == 0) {
            hTaggerScalerAccum->Fill(EventBlock.Scalers.at(i).ElementID, EventBlock.Scalers.at(i).Value);
            hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, EventBlock.Scalers.at(i).ElementID+1, EventBlock.Scalers.at(i).Value/TaggerLT*ExpLT);
        } else if (EventBlock.Scalers.at(i).DetectorID == 10) {
            hTriggerScalerAccum->Fill(EventBlock.Scalers.at(i).ElementID, EventBlock.Scalers.at(i).Value);
        }
        i++;
    }
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 300, TaggerLT);
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 301, ExpLT);
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 302, GetTaggerLiveCounter());
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 303, GetExperimentLiveCounter());
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 304, GetUngatedLiveCounter());
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 305, fNRejectedEvents);
    hTaggerScalerAccum_VS_EventID->SetBinContent(NScalerBlocksUsed+1, 306, fNTotalEvents);

    //NScalerBlocksUsed++;
    double CorrectionFactor = (1.-(double)fNRejectedEvents/(double)fNTotalEvents);
    hLiveTimeAccum->Fill(0.5, GetUngatedLiveCounter() * CorrectionFactor); //Ungated
    hLiveTimeAccum->Fill(1.5, GetExperimentLiveCounter() * CorrectionFactor ) ; //CB Gated, corrected by the dropped Events = dead time
    hLiveTimeAccum->Fill(2.5, GetTaggerLiveCounter() * CorrectionFactor ); //Tagger Gated
    hLiveTimeAccum->Fill(3.5, GetUngatedLiveCounter() ); //Ungated, not corrected by Dropped Events

    if (CorrectionFactor < 1.0) printf("LiveTime corrected by: %f\n", CorrectionFactor);

    hDroppedEvents->Fill(0.5, fNRejectedEvents);
    hDroppedEvents->Fill(1.5, fNTotalEvents);
}

double GetAngleBetweenTwoVectors(TPosition vec1, TPosition vec2) {
    return TMath::ACos(TMath::Cos(vec1.theta)*TMath::Cos(vec2.theta)+TMath::Cos(vec1.phi-vec2.phi)*TMath::Sin(vec1.theta)*TMath::Sin(vec2.theta));
}

int Do_PhysicsAnalysis () {
    TVector3 MyBoostVector;

    //Check for wrong Tagger Scaler reads START
    int i = 0;
    double TaggTotalNScalerHits = 0;
    while (i<EventBlock.Scalers.size()) {
        if (EventBlock.Scalers.at(i).DetectorID == 0) { //Tagger
            TaggTotalNScalerHits += EventBlock.Scalers.at(i).Value;
        }
        i++;
    }

    //printf("INFO: Number of Tagger-Hits registered by Scalers: %f Avg: %f\n",TaggTotalNScalerHits, AvgSumPerEventBlockCountedByScalers);
    if (AvgSumPerEventBlockCountedByScalers >= 0.1) {
        if ( (TaggTotalNScalerHits <= (AvgSumPerEventBlockCountedByScalers*0.8) ) || (TaggTotalNScalerHits >= (AvgSumPerEventBlockCountedByScalers*1.2) ) ) {
            printf("WARNING: Tagger scalers are off by factor %f from average. This event block will be dropped.\n", TaggTotalNScalerHits / AvgSumPerEventBlockCountedByScalers );
            return 1;
        }
    }
    //Check for wrong Tagger Scaler reads END

    //Check for wrong Trigger Scaler reads START
    int SomeThingWrong = 0;
    double TaggerLT = ((double)GetTaggerLiveCounter() / (double)GetUngatedLiveCounter() );
    double ExpLT = ((double)GetExperimentLiveCounter() / (double)GetUngatedLiveCounter() );
    if ( (TaggerLT <= 0.2) || (TaggerLT >=1.) ) SomeThingWrong = -1;
    if ( (ExpLT <= 0.2) || (ExpLT >=1.) ) SomeThingWrong = -1;
    if (SomeThingWrong) {
        printf("WARNING: Experiment LT scalers (%f %f) are wrong. This event block will be dropped.\n",  TaggerLT, ExpLT);
        return 2;
    }
    //Check for wrong Trigger Scaler reads END

    int NTotalEvents = 0;
    int NRejectedEvents = 0; //Due to wrong MAMI Bitpattern etc.

    i = 0;
    while (i<EventBlock.Events.size()) { //Loop through all events in EventBlock
        TEvent MyEvent;
        MyEvent = EventBlock.Events.at(i);

        //printf("New Event ID: %d\tTotalN:%d\n", MyEvent.EventID, MyEvent.CBClusters.size());

        //Check Angle between all Clusters START
        int k=0;
        while (k<MyEvent.CBClusters.size()) {
            int n=k+1;
            while (n<MyEvent.CBClusters.size()) {
                if (TMath::Abs(MyEvent.CBClusters.at(k).Time - MyEvent.CBClusters.at(n).Time) < 20) {
                    hAngleBetweenClusters->Fill(
                            GetAngleBetweenTwoVectors(MyEvent.CBClusters.at(k).Position, MyEvent.CBClusters.at(n).Position)*180/TMath::Pi(),
                            MyEvent.CBClusters.at(k).Energy / MyEvent.CBClusters.at(n).Energy
                            );
//                    if (GetAngleBetweenTwoVectors(MyEvent.CBClusters.at(k).Position, MyEvent.CBClusters.at(n).Position)*180/TMath::Pi() < 40.) {
//                        hEnergyPhoton1VSEnergyPhoton2_Prompt->Fill(MyEvent.CBClusters.at(k).Energy, MyEvent.CBClusters.at(n).Energy);
//                        hEnergyPhoton1VSEnergyPhoton2_Prompt->Fill(MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(n).NumberOfCrystals);

/*                        if ((k==0) && (n==1) ){
                            printf("\n");
                            for (int o=0;o<MyEvent.CBClusters.size();o++) {
                                printf("EventID. %d NSize: %d\t %d   %d  %d    %f  %f\n",
                                    MyEvent.EventID, MyEvent.CBClusters.size(), MyEvent.CBClusters.at(o).CentralElementID,
                                       MyEvent.CBClusters.at(o).NumberOfCrystals, MyEvent.CBClusters.at(o).ClusterID,
                                       MyEvent.CBClusters.at(o).Time, MyEvent.CBClusters.at(o).Energy );
                            }
                            int kk=0;
                            while (kk < EventBlock.Events.at(i).HitElements.size()) { //Goes through all hits in event
                                int AktElementNr = EventBlock.Events.at(i).HitElements.at(kk).ElementID;
                                int AktDetectorNr = EventBlock.Events.at(i).HitElements.at(kk).DetectorID;

                                if (AktDetectorNr == 1) { //For CB only
                                    printf("%d: %3d %3.1f\t", AktElementNr, EventBlock.Events.at(i).HitElements.at(kk).ParticipatingClusterID, EventBlock.Events.at(i).HitElements.at(kk).Energy);
                                }
                                kk++;
                            }

                        }
*/    /*                    printf("EventID. %d NSize: %d\tCentral Crystal: %d %d   %d %d   Multi: %d %d   t: %f %f  E: %f %f\n",
                                 MyEvent.EventID, MyEvent.CBClusters.size(),
                                 k, n,
                                 MyEvent.CBClusters.at(k).CentralElementID, MyEvent.CBClusters.at(n).CentralElementID,
                               MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(n).NumberOfCrystals,
                                 MyEvent.CBClusters.at(k).Time, MyEvent.CBClusters.at(n).Time,
                                MyEvent.CBClusters.at(k).Energy, MyEvent.CBClusters.at(n).Energy ) ;
      */
//                  }
                }

                n++;
            }
            k++;
        }
        //Check Angle between all clusters END

        //** Start hCBEnergySumAllEvents
        double CBEnergySum = 0;

        k=0;
        while (k<MyEvent.CBClusters.size()){
            CBEnergySum = CBEnergySum + MyEvent.CBClusters.at(k).Energy;
            int TempPartID = 22;
            if (MyEvent.CBClusters.at(k).IsCharged) TempPartID = 2212;
           /// printf("ID: %d\tphi: %f\tth: %f\tp: %f\tE: %f\n", TempPartID, MyEvent.CBClusters.at(k).Position.phi*180/TMath::Pi(), MyEvent.CBClusters.at(k).Position.theta*180/TMath::Pi(),
              ///     MyEvent.CBClusters.at(k).Position.r, MyEvent.CBClusters.at(k).Energy );

            if (MyEvent.CBClusters.at(k).IsCharged) {
                hCBClusterMulti_VS_Energy_PIDCharged->Fill(MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(k).Energy);
            } else {
                hCBClusterMulti_VS_Energy_PIDUncharged->Fill(MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(k).Energy);
            }


//            if (MyEvent.CBClusters.at(k).IsCharged) { //PID included:
            if (MyEvent.CBClusters.at(k).NumberOfCrystals<3) { //PID out
                MyEvent.CBClusters.erase(MyEvent.CBClusters.begin() + k);
            } else {
                k++;
            }

        }
        hCBEnergySumAllEvents->Fill(CBEnergySum);

        //** End hCBEnergySumAllEvents

        //Do not analyse Events with Error Blocks if Mk2 DataFormat
        NTotalEvents++;
        if (RawDataFormat == 2) {
            if (MyEvent.NErrorBlocks > 0) {
                //printf("WARNING: EventID: %d has NErrorBlocks: %d\n", MyEvent.EventID, MyEvent.NErrorBlocks);
                NRejectedEvents++;
                i++;
                continue;
                printf("This SHOULD NEVER HAPPEN!\n");
            }
        }

        std::vector<TLorentzVector> IncomingPhotons;
        std::vector<double> IncomingPhotonsTime;
        std::vector<Int_t> IncomingPhotonsTaggerCh;
        std::vector<TLorentzVector> IncomingPhotonsNotPrompt;
        std::vector<double> IncomingPhotonsNotPromptTime;
        std::vector<Int_t> IncomingPhotonsNotPromptTaggerCh;
        for (int g=0;g<MyEvent.HitElements.size();g++) {
            if (MyEvent.HitElements.at(g).DetectorID == 0) { //If Tagger
                for (int h=0;h<MyEvent.HitElements.at(g).Time.size();h++) {
                    double PhotonEnergy = RawADCData.Tagger.Elements.at(MyEvent.HitElements.at(g).ElementID).CentralEnergyMEV;
                    PhotonEnergy = ElectronBeamEnergy - PhotonEnergy;
                    double PhotonTime = MyEvent.HitElements.at(g).Time.at(h);
/*                    if (TMath::Abs(PhotonTime) <= 10) {
                        IncomingPhotons.push_back(TLorentzVector(0,0,PhotonEnergy,PhotonEnergy));
                        IncomingPhotonsTime.push_back(PhotonTime);
                        IncomingPhotonsTaggerCh.push_back(MyEvent.HitElements.at(g).ElementID);
                    } else if ( (PhotonTime < -100) || (PhotonTime > 100) ) {
                        IncomingPhotonsNotPrompt.push_back(TLorentzVector(0,0,PhotonEnergy,PhotonEnergy));
                        IncomingPhotonsNotPromptTime.push_back(PhotonTime);
                        IncomingPhotonsNotPromptTaggerCh.push_back(MyEvent.HitElements.at(g).ElementID);
                    }
*/                  if (TMath::Abs(PhotonTime) <= 150) {  //Time calculation relative to Meson will be done later
                        IncomingPhotons.push_back(TLorentzVector(0,0,PhotonEnergy,PhotonEnergy));
                        IncomingPhotonsTime.push_back(PhotonTime);
                        IncomingPhotonsTaggerCh.push_back(MyEvent.HitElements.at(g).ElementID);
                    }
                }
            }
        }
/*
        double CBEnergySum = 0;

        //Only Uncharged Particles, please
        int k=0;

        while (k<MyEvent.CBClusters.size()){
            CBEnergySum = CBEnergySum + MyEvent.CBClusters.at(k).Energy;
            int TempPartID = 22;
            if (MyEvent.CBClusters.at(k).IsCharged) TempPartID = 2212;
           /// printf("ID: %d\tphi: %f\tth: %f\tp: %f\tE: %f\n", TempPartID, MyEvent.CBClusters.at(k).Position.phi*180/TMath::Pi(), MyEvent.CBClusters.at(k).Position.theta*180/TMath::Pi(),
              ///     MyEvent.CBClusters.at(k).Position.r, MyEvent.CBClusters.at(k).Energy );

            if (MyEvent.CBClusters.at(k).IsCharged) {
                MyEvent.CBClusters.erase(MyEvent.CBClusters.begin() + k);
            } else {
                k++;
            }
        }*/
        hCBEnergySum->Fill(CBEnergySum);
        hCBEnergySum_VS_EventID->Fill(MyEvent.EventID, CBEnergySum);


        hNPhotons->Fill(MyEvent.CBClusters.size());

        if (MyEvent.CBClusters.size() > 1) { //All Events with any number of Photons
        //if (MyEvent.CBClusters.size() == 2) { //Only Events with two Photons
            k=0;
            while (k<MyEvent.CBClusters.size()){
                int l=k+1;
                while (l<MyEvent.CBClusters.size()){
                    double DeltaTimeMesonGammas = MyEvent.CBClusters.at(k).Time - MyEvent.CBClusters.at(l).Time;
                    double MesonTime = (MyEvent.CBClusters.at(k).Time + MyEvent.CBClusters.at(l).Time)/2.;
                    hCB2GammaDeltaTime->Fill(DeltaTimeMesonGammas);
                    if ( TMath::Abs(DeltaTimeMesonGammas) <= 30 ) { //Require, that both Gammas are correlated
                        TLorentzVector v1;
                        TLorentzVector v2;
                        TLorentzVector vSum;

                        v1.SetX(1);
                        v2.SetY(1);

                        v1.SetPhi(MyEvent.CBClusters.at(k).Position.phi);
                        v1.SetTheta(MyEvent.CBClusters.at(k).Position.theta);
                        v1.SetRho(MyEvent.CBClusters.at(k).Energy);
                        v1.SetE(MyEvent.CBClusters.at(k).Energy);

                        v2.SetPhi(MyEvent.CBClusters.at(l).Position.phi);
                        v2.SetTheta(MyEvent.CBClusters.at(l).Position.theta);
                        v2.SetRho(MyEvent.CBClusters.at(l).Energy);
                        v2.SetE(MyEvent.CBClusters.at(l).Energy);

                        vSum = v1 + v2;
                        TLorentzVector vSumBeforeFit = vSum;

                     //   printf("C1: %i (%f)\t C2: %i(%f) =\t%f\n", MyEvent.CBClusters.at(0).CentralElementID, MyEvent.CBClusters.at(0).Energy,
                       //        MyEvent.CBClusters.at(1).CentralElementID, MyEvent.CBClusters.at(1).Energy, vSum.M());

                        hMesonInvariantMass->Fill(vSum.M());
                        hInvariant2GammaMassVsThetaPrompt->Fill(vSum.M(), vSum.Theta()*180/TMath::Pi());

                        double EffHelBeam = 0;
                        double EffHel = 0;
                        if ( (MyEvent.HelicityBit & 0x1E) == 12 ) {
                            EffHelBeam = +1;
                        } else if ( (MyEvent.HelicityBit & 0x1E) == 18) {
                            EffHelBeam = -1;
                        } else {
                            if (MyEvent.HelicityBit != -1) {
                                Printf("WARNING: Unexpected Helicity Bit: %d",MyEvent.HelicityBit);
                            }
                        }
                        //if ( (EffHelBeam == 0) && (RequireBeamHelicityPresent) ) {NRejectedEvents++;} //No longer necessary because faulty files do not get used for F later


//                        if ( TMath::Abs(MassPion0-vSum.M()) < 200 ) { //200 for tests to see the ful Inv Mass Distribution
                        if ( TMath::Abs(MassPion0-vSum.M()) < 20 ) {
                            //******** Kin Fit Start ******

                            TA2CBKinematicFitter* KinFitter;
                            TA2KFParticle KFPhoton[2];
                            TA2KFParticle KFPi0;

                            KinFitter = new TA2CBKinematicFitter(2, 1, 0);

                            KFPhoton[0].Set4Vector(v1);
                            KFPhoton[1].Set4Vector(v2);
                            KFPhoton[0].SetResolutions(3.*TMath::DegToRad(), 3.*TMath::DegToRad()/sin(v1.Theta()), 0.02*v1.E()/(TMath::Power(v1.E()/1000., -1./4.))); //GetSigmaTheta(), GetSigmaPhi(), GetSigmaE()
                            KFPhoton[1].SetResolutions(3.*TMath::DegToRad(), 3.*TMath::DegToRad()/sin(v2.Theta()), 0.02*v2.E()/(TMath::Power(v2.E()/1000., -1./4.))); //GetSigmaTheta(), GetSigmaPhi(), GetSigmaE()

                            KinFitter->Reset();
                            KinFitter->AddPosKFParticle(KFPhoton[0]);
                            KinFitter->AddPosKFParticle(KFPhoton[1]);
                            KinFitter->AddInvMassConstraint(MassPion0);
                            KinFitter->Solve();

                            KFPi0 = KinFitter->GetTotalFitParticle();

                            TLorentzVector MesonCorr;
                            MesonCorr = KFPi0.Get4Vector();
  /*                          printf("a: E     = %f -> %f\n", vSum.E(), MesonCorr.E());
                            printf("a: theta = %f -> %f\n", vSum.Theta()*180/3.141, MesonCorr.Theta()*180/3.141);
                            printf("a: phi   = %f -> %f\n", vSum.Phi()*180/3.141, MesonCorr.Phi()*180/3.141);
                            printf("a: m     = %f -> %f\n", vSum.M(), MesonCorr.M());
                            printf("\n");
*/                            hKinFitPulls->Fill(KinFitter->Pull(0));

                            hInvariant2GammaMassVsThetaPromptKinFit->Fill(MesonCorr.M(), MesonCorr.Theta()*180/TMath::Pi());

                            //******** Kin Fit End ********


                            //SetParticleMass(&vSum, MassPion0); //Set the Mass of the Meson correctly and adapt the 4vector
                            vSum = MesonCorr;
                            delete KinFitter;

                            hMesonInvariantMassCorrected->Fill(vSum.M());
                            hMesonPhi_VS_EventID->Fill(MyEvent.EventID, vSum.Phi()*180/TMath::Pi());
                            hMesonThetaLab_VS_EventID->Fill(MyEvent.EventID, vSum.Theta()*180/TMath::Pi());

                            TLorentzVector P4Missing, P4MesonCM;
                            if (RunsMetaInformation.at(IndexRunMetaInfomation).BeamTimeBlock == 1) {
                                EffHelBeam = EffHelBeam * -1; //For Feb beamtime the helicity bit was flipped.
                            }
                            for (int g=0;g<IncomingPhotons.size();g++) {
                                P4Missing = LVTarget + IncomingPhotons.at(g) - vSum;
                                MyBoostVector = -1. * (LVTarget + IncomingPhotons.at(g)).BoostVector();
                                P4MesonCM = vSum;
                                P4MesonCM.Boost(MyBoostVector);
                                double MesonTaggTimeDiff = MesonTime-IncomingPhotonsTime.at(g);

                                //Fill Debug histogram start
                                if ( (MesonTaggTimeDiff >= -12.) && (MesonTaggTimeDiff <= 8.) ) {
                                    hMesonInvMassVSMissingMass_Prompt->Fill(vSumBeforeFit.M(), P4Missing.M());
                                }
                                if ( ( (MesonTaggTimeDiff >= -130.) && (MesonTaggTimeDiff <= -30.) ) || ( (MesonTaggTimeDiff >= 30.) && (MesonTaggTimeDiff <= 130.) ) ) {
                                    hMesonInvMassVSMissingMass_Bg->Fill(vSumBeforeFit.M(), P4Missing.M());
                                }
                                //FIll Debug histogram end

                                if ( TMath::Abs(P4Missing.M()-MassProton) < 50 ) {

                                    hMesonInvariantMassAfterCuts->Fill(vSumBeforeFit.M());

                                    //Signal
                                    if ( (MesonTaggTimeDiff >= -12.) && (MesonTaggTimeDiff <= 8.) ) {
                                        hTimeMesonTagger->Fill(MesonTaggTimeDiff);

                                        #ifdef DO_hMissingMassPrompt
                                            hMissingMassPrompt.at(IncomingPhotonsTaggerCh.at(g))->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
                                        #endif
                                        hMissingMassVsMesonThetaPrompt->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
                                        hCBClusterMulti_VS_Energy_Photon_Prompt->Fill(MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(k).Energy);
                                        hCBClusterMulti_VS_Energy_Photon_Prompt->Fill(MyEvent.CBClusters.at(l).NumberOfCrystals, MyEvent.CBClusters.at(l).Energy);

                                        hAngleBetweenClusters->Fill(
                                                    GetAngleBetweenTwoVectors(MyEvent.CBClusters.at(k).Position, MyEvent.CBClusters.at(l).Position)*180/TMath::Pi(),
                                                    0
                                                    );
                                        hEnergyPhoton1VSEnergyPhoton2_Prompt->Fill(MyEvent.CBClusters.at(k).Energy, MyEvent.CBClusters.at(l).Energy);
                                        ///MyEvent.HelicityBit
                                        ///

                                        // Calculation for F
                                        //Difference in \phi between Target and pi0
                                        double ReactionAngle = cos( P4MesonCM.Phi() - TMath::Pi()/2 * RunsMetaInformation.at(IndexRunMetaInfomation).TargetNMRSignal * RunsMetaInformation.at(IndexRunMetaInfomation).TargetOrient );
                                        EffHel = EffHelBeam * ReactionAngle;

                                        if (EffHel >= 0.35) {
                                            hMissingMassCombinedPromptFP->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolF->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsF->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                            hCBEnergySumFP->Fill(CBEnergySum);
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedPromptFM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolF->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsF->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                            hCBEnergySumFM->Fill(CBEnergySum);
                                        }


                                        // Calculation for T
                                        //Difference in \phi between Target and pi0
                                        ReactionAngle = sin( P4MesonCM.Phi() - TMath::Pi()/2 * RunsMetaInformation.at(IndexRunMetaInfomation).TargetNMRSignal * RunsMetaInformation.at(IndexRunMetaInfomation).TargetOrient );
                                        EffHel = ReactionAngle;

                                        if (EffHel >= 0.35) {
                                            hMissingMassCombinedPromptTP->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                            hCBEnergySumTP->Fill(CBEnergySum);
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedPromptTM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                            hCBEnergySumTM->Fill(CBEnergySum);
                                        }

                                        //////////////////////////////////////

                                        hMissingMassCombinedPrompt->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());

                                        if (EffHelBeam != 0.) { //To reject events with wrong MAMI Bit Pattern
                                            hBeamPol->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).BeamPolDegree, 1.);
                                        }
                                        hTaggEffAbsAll->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.); //Mesons without EffHel cut
                                    }

                                    //Background
                                    if ( ( (MesonTaggTimeDiff >= -130.) && (MesonTaggTimeDiff <= -30.) ) || ( (MesonTaggTimeDiff >= 30.) && (MesonTaggTimeDiff <= 130.) ) ) {
                                        hTimeMesonTagger->Fill(MesonTaggTimeDiff);

                                        #ifdef DO_hMissingMassPrompt
                                            hMissingMassBg.at(IncomingPhotonsTaggerCh.at(g))->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
                                        #endif
                                        hMissingMassVsMesonThetaBg->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
                                        hCBClusterMulti_VS_Energy_Photon_Bg->Fill(MyEvent.CBClusters.at(k).NumberOfCrystals, MyEvent.CBClusters.at(k).Energy);
                                        hCBClusterMulti_VS_Energy_Photon_Bg->Fill(MyEvent.CBClusters.at(l).NumberOfCrystals, MyEvent.CBClusters.at(l).Energy);
                                        hEnergyPhoton1VSEnergyPhoton2_Bg->Fill(MyEvent.CBClusters.at(k).Energy, MyEvent.CBClusters.at(l).Energy);

                                        //Calculation for F
                                        //Difference in \phi between Target and pi0
                                        double ReactionAngle = cos( P4MesonCM.Phi() - TMath::Pi()/2 * RunsMetaInformation.at(IndexRunMetaInfomation).TargetNMRSignal * RunsMetaInformation.at(IndexRunMetaInfomation).TargetOrient );
                                        EffHel = EffHelBeam * ReactionAngle;

                                        if (EffHel >= 0.35) {
                                            hMissingMassCombinedBgFP->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolF->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle, -.1);

                                            hTaggEffAbsF->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, -0.1);
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedBgFM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolF->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle, -.1);

                                            hTaggEffAbsF->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, -0.1);
                                        }


                                        //Calculation for T
                                        //Difference in \phi between Target and pi0
                                        ReactionAngle = sin( P4MesonCM.Phi() - TMath::Pi()/2 * RunsMetaInformation.at(IndexRunMetaInfomation).TargetNMRSignal * RunsMetaInformation.at(IndexRunMetaInfomation).TargetOrient );
                                        EffHel = ReactionAngle;

                                        if (EffHel >= 0.35) {
                                            hMissingMassCombinedBgTP->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle, -.1);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, -0.1);
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedBgTM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle, -.1);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, -0.1);
                                        }

                                        ////////////////////////////////////////////

                                        hMissingMassCombinedBg->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());

                                        if (EffHelBeam != 0.) { //To reject events with wrong MAMI Bit Pattern
                                            hBeamPol->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).BeamPolDegree, -0.1);
                                        }
                                        hTaggEffAbsAll->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, -0.1); //Mesons without EffHel cut

                                    }
                                }
                            }

                        }
                    }

                    l++;
                }
                k++;
            }


        }

        i++;
    }

    if (NRejectedEvents>0) {
        printf("Dropped Events in Physics Analysis %d from %d: ", NRejectedEvents, NTotalEvents);
    }

    Handle_ScalerCounts(NRejectedEvents, NTotalEvents);

    #ifdef DO_hMissingMassPrompt
        for (i=0;i<NTaggerElements;i++) {
            hMissingMassSignal.at(i)->Add(hMissingMassPrompt.at(i), hMissingMassBg.at(i), 1, -0.1);
        }
    #endif
    hMissingMassVsMesonThetaSignal->Add(hMissingMassVsMesonThetaPrompt, hMissingMassVsMesonThetaBg, 1, -0.1);
    hMissingMassCombinedSignal->Add(hMissingMassCombinedPrompt, hMissingMassCombinedBg, 1, -0.1);

    //F
    hMissingMassCombinedSignalFP->Add(hMissingMassCombinedPromptFP, hMissingMassCombinedBgFP, 1, -0.1);
    hMissingMassCombinedSignalFM->Add(hMissingMassCombinedPromptFM, hMissingMassCombinedBgFM, 1, -0.1);

    //T
    hMissingMassCombinedSignalTP->Add(hMissingMassCombinedPromptTP, hMissingMassCombinedBgTP, 1, -0.1);
    hMissingMassCombinedSignalTM->Add(hMissingMassCombinedPromptTM, hMissingMassCombinedBgTM, 1, -0.1);

    //MissingMass VS InvMass
    hMesonInvMassVSMissingMass_Signal->Add(hMesonInvMassVSMissingMass_Prompt, hMesonInvMassVSMissingMass_Bg, 1, -0.1);
    //Cluster E VS Cluster Multi
    hCBClusterMulti_VS_Energy_Photon_Signal->Add(hCBClusterMulti_VS_Energy_Photon_Prompt, hCBClusterMulti_VS_Energy_Photon_Bg, 1, -0.1);
    //More
    hEnergyPhoton1VSEnergyPhoton2_Signal->Add(hEnergyPhoton1VSEnergyPhoton2_Prompt, hEnergyPhoton1VSEnergyPhoton2_Bg, 1, -0.1);

    return 0;
}



int Do_FinalPhysicsAnalysis () {
    printf("INFO: Do_FinalPhysicsAnalysis Start\n");

    //Calculate Errors
    hTaggerScalerAccum->Sumw2();
    hLiveTimeAccum->Sumw2();
    hTriggerScalerAccum->Sumw2();


    gROOT->mkdir("PostRunPhysics");
    gROOT->cd("PostRunPhysics");
    //CB Livetime correction
    hMissingMassCombinedSignalLTCorrected = (TH2D*)hMissingMassCombinedSignal->Clone("MissingMassCombinedSignalLTCorrected");
    hMissingMassCombinedSignalLTCorrected->Scale(1.*GetNotDroppedEventsRatio()/GetExpLiveTimeAccRatio());
    //F
    hMissingMassCombinedSignalLTCorrectedFP = (TH2D*)hMissingMassCombinedSignalFP->Clone("MissingMassCombinedSignalLTCorrectedFP");
    hMissingMassCombinedSignalLTCorrectedFP->Scale(1.*GetNotDroppedEventsRatio()/GetExpLiveTimeAccRatio());
    hMissingMassCombinedSignalLTCorrectedFM = (TH2D*)hMissingMassCombinedSignalFM->Clone("MissingMassCombinedSignalLTCorrectedFM");
    hMissingMassCombinedSignalLTCorrectedFM->Scale(1.*GetNotDroppedEventsRatio()/GetExpLiveTimeAccRatio());
    //T
    hMissingMassCombinedSignalLTCorrectedTP = (TH2D*)hMissingMassCombinedSignalTP->Clone("MissingMassCombinedSignalLTCorrectedTP");
    hMissingMassCombinedSignalLTCorrectedTP->Scale(1.*GetNotDroppedEventsRatio()/GetExpLiveTimeAccRatio());
    hMissingMassCombinedSignalLTCorrectedTM = (TH2D*)hMissingMassCombinedSignalTM->Clone("MissingMassCombinedSignalLTCorrectedTM");
    hMissingMassCombinedSignalLTCorrectedTM->Scale(1.*GetNotDroppedEventsRatio()/GetExpLiveTimeAccRatio());

    //Tagger Livetime correction
    hTaggerScalerAccumLTCorrectedAllEvents = (TH1D*)hTaggerScalerAccum->Clone("TaggerScalerAccumLTCorrectedInclDroppedEvents");
    hTaggerScalerAccumLTCorrectedAllEvents->Scale(1./GetTaggerLiveTimeAccRatio());

    hTaggerScalerAccumLTCorrected = (TH1D*)hTaggerScalerAccum->Clone("TaggerScalerAccumLTCorrected");
    hTaggerScalerAccumLTCorrected->Scale(1.*GetNotDroppedEventsRatio()/GetTaggerLiveTimeAccRatio());

    //Photons
    hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff = (TH1D*)hTaggerScalerAccumLTCorrected->Clone("PhotonFluxLTCorrectedWOTaggEff");
    hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff->Scale(RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff);
    //correct for the TaggEff Measurement
    hTaggerScalerAccumPhotonsLTCorrected = (TH1D*)hTaggerScalerAccumPhotonsLTCorrectedWOTaggEff->Clone("PhotonFluxLTCorrected");
    if (strlen(TaggEffNormFile) != 0) {
        Printf("INFO: Open TaggEffNorm file: %s", TaggEffNormFile);
        TFile *f1 = new TFile(TaggEffNormFile);
        TH1D *h1 = (TH1D*)f1->Get("TaggEffMeanNorm");
        Printf("INFO: Do the TaggEffNorm calculation.");
        hTaggerScalerAccumPhotonsLTCorrected->Multiply(h1);
        f1->Close();
    } else {
        printf("WARNING: No TaggEffFile in Tagger configuration file specified. Skipping the TaggEffMeanNorm correction.\nINFO: This is normal for TaggEff runs.\n");
    }

    //Determine the Flux for Dropped Events, for F Observable
    gROOT->mkdir("PostRunPhysics");
    gROOT->cd("PostRunPhysics");

    double fCorrectionRatio = 1;
    fCorrectionRatio = 1. - hDroppedEvents->GetBinContent(1) / hDroppedEvents->GetBinContent(2);
    printf("INFO: Correction Ratio for dropped events: %f\n",fCorrectionRatio);


    printf("INFO: Do_FinalPhysicsAnalysis End\n");
    return 0;
}


