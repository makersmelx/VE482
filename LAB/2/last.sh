curl --silent 'http://aqicn.org/?city=Shanghai&widgetscript&size=large' | sed 's/^.*title\=\\"Moderate\\">/AQ: /g' | sed "s/<.*$//g" | sed -n "3p"
curl --silent 'http://aqicn.org/?city=Shanghai&widgetscript&size=large' | sed "s/.*style\='font-size:10px;'>/Temp: /g" | sed 's/<.*$//g' | sed -n '3p'
