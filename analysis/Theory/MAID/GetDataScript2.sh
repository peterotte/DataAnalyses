echo -n "http://wwwkph.kph.uni-mainz.de/cgi-bin/maid1?switch=215&param2=1&param3=1&param50=3&value35=0&value36=" > MAIDURL.txt
#echo -n "1200.0" >> MAIDURL.txt
echo -n $1 >> MAIDURL.txt
echo -n "&value37=0&value41=1&value42=180&param99=0&param11=1&param12=1&param13=1&param14=1&param15=1&param16=1&param17=1&param18=1&param19=1&param20=1&param21=1&param22=1&param23=1&param24=1&param25=1&param26=1&value11=1.0&value12=1.0&value13=1.0&value51=1.0&value52=1.0&value53=1.0&value54=1.0&value55=1.0&value56=1.0&value57=1.0&value58=1.0&value59=1.0&value60=1.0&value61=1.0&value62=1.0&value63=1.0&value64=1.0&value65=1.0&value66=1.0&value67=1.0&value68=1.0&value69=1.0&value70=1.0&value71=1.0&value72=1.0&value73=1.0&value74=1.0&value75=1.0&value76=1.0&value77=1.0&value78=1.0&value79=1.0&value80=1.0&value81=1.0&value82=1.0&value83=1.0&value84=1.0" >> MAIDURL.txt


wget -i MAIDURL.txt -o log -O - > MAIDResponse.txt
mv MAIDResponse.txt MAIDData/MAIDValues_ch$2.dat
echo "Got Data from MAID."

