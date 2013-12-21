

int Do_FillNHitsHistograms() {
    int i=0;
    while (i < EventBlock.Events.size()) { //Loop through all events

        int k=0;
        int NTagger = 0;
        int NCB = 0;
        int NPID = 0;

        while (k<EventBlock.Events.at(i).HitElements.size()) {
            int AktElementNr = EventBlock.Events.at(i).HitElements.at(k).ElementID;

            switch (EventBlock.Events.at(i).HitElements.at(k).DetectorID) {
            case DetectorIDTagger:
                hTaggerNMultiHitsCuts->Fill(EventBlock.Events.at(i).HitElements.at(k).Time.size(), AktElementNr);
                NTagger = NTagger + EventBlock.Events.at(i).HitElements.at(k).Time.size();
                break;
            case DetectorIDCB:
                hCBNMultiHitsCuts->Fill(EventBlock.Events.at(i).HitElements.at(k).Time.size(), AktElementNr);
                NCB = NCB + EventBlock.Events.at(i).HitElements.at(k).Time.size();
                break;
            case DetectorIDPID:
                hPIDNMultiHitsCuts->Fill(EventBlock.Events.at(i).HitElements.at(k).Time.size(), AktElementNr);
                NPID = NPID + EventBlock.Events.at(i).HitElements.at(k).Time.size();
                break;
            }

            k++;
        }
        hTaggerNHits->Fill(NTagger);
        hCBNHits->Fill(NCB);
        hPIDNHits->Fill(NPID);

        i++;
    }

}
