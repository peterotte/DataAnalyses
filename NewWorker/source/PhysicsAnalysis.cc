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
int GetExperimentLiveCounter() {
    return EventBlock.ExperimentLiveCounter;
}
int GetUngatedLiveCounter() {
    return EventBlock.UngatedLiveCounter;
}
int GetTaggerLiveCounter() {
    return EventBlock.TaggerLiveCounter;
}

double GetExpLiveTimeRatio() {
    return (1.*GetExperimentLiveCounter() / (1.*GetUngatedLiveCounter()));
}

//For complete Run, accumulated
double GetExperimentLiveCounterAcc() {
    return hLiveTimeAccum->GetBinContent(2);
}
double GetUngatedLiveCounterAcc() {
    return hLiveTimeAccum->GetBinContent(1);
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
//-------------------------------------------------------------------------------


int Handle_ScalerCounts(int fNRejectedEvents, int fNTotalEvents) {
    if (LastScalerReadNCh != EventBlock.Scalers.size()) {
        //Show this warning only after the first EventBlock
        if (LastScalerReadNCh >= 0) Printf("ERROR: Number of Scalers differs to last EventBlock. %d %d", LastScalerReadNCh, EventBlock.Scalers.size());
    }
    LastScalerReadNCh = EventBlock.Scalers.size();

    int i = 0;
    while (i<EventBlock.Scalers.size()) {
        if (EventBlock.Scalers.at(i).DetectorID == 0) {
            hTaggerScalerAccum->Fill(EventBlock.Scalers.at(i).ElementID, EventBlock.Scalers.at(i).Value);
        } else if (EventBlock.Scalers.at(i).DetectorID == 10) {
            hTriggerScalerAccum->Fill(EventBlock.Scalers.at(i).ElementID, EventBlock.Scalers.at(i).Value);
        }
        i++;
    }

    double CorrectionFactor = (1.-(double)fNRejectedEvents/(double)fNTotalEvents);
    hLiveTimeAccum->Fill(0.5, GetUngatedLiveCounter()); //Ungated
    hLiveTimeAccum->Fill(1.5, GetExperimentLiveCounter() * CorrectionFactor ) ; //CB Gated, corrected by the dropped Events = dead time
    hLiveTimeAccum->Fill(2.5, GetTaggerLiveCounter() * CorrectionFactor ); //Tagger Gated

    if (CorrectionFactor <= 1) printf("LiveTime corrected by: %f\n", CorrectionFactor);

    hDroppedEvents->Fill(0.5, fNRejectedEvents);
    hDroppedEvents->Fill(1.5, fNTotalEvents);
}


int Do_PhysicsAnalysis () {
    TVector3 MyBoostVector;

    int NTotalEvents = 0;
    int NRejectedEvents = 0; //Due to wrong MAMI Bitpattern etc.

    int i = 0;
    while (i<EventBlock.Events.size()) { //Loop through all events in EventBlock
        TEvent MyEvent;
        MyEvent = EventBlock.Events.at(i);

        ///printf("New Event ID: ??\tTotalN:%d\n", MyEvent.CBClusters.size());

        NTotalEvents++;
        if (MyEvent.NErrorBlocks > 0) {
            //printf("WARNING: EventID: %d has NErrorBlocks: %d\n", MyEvent.EventID, MyEvent.NErrorBlocks);
            NRejectedEvents++;
            i++;
            continue;
            printf("This SHOULD NEVER HAPPEN!\n");
        };

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
        }
        hCBEnergySum->Fill(CBEnergySum);
        hCBEnergySum_VS_EventID->Fill(MyEvent.EventID, CBEnergySum);


        hNPhotons->Fill(MyEvent.CBClusters.size());

        if (MyEvent.CBClusters.size() > 1) {
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
                        if ( (EffHelBeam == 0) && (RequireBeamHelicityPresent) ) {NRejectedEvents++;}


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
                                if ( TMath::Abs(P4Missing.M()-MassProton) < 50 ) {

                                    double MesonTaggTimeDiff = MesonTime-IncomingPhotonsTime.at(g);

                                    //Signal
                                    if ( (MesonTaggTimeDiff >= -12.) && (MesonTaggTimeDiff <= 8.) ) {
                                        hTimeMesonTagger->Fill(MesonTaggTimeDiff);

                                        #ifdef DO_hMissingMassPrompt
                                            hMissingMassPrompt.at(IncomingPhotonsTaggerCh.at(g))->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
                                        #endif
                                        hMissingMassVsMesonThetaPrompt->Fill(P4Missing.M(), P4MesonCM.Theta()*180/TMath::Pi());
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
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedPromptFM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolF->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsF->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                        }


                                        // Calculation for T
                                        //Difference in \phi between Target and pi0
                                        ReactionAngle = sin( P4MesonCM.Phi() - TMath::Pi()/2 * RunsMetaInformation.at(IndexRunMetaInfomation).TargetNMRSignal * RunsMetaInformation.at(IndexRunMetaInfomation).TargetOrient );
                                        EffHel = ReactionAngle;

                                        if (EffHel >= 0.35) {
                                            hMissingMassCombinedPromptTP->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
                                        } else if (EffHel <= -0.35) {
                                            hMissingMassCombinedPromptTM->Fill(IncomingPhotonsTaggerCh.at(g), P4MesonCM.Theta()*180/TMath::Pi());
                                            hTargetPolT->Fill(IncomingPhotonsTaggerCh.at(g), TMath::Abs(RunsMetaInformation.at(IndexRunMetaInfomation).TargetPolDegree) * ReactionAngle);

                                            hTaggEffAbsT->Fill(IncomingPhotonsTaggerCh.at(g), RunsMetaInformation.at(IndexRunMetaInfomation).AbsTaggEff, 1.);
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
        printf("Dropped Events in Physics Analysis %d from %d\n", NRejectedEvents, NTotalEvents);
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
    hMissingMassCombinedSignalLTCorrected->Scale(1./GetExpLiveTimeAccRatio());
    //F
    hMissingMassCombinedSignalLTCorrectedFP = (TH2D*)hMissingMassCombinedSignalFP->Clone("MissingMassCombinedSignalLTCorrectedFP");
    hMissingMassCombinedSignalLTCorrectedFP->Scale(1./GetExpLiveTimeAccRatio());
    hMissingMassCombinedSignalLTCorrectedFM = (TH2D*)hMissingMassCombinedSignalFM->Clone("MissingMassCombinedSignalLTCorrectedFM");
    hMissingMassCombinedSignalLTCorrectedFM->Scale(1./GetExpLiveTimeAccRatio());
    //T
    hMissingMassCombinedSignalLTCorrectedTP = (TH2D*)hMissingMassCombinedSignalTP->Clone("MissingMassCombinedSignalLTCorrectedTP");
    hMissingMassCombinedSignalLTCorrectedTP->Scale(1./GetExpLiveTimeAccRatio());
    hMissingMassCombinedSignalLTCorrectedTM = (TH2D*)hMissingMassCombinedSignalTM->Clone("MissingMassCombinedSignalLTCorrectedTM");
    hMissingMassCombinedSignalLTCorrectedTM->Scale(1./GetExpLiveTimeAccRatio());

    //Tagger Livetime correction
    hTaggerScalerAccumLTCorrected = (TH1D*)hTaggerScalerAccum->Clone("TaggerScalerAccumLTCorrected");
    hTaggerScalerAccumLTCorrected->Scale(1./GetTaggerLiveTimeAccRatio());

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


