#!/bin/bash

hadd -f output/sumButA.root /datapool/2010_09/outputA/*.root
hadd -f output/sumButB.root /datapool/2010_09/outputB/*.root
hadd -f output/sumButC.root /datapool/2010_09/outputC/*.root
hadd -f output/sumButD.root /datapool/2010_09/outputD/*.root

hadd -f output/sumButE.root /datapool/2011_02/outputE/*.root
hadd -f output/sumButF.root /datapool/2011_02/outputF/*.root
hadd -f output/sumButG.root /datapool/2011_02/outputG/*.root
hadd -f output/sumButH.root /datapool/2011_02/outputH/*.root

#Jan 2011
hadd -f output/sumButI.root /datapool/2011_01/outputI/*.root
hadd -f output/sumButJ.root /datapool/2011_01/outputJ/*.root
