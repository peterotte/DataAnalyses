
echo -n "Getting F and T for W=$1... "
echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=F&iv=A&il=0&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" > URL.txt
echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=F&iv=A&il=1&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" >> URL.txt
echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=F&iv=A&il=2&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" >> URL.txt

echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=T&iv=A&il=0&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" >> URL.txt
echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=T&iv=A&il=1&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" >> URL.txt
echo "http://gwdac.phys.gwu.edu/cgi-bin/go3pr?sl=SN11&rt=1&ot=T&iv=A&il=2&ii=3&iu=180&fv=W&fn="$1"&jpeg=GO99&u=995&l=1005" >> URL.txt

wget -i URL.txt -o log -O - > MAIDResponse.txt
mv MAIDResponse.txt MAIDData/MAIDValues_ch$2.dat
echo "Got Data from SAID."

