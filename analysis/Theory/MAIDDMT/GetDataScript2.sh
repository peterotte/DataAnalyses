echo "http://wwwkph.kph.uni-mainz.de/cgi-bin/maid1?switch=105&param0=1&param1=1&param11=1&param12=1&param13=1&param14=1&param15=1&param16=1&param17=1&param18=1&param19=1&param20=1&param21=1&value51=1.0&value52=1.0&value53=1.0&value54=1.0&value55=0.0&value56=1.0&value57=1.0&value58=0.0&value59=1.0&value60=1.0&value61=1.0&value62=1.0&value63=0.0&value64=1.0&value65=0.0&value66=1.0&value67=0.0&value68=1.0&value69=1.0&value70=0.0&param50=3&value35=0&value36="$1"&value37=0&value41=1&value42=180" > MAIDURL.txt
echo "http://wwwkph.kph.uni-mainz.de/cgi-bin/maid1?switch=105&param0=1&param1=1&param11=1&param12=1&param13=1&param14=1&param15=1&param16=1&param17=1&param18=1&param19=1&param20=1&param21=1&value51=1.0&value52=1.0&value53=1.0&value54=1.0&value55=0.0&value56=1.0&value57=1.0&value58=0.0&value59=1.0&value60=1.0&value61=1.0&value62=1.0&value63=0.0&value64=1.0&value65=0.0&value66=1.0&value67=0.0&value68=1.0&value69=1.0&value70=0.0&param50=3&value35=0&value36="$1"&value37=100&value41=1&value42=180" >> MAIDURL.txt



wget -i MAIDURL.txt -o log -O - > MAIDResponse.txt
mv MAIDResponse.txt MAIDData/MAIDValues_ch$2.dat
echo "Got Data from MAIDDMT."

