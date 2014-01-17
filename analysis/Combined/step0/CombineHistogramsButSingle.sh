#!/bin/bash

hadd -f output/sumButA.root /datapool/2010_09/outputA/*.dat.root
hadd -f output/sumButB.root /datapool/2010_09/outputB/*.dat.root
hadd -f output/sumButC.root /datapool/2010_09/outputC/*.dat.root
hadd -f output/sumButD.root /datapool/2010_09/outputD/*.dat.root

hadd -f output/sumButE.root /datapool/2011_02/outputE/*.dat.root
hadd -f output/sumButF.root /datapool/2011_02/outputF/*.dat.root
hadd -f output/sumButG.root /datapool/2011_02/outputG/*.dat.root
hadd -f output/sumButH.root /datapool/2011_02/outputH/*.dat.root
