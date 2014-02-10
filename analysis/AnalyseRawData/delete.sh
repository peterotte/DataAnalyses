#!/bin/bash

echo "Delete Sep 2010 output"
cd /datapool/2010_09
./delete.sh

echo "Delete Feb 2011 output"
cd /datapool/2011_02
./delete.sh

echo "Delete May 2013 output"
cd /datapool/2013_05_pi0
./delete.sh

echo "Done."